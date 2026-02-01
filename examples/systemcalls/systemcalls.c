//Author - Hanooshram venkateswaran
//File name - systemcalls.c
//Attributions - referenced the link: https://stackoverflow.com/a/13784315/1446624 for the do_exec_redirect section of the code
//Note - Added include header files to properly build using unit-test.sh script




#include "systemcalls.h"

#include <stdlib.h>    // For system(), exit(), EXIT_FAILURE
#include <unistd.h>    // For fork(), execv(), close(), dup2()
#include <sys/wait.h>  // For waitpid(), WIFEXITED, WEXITSTATUS
#include <sys/types.h> // For pid_t type
#include <fcntl.h>     // For open() and O_ flags
#include <sys/stat.h>  // For file permissions

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

int status = system(cmd);
//This calls the system function to exe the command string, return value has the termination status of the  process

if (status == -1) { // return value of -1 indicates system failure
        return false;
    }

//status 0 indicates success, and WIFEXITED checks if the child process terminated normally
if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return true;
    }


// returns 'false' if the command failed
    return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
// flushes standard out before fork to avoid duplicate prints
fflush(stdout);

pid_t pid = fork();
if (pid == -1) {
//The fork failed
    va_end(args);
    return false;
} else if (pid == 0) {
// child process executes the command
    execv(command[0], command);
// execv only returns if an error has occurred
    exit(EXIT_FAILURE);
}

int status;
// parent waits for the child process to terminate
if (waitpid(pid, &status, 0) == -1) {
    va_end(args);  
    return false;
}
va_end(args);

// returns true if the child terminated with exit code 0
return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/


// Opens, creates and truncatesthe output file based on the condition
    int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        // Failed to open the file
        va_end(args);
        return false;
    }

    // Flushes standard out before fork to avoid duplicate prints
    fflush(stdout);

    pid_t pid = fork();
    if (pid == -1) {
        // Fork failed
        close(fd);
        va_end(args);
        return false;
    } else if (pid == 0) {
        // Child process logic
        // Redirects standard out to the file descriptor (fd)
        if (dup2(fd, 1) < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        // Closes the file descriptor
        close(fd);

        execv(command[0], command);
        // execv only returns if an error has occurred
        exit(EXIT_FAILURE);
    }

    // Parent process logic
    close(fd); // Parent does not need the file descriptor
    int status;
    // Parent waits for the child process to terminate
    if (waitpid(pid, &status, 0) == -1) {
        va_end(args);
        return false;
    }

    va_end(args);
    // Returns true if the child terminated normally with exit code 0
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}
