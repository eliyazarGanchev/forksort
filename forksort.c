#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINES 10000
#define MAX_LEN 1024

typedef struct {
    pid_t pid;
    int stdin_pipe[2];
    int stdout_pipe[2];
} child_t;

int init_pipe(child_t *child) {
    if (pipe(child->stdin_pipe) == -1) return -1;
    if (pipe(child->stdout_pipe) == -1) {
        close(child->stdin_pipe[0]);
        close(child->stdin_pipe[1]);
        return -1;
    }
    return 0;
}

int run_child(child_t *child) {
    if (init_pipe(child) == -1) return -1;

    pid_t pid = fork();
    if (pid == -1) return -1;

    if (pid == 0) { // Child process
        close(child->stdin_pipe[1]);
        close(child->stdout_pipe[0]);

        dup2(child->stdin_pipe[0], STDIN_FILENO);
        dup2(child->stdout_pipe[1], STDOUT_FILENO);

        execlp("./forksort", "forksort", NULL);
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(child->stdin_pipe[0]);
        close(child->stdout_pipe[1]);
    }
    return pid;
}

int read_input(char *lines[]) {
    size_t size = 0;
    char *buffer = NULL;
    int count = 0;

    while (getline(&buffer, &size, stdin) != -1) {
        lines[count] = strdup(buffer);
        count++;
    }

    free(buffer);
    return count;
}

void distribute_lines(char *lines[], int count, FILE *f1, FILE *f2) {
    int mid = count / 2;
    for (int i = 0; i < mid; i++) {
        fprintf(f1, "%s", lines[i]);
    }
    for (int i = mid; i < count; i++) {
        fprintf(f2, "%s", lines[i]);
    }
    fflush(f1);
    fflush(f2);
}

int read_sorted_output(FILE *child_file, char *sorted[]) {
    size_t size = 0;
    char *buffer = NULL;
    int count = 0;

    while (getline(&buffer, &size, child_file) != -1) {
        sorted[count++] = strdup(buffer);
    }

    free(buffer);
    return count;
}

void merge_sorted_output(char *sorted_left[], int left_count, char *sorted_right[], int right_count) {
    int i = 0, j = 0;

    while (i < left_count && j < right_count) {
        if (strcmp(sorted_left[i], sorted_right[j]) <= 0) {
            printf("%s", sorted_left[i++]);
        } else {
            printf("%s", sorted_right[j++]);
        }
    }

    while (i < left_count) printf("%s", sorted_left[i++]);
    while (j < right_count) printf("%s", sorted_right[j++]);

    fflush(stdout);
}

int main() {
    char *lines[MAX_LINES];
    int count = read_input(lines);

    if (count == 1) {
        printf("%s", lines[0]);
        free(lines[0]);
        exit(EXIT_SUCCESS);
    }

    child_t child1, child2;
    if ((child1.pid = run_child(&child1)) < 0 || (child2.pid = run_child(&child2)) < 0) {
        fprintf(stderr, "Error running child processes.\n");
        exit(EXIT_FAILURE);
    }

    FILE *f1 = fdopen(child1.stdin_pipe[1], "w");
    FILE *f2 = fdopen(child2.stdin_pipe[1], "w");

    if (!f1 || !f2) {
        fprintf(stderr, "Error opening pipes for writing.\n");
        exit(EXIT_FAILURE);
    }

    distribute_lines(lines, count, f1, f2);

    fclose(f1);
    fclose(f2);

    waitpid(child1.pid, NULL, 0);
    waitpid(child2.pid, NULL, 0);

    char *sorted_left[MAX_LINES], *sorted_right[MAX_LINES];
    FILE *child_f1 = fdopen(child1.stdout_pipe[0], "r");
    FILE *child_f2 = fdopen(child2.stdout_pipe[0], "r");

    if (!child_f1 || !child_f2) {
        fprintf(stderr, "Error opening pipes for reading.\n");
        exit(EXIT_FAILURE);
    }

    int left_count = read_sorted_output(child_f1, sorted_left);
    int right_count = read_sorted_output(child_f2, sorted_right);

    fclose(child_f1);
    fclose(child_f2);

    merge_sorted_output(sorted_left, left_count, sorted_right, right_count);

    for (int i = 0; i < left_count; i++) free(sorted_left[i]);
    for (int i = 0; i < right_count; i++) free(sorted_right[i]);

    return EXIT_SUCCESS;
}
