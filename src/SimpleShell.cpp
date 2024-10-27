/**
 *    Zachary Marrs
 *    CS 461 P 001 - Operating Systems
 *    Professor Essa Imhmed
 *    10/27/2024
 *
 *    ps2 - Simple Shell
 */
#include "../include/SimpleShell.h"
using namespace std;

/**
 * @brief Creates a simple shell that accepts commands from standard input and executes them if possible.
 *        and if the command is valid.
 */

/**
* Once command tokens are received, this function spawns a child process which executes any valid commands.
*
* @param   argv   The vector<string>& of commands inputed by the user, to be executed
*/
void SimpleShell::execute(const vector<string>& argv)
{
    int status;
    pid_t child;
    
    // Spawning a child process
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
        
        // Prepare arguments for execvp (must end with nullptr)
        vector<const char*> args;
        for (const auto& arg : argv) {
            args.push_back(arg.c_str());
        }
        args.push_back(nullptr);
        
        // Execute *any valid command* using execvp
        if (execvp(argv[0].c_str(), const_cast<char* const*>(args.data())) == -1) {
            perror("execvp failed");
            _exit(1);
        }
    }
    else {
        perror("fork failed"); // Error handling if fork fails
        exit(1);
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
        
        // Handle "cd" command in parent loop
        if (tokens[0] == "cd") {
            if (tokens.size() > 1) {
                if (chdir(tokens[1].c_str()) != 0) {
                    perror("cd function failed");
                }
            } else {
                cerr << "cd: missing argument" << endl;
            }
            continue;
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
