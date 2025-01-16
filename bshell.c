#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Macros for fprintf
#define TOK_DELIM "\t\r\n"
#define MAGENTA "\033[1;35m"
#define RESET "\e[0m"
// Macros for buffer
#define LINE_BUFFER_SIZE 1024
#define TOKEN_BUFFER_SIZE 256

// Function declarations
int dash_execute(char **);
int dash_exit();
char *read_line();
char **split_line(char *);

/*
* Function to execute the line commands
* Tokens as paramters, outputs a flag identifier
*/
int dash_execute(char **args)
{
    pid_t cpid;
    int status;

    if (strcmp(args[0], "exit") == 0)
    {
        return dash_exit();
    }
    // Create a new process by duplicating process
    cpid = fork();
    // Sucess, in child process
    if (cpid == 0) 
    {
        // execvp to execute command, check for error (-1)
        if(execvp(args[0], args) < 0)
        {
            fprintf(stderr, "%sbshell: Command not found!%s\n", MAGENTA, RESET);
            exit(EXIT_FAILURE);
        }
    }
    // Error in forking
    else if (cpid < 0)
    {
        fprintf(stderr, "%sbshell: Error Forking!%s\n", MAGENTA, RESET);
        exit(EXIT_FAILURE);
    }
    // Wait for ending of child process, in case of parent
    else
    {
        waitpid(cpid, &status, WUNTRACED);
    }
    // Return 1 when cpid higher than 0
    return 1;
}
/*
* Exit bshell with a return 0
* Tokens from line parameters, returns 0.
*/
int dash_exit()
{
    return 0;
}

/*
* Used to read a line from the command line.
* Takes in no parameters, returns the buffer pointer to the whole line.
*/
char *read_line() 
{
    // Initialize a buffer with a size of 1024 bytes
    int buffsize = LINE_BUFFER_SIZE;
    char *buffer = malloc(sizeof(char) * buffsize);
    // Store int value of character in ASCII
    int c;
    // Position starts at 0
    int position = 0;
    // Check null buffer to warn and terminate accordingly
    if(!buffer)
    {
        fprintf(stderr, "%sbshell: Memory allocation error!%s\n", MAGENTA, RESET);
        exit(EXIT_FAILURE);
    }
    // Infinite loop, only new-line and End Of File make return
    while (1)
    {
        // Read one character from standard input
        c = getchar();
        // Certain characters indicate end of line
        if (c == EOF || c == '\n')
        {
            // New-line for clarity after reading a line
            printf("\n");
            // To return properly, terminate string in buffer
            buffer[position] = '\0';
            return buffer;
        }
        else 
        {
            // If is valid char, place in current position and increment position by 1
            buffer[position++] = c;
        }
        // End of buffer, realloc another 1024 bytes to keep reading
        if (position >= buffsize) 
        {
            buffsize += LINE_BUFFER_SIZE;
            buffer = realloc(buffer, buffsize);
            // Handle failure
            if (!buffer) 
            {
                fprintf(stderr, "%sbshell: Memory reallocation error!%s\n", MAGENTA, RESET);
                exit(EXIT_FAILURE);
            }
        }
    }
}

/*
* Tokenize and split each line for handling.
* Takes in the line, spits out the tokens.
*/
char **split_line(char *line)
{
    // Initialize variables and memory
    int buffsize = TOKEN_BUFFER_SIZE, position = 0;
    char **tokens = malloc(buffsize * sizeof(char *));
    char *token;
    // Handle alloc failures
    if (!tokens)
    {
        fprintf(stderr, "%sbshell: Memory allocation error!%s\n", MAGENTA, RESET);
        exit(EXIT_FAILURE);
    }
    // First call serves to feed line, with certain delimitors
    token = strtok(line, TOK_DELIM);
    while (token != NULL)
    {
        // Place read token and update position 
        tokens[position++] = token;
        // Reallocate when above buffer size
        if (position >= buffsize)
        {
            buffsize += TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, buffsize * sizeof(char *));
            // Handle memory error properly
            if (!tokens)
            {
                fprintf(stderr, "%sbshell: Memory reallocation error!%s\n", MAGENTA, RESET);
                exit(EXIT_FAILURE);
            }
        }
        // Read next token from line input, with the correct delims
        token = strtok(NULL, TOK_DELIM);
    }

    // Properly terminate the matrix will null pointer and return it
    tokens[position] = NULL;
    return tokens;
}

/*
* */
/*
* Main function of the shell loop.
* No parameters, no return type.
*/
void loop() 
{
    // pointer will store command line input
    char *line;
    // array of arguments parsed from command line 
    char **args;
    // status flag for loop
    int status = 1;

    do 
    {
        // Always print "> " to indicate a line to read 
        printf("> ");
        // Read command line input and store in line
        line = read_line();
        // Parse into arguments
        args = split_line(line);
        // Execute command and update status
        status = dash_execute(args);
        // Always free C memory, avoid leaks
        free(line);
        free(args);
    } 
    while(status); // Repeat while status is not zero
}

// Main function just calls loop and returns 0
int main() 
{
    loop();
    return 0;
}