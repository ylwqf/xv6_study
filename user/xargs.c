// Lab Xv6 and Unix utilities
// xargs.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_ARG_LENGTH 100
#define MAX_LINE_LENGTH 512

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "usage: xargs command [args...]\n");
        exit(1);
    }
    
    // Set up initial arguments (skip "xargs")
    char *cmd_argv[MAXARG];
    int cmd_argc = 0;
    
    // Copy arguments from command line (skip xargs itself)
    for (int i = 1; i < argc; i++) {
        cmd_argv[cmd_argc++] = argv[i];
    }
    
    char line[MAX_LINE_LENGTH];
    char *current = line;
    int n;
    char ch;
    
    // Read from stdin character by character
    while ((n = read(0, &ch, 1)) > 0) {
        if (ch == '\n') {
            // End of line - execute command
            *current = '\0'; // Null-terminate the argument
            
            if (line[0] != '\0') { // Only execute if we have a non-empty argument
                // Set up final argument list
                char *final_argv[MAXARG];
                int final_argc = 0;
                
                // Copy command arguments
                for (int i = 0; i < cmd_argc; i++) {
                    final_argv[final_argc++] = cmd_argv[i];
                }
                
                // Add the line as the last argument
                final_argv[final_argc++] = line;
                final_argv[final_argc] = 0; // NULL terminate
                
                // Fork and execute
                if (fork() == 0) {
                    exec(final_argv[0], final_argv);
                    fprintf(2, "exec failed for %s\n", final_argv[0]);
                    exit(1);
                }
                wait(0);
            }
            
            // Reset for next line
            current = line;
            *current = '\0';
        } else if (ch == ' ' || ch == '\t') {
            // Handle spaces if we wanted to support multiple arguments per line
            // For simplicity, we're treating each line as a single argument
            *current++ = ch;
        } else {
            // Regular character
            if (current - line < MAX_LINE_LENGTH - 1) {
                *current++ = ch;
            }
        }
    }
    
    // Handle the last line if it doesn't end with newline
    if (current != line) {
        *current = '\0';
        
        if (line[0] != '\0') {
            char *final_argv[MAXARG];
            int final_argc = 0;
            
            for (int i = 0; i < cmd_argc; i++) {
                final_argv[final_argc++] = cmd_argv[i];
            }
            
            final_argv[final_argc++] = line;
            final_argv[final_argc] = 0;
            
            if (fork() == 0) {
                exec(final_argv[0], final_argv);
                fprintf(2, "exec failed for %s\n", final_argv[0]);
                exit(1);
            }
            wait(0);
        }
    }
    
    exit(0);
}
