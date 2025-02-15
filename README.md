Forksort - Parallel Merge Sort Using Fork

Overview

This project implements a parallel merge sort using the fork system call in C. The program reads multiple lines from standard input, sorts them using a recursive merge sort approach, and outputs the sorted results. The sorting process is divided between two child processes using inter-process communication (IPC) via pipes.

Features

Implements a parallel merge sort using child processes.

Uses pipes for inter-process communication (IPC).

Efficiently distributes input data between child processes.

Ensures proper process management, including error handling.

Installation

To compile the program, run the following command:

make

This will generate the forksort executable.

Usage

Run the program by providing an input file containing multiple lines of text:

./forksort < input.txt

Alternatively, you can enter input manually and terminate with Ctrl+D:

./forksort

Example Input

Heinrich
Anton
Theodor
Dora
Hugo

Expected Output

Anton
Dora
Heinrich
Hugo
Theodor

Implementation Details

1. Process Management

The parent process creates two child processes using fork().

Each child process receives a portion of the input to sort independently.

The sorted results from both child processes are merged in the parent process.

2. Inter-Process Communication (IPC)

Pipes are used to send and receive data between the parent and child processes.

The parent writes input data to child processes via their respective pipes.

The child processes sort their data and write the results back to the parent.

3. Sorting Strategy

If only one line is received, it is immediately printed.

If multiple lines are received, they are split into two halves.

Each child process sorts its portion using qsort().

The parent merges the sorted results from the child processes.

4. Error Handling

The program includes error handling for the following scenarios:

Failed pipe creation.

Failed process creation (fork failure).

Failed execution of child process (execlp failure).

Issues with reading/writing to pipes.

Memory allocation failures.

5. Code Structure

The program consists of the following key functions:

init_pipe(): Initializes pipes for communication between processes.

run_child(): Creates child processes and redirects input/output.

read_input(): Reads input lines from stdin.

distribute_lines(): Splits input lines and sends them to child processes.

read_sorted_output(): Reads sorted output from child processes.

merge_sorted_output(): Merges the sorted results from both child processes.

Known Issues and Limitations

The program assumes that the input fits within the predefined buffer size (MAX_LINES and MAX_LEN).

Sorting is performed in-memory, which may cause issues for extremely large inputs.

License

This project is released under the MIT License.

Author

This implementation was written to demonstrate process-based parallel sorting using inter-process communication and merge sort.

