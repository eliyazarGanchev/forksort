#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINES 10000
#define MAX_LEN 1024

// Function to merge sorted parts
void merge(char *left[], int left_size, char *right[], int right_size) {
    int i = 0, j = 0;
    while (i < left_size && j < right_size) {
        if (strcmp(left[i], right[j]) <= 0)
            printf("%s", left[i++]);
        else
            printf("%s", right[j++]);
    }
    while (i < left_size) printf("%s", left[i++]);
    while (j < right_size) printf("%s", right[j++]);
}

int main() {
    char *lines[MAX_LINES];
    int count = 0;
    char buffer[MAX_LEN];
    
    // Read input lines
    while (fgets(buffer, MAX_LEN, stdin)) {
        lines[count] = strdup(buffer);
        count++;
    }

    // Base case: only one line, print and exit
    if (count == 1) {
        printf("%s", lines[0]);
        free(lines[0]);
        return EXIT_SUCCESS;
    }

    // Split input into two parts
    int mid = count / 2;
    int fd1[2], fd2[2];
    pipe(fd1);
    pipe(fd2);

    pid_t pid1 = fork();
    if (pid1 == 0) {  // First child
        close(fd1[0]);
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[1]);
        for (int i = 0; i < mid; i++) printf("%s", lines[i]);
        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {  // Second child
        close(fd2[0]);
        dup2(fd2[1], STDOUT_FILENO);
        close(fd2[1]);
        for (int i = mid; i < count; i++) printf("%s", lines[i]);
        exit(EXIT_SUCCESS);
    }

    close(fd1[1]);
    close(fd2[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    // Read sorted output from children
    char *sorted_left[MAX_LINES], *sorted_right[MAX_LINES];
    int left_count = 0, right_count = 0;
    while (fgets(buffer, MAX_LEN, fdopen(fd1[0], "r"))) sorted_left[left_count++] = strdup(buffer);
    while (fgets(buffer, MAX_LEN, fdopen(fd2[0], "r"))) sorted_right[right_count++] = strdup(buffer);

    merge(sorted_left, left_count, sorted_right, right_count);
    
    return EXIT_SUCCESS;
}