# Zachary Marrs
# CS 461 P 001 - Operating Systems 
# Professor Essa Imhmed 
# 12/3/2024

#!/bin/bash

# Path to shell executable
SHELL_EXECUTABLE="./bin/simpleshell"

# Check if the shell executable exists
if [ ! -f "$SHELL_EXECUTABLE" ]; then
    echo "Error: SimpleShell executable not found at $SHELL_EXECUTABLE."
    exit 1
fi

# Start a log file for capturing test results
LOGFILE="test_results.log"
echo "Testing SimpleShell" > "$LOGFILE"
echo "===================" >> "$LOGFILE"

# Function to run a command and log results
run_test() {
    TEST_DESC=$1
    COMMAND=$2
    EXPECTED=$3

    echo "Running test: $TEST_DESC" >> "$LOGFILE"
    echo "Command: $COMMAND" >> "$LOGFILE"

    OUTPUT=$($SHELL_EXECUTABLE <<< "$COMMAND" 2>&1)
    echo "Output: $OUTPUT" >> "$LOGFILE"

    if [[ "$OUTPUT" == *"$EXPECTED"* ]]; then
        echo "Result: PASS" >> "$LOGFILE"
    else
        echo "Result: FAIL" >> "$LOGFILE"
        echo "Expected: $EXPECTED" >> "$LOGFILE"
    fi
    echo >> "$LOGFILE"
}

# Run tests
run_test "Test basic command: ls" "ls" "src"
run_test "Test built-in command: pwd" "pwd" "/"
run_test "Test redirection: echo to file" "echo Hello > testfile && cat testfile" "Hello"
run_test "Test pipe: echo and tr" "echo hello | tr '[:lower:]' '[:upper:]'" "HELLO"
run_test "Test built-in cd command" "cd / && pwd" "/"
run_test "Test invalid command" "invalidcommand" "command not found"

# Cleanup temporary files
rm -f testfile

# Display test summary
echo "Test results saved to $LOGFILE"
cat "$LOGFILE"
