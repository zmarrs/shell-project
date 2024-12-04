# Simple Shell Program

 Zachary Marrs
CS 461 P 001 - Operating Systems
Professor Essa Imhmed
11/22/2024

This is a simple shell program written in C++ as part of an assignment for an Operating Systems course at CNM. The program implements basic shell functionality such as reading commands from the user, parsing the input, and executing commands by creating child processes. It is the start of a larger shell program that will be impleneted over further assignments throughout the semester. Functionality has been extended to include I/O redirection, allowing standard output to be redirected to an output file. Additional functionality added to include the ability to send output to another program.  

## Files Included

- `SimpleShell.h`: Header file containing the definition of the `SimpleShell` class and its methods.
- `SimpleShell.cpp`: Source file containing the implementation of the `SimpleShell` class, updated to include all commands.
- `Makefile`: File used to compile and run the program via the `make` command.
- `test.sh`: A bash script which runs input tests on expected inputed shell commands including invalid commands and logs the test results.

## How to Compile and Run

In a terminal, navigate to the project folder and run the following commands 

```bash
make  # Compiles the program and then runs the executable
make compile  # Compiles the source code into an executable located in the bin/ directory
make run  # Executes the compiled program
make clean  # Deletes the object files and the executable from the bin/ directory
