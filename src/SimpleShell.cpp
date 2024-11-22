/**
 *    Zachary Marrs
 *    CS 461 P 001 - Operating Systems
 *    Professor Essa Imhmed
 *    10/27/2024
 *
 *    ps2 - Simple Shell
 */
#include "../include/SimpleShell.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

/**
 * @brief Creates a simple shell that accepts commands from standard input and executes them if possible.
 *        and if the command is valid.
 */

/**
* Once command tokens are received, this function spawns a child process which executes any valid commands.
* It also supports piping between commands.
*
* @param   argv   The vector<string>& of commands inputed by the user, to be executed
*/
void SimpleShell::execute(const vector<string>& argv)
{
    int status;
    pid_t child;

    // Detect if there's a pipe in the command
    size_t pipe_pos = find(argv.begin(), argv.end(), "|") - argv.begin();
    
    // If pipe is found, split the command into two parts
    if (pipe_pos != argv.size()) {
        vector<string> left_cmd(argv.begin(), argv.begin() + pipe_pos);
        vector<string> right_cmd(argv.begin() + pipe_pos + 1, argv.end());
        
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) { // Create pipe
            perror("pipe failed");
            _exit(1);
        }

        // Handle the left command in a child process
        if ((child = fork()) == 0) {
            // Redirect stdout to pipe
            if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
                perror("Failed to redirect output to pipe");
                _exit(1);
            }
            close(pipe_fd[0]); // Close read end in the child process

            // Prepare arguments for execvp for left command
            vector<const char*> left_args;
            for (const string& token : left_cmd) {
                left_args.push_back(token.c_str());
            }
            left_args.push_back(nullptr);

            if (execvp(left_cmd[0].c_str(), const_cast<char* const*>(left_args.data())) == -1) {
                perror("execvp failed on left command");
                _exit(1);
            }
        }
        else { // Parent process waits for the left command to finish
            close(pipe_fd[1]); // Close write end in parent
            waitpid(child, &status, 0);
        }

        // Handle the right command in another child process
        if ((child = fork()) == 0) {
            // Redirect stdin from pipe
            if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
                perror("Failed to redirect input from pipe");
                _exit(1);
            }
            close(pipe_fd[1]); // Close write end in the child process

            // Prepare arguments for execvp for right command
            vector<const char*> right_args;
            for (const string& token : right_cmd) {
                right_args.push_back(token.c_str());
            }
            right_args.push_back(nullptr);

            if (execvp(right_cmd[0].c_str(), const_cast<char* const*>(right_args.data())) == -1) {
                perror("execvp failed on right command");
                _exit(1);
            }
        }
        else { // Parent process waits for the right command to finish
            close(pipe_fd[0]); // Close read end in parent
            waitpid(child, &status, 0);
        }
    }
    else {
        // If no pipe, handle regular command execution
        child = fork();
        
        if (child > 0) { // Parent process portion
            cout << "(" << getpid() << ") : I am a parent process waiting..." << endl;
            waitpid(child, &status, 0); // Wait for child process to finish
            cout << "Waiting complete" << endl;
        }
        else if (child == 0) { // Child process portion
            cout << "I am a child executing a new environment" << endl;
            
            if (argv.empty()) {
                cerr << "Error: No command to execute" << endl;
                _exit(1);
            }

            // Variables for file redirection
            int input_fd = -1;
            int output_fd = -1;
            
            // Prepare arguments for execvp (must end with nullptr)
            vector<const char*> args;

            // I/O Redirection
            for (size_t i = 0; i < argv.size(); ++i) {
                if (argv[i] == "<" && i + 1 < argv.size()) {
                    input_fd = open(argv[i + 1].c_str(), O_RDONLY);
                    if (input_fd == -1) {
                        perror("Failed to open input file");
                        _exit(1);
                    }
                    if (dup2(input_fd, STDIN_FILENO) == -1) {
                        perror("Failed to redirect input");
                        _exit(1);
                    }
                    i++; // Skip the filename
                } else if (argv[i] == ">" && i + 1 < argv.size()) {
                    output_fd = open(argv[i + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (output_fd == -1) {
                        perror("Failed to open output file");
                        _exit(1);
                    }
                    if (dup2(output_fd, STDOUT_FILENO) == -1) {
                        perror("Failed to redirect output");
                        _exit(1);
                    }
                    i++; // Skip the filename
                } else {
                    args.push_back(argv[i].c_str());
                }
            }
            
            args.push_back(nullptr);

            // Execute cd (change directory) command
            if (argv[0] == "cd") {
                if (argv.size() > 1) {
                    // Handle "~" for home directory expansion
                    string target_dir = argv[1]; // Copy argv[1] to a mutable string
                    if (target_dir.rfind("~/", 0) == 0) {
                        const char* home = getenv("HOME");
                        if (home == nullptr) {
                            cerr << "Error: HOME environment variable not set." << endl;
                            _exit(1);
                        }
                        target_dir = std::string(home) + target_dir.substr(1); // Replace ~ with home directory
                    }
                    if (chdir(target_dir.c_str()) == -1) {
                        perror("cd failed");
                        _exit(1);
                    }
                } else {
                    cerr << "cd: missing argument" << endl;
                    _exit(1);
                }
            }
            // Handle "pwd" to print working directory
            else if (argv[0] == "pwd") {
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                    cout << cwd << endl;
                } else {
                    perror("getcwd failed");
                }
                _exit(0);
            }
            // Handle "mkdir" to create directories
            else if (argv[0] == "mkdir") {
                if (argv.size() < 2) {
                    cerr << "mkdir: missing directory name" << endl;
                    _exit(1);
                }
                if (mkdir(argv[1].c_str(), 0777) == -1) {
                    perror("mkdir failed");
                    _exit(1);
                }
                _exit(0);
            }

            // Execute *any valid command* using execvp
            if (execvp(argv[0].c_str(), const_cast<char* const*>(args.data())) == -1) {
                perror("execvp failed");
                _exit(1);
            }
            
            // Close any opened files
            if (input_fd != -1) close(input_fd);
            if (output_fd != -1) close(output_fd);
        }
        else {
            perror("fork failed"); // Error handling if fork fails
            exit(1);
        }
    }
}

/**
* Parses the input string given by the user into tokens
*
* @param    line        The line of text inputed by the user
* @param    tokens      The command tokens
* @param    delimiter   The type of delimiter used to separate commands. (i.e. ",",";", or "-")
*/
void SimpleShell::parse(const string& line, vector<string>& tokens, const string& delimiter)
{
    size_t start = 0;
    size_t end = 0;
    
    while ((end = line.find(delimiter, start)) != string::npos) {
        if (end != start) { // Ignore empty tokens
            tokens.push_back(line.substr(start, end - start));
        }
        start = end + delimiter.length();
    }
    
    if (start < line.length()) { // Add the last token
        tokens.push_back(line.substr(start));
    }
}

/**
*  Runs the shell program loop.  Within the loop, the user is prompted for commands to input.
*  Next, the input from the keyboard is parsed by delimiters for command tokens to execute,
*  skipping any empty input.  If an exit command is received, the shell is terminated, otherwise,
*  any valid command tokens are executed.
*
*/
void SimpleShell::run()
{
    while (true) {
        string line;
        vector<string> tokens;
        
        // Print the prompt
        cout << "(" << getpid() << ") % ";
        
        // Get input from the keyboard
        if (!getline(cin, line)) {
            break; // Exit the shell if input fails (e.g., EOF)
        }
        
        // Parse the input into tokens
        parse(line, tokens, " ");
        
        if (tokens.empty()) {
            continue; // Skip empty input
        }
        
        // Check for "exit" command to terminate the shell immediately
        if (tokens[0] == "exit") {
            cout << "Exiting shell..." << endl;
            break;
        }

        // Execute user command tokens
        execute(tokens);
    }
}

int main()
{
    SimpleShell shell;
    shell.run(); // Start the shell loop
    return 0;
}
