/*
*File name - writer.c (A replacement to the previously written writer.sh script)
*Author - Hanooshram Venkateswaran
*Attributions - Resources used -
*
*  1) https://www.ibm.com/docs/en/aix/7.2.0?topic=shell-input-output-redirection-in-c
*  2) https://www.geeksforgeeks.org/cpp/command-line-arguments-in-c-cpp/ 
*  I had also looked up on how to convert a specific shell script command sytax to C for linux
*
*/



#include <stdio.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Opens connection to system logger using LOG_USER facility
    openlog("writer", LOG_PID, LOG_USER);

    // Verifies argument count; equivalent to [ $# -lt 2 ] in the writer.sh script
    if (argc != 3) {
        syslog(LOG_ERR, "Invalid argument count: %d", argc - 1);
        fprintf(stderr, "Usage: %s <path> <string>\n", argv[0]);
        return 1;
    }

    char *target_file = argv[1];   // Matches writefile=$1 from writer.sh
    char *text_content = argv[2];  // Matches writestr=$2

    // log writing operation at LOG_DEBUG level
    syslog(LOG_DEBUG, "Writing %s to %s", text_content, target_file);

    // Open file for writing; equivalent to the '>' redirection from writer.sh
    FILE *my_file = fopen(target_file, "w");
    
    // Check if the file opened successfully or not; equivalent to [ $? -ne 0 ] 
    if (my_file == NULL) {
        syslog(LOG_ERR, "Error opening file: %s", strerror(errno));
        perror("Fopen failed");
        return 1;
    }

    // Writes the string to the file
    fprintf(my_file, "%s", text_content);
    
    // Closes file and system log
    fclose(my_file);
    closelog();
    
    return 0;
}
