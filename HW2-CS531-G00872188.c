/*
 * File:   HW2-CS531-G00872188.c
 *
 * Created on March 21, 2017, 7:57 PM
 */


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#define MAX_ARGS_LEN 1024  /*Maximum length of argument string*/
#define SPACE 32
#define TAB 9
#define NEW_LINE 10

#ifndef DEBUG
#define DEBUG 0 /*1 to set DEBUG mode. 0 to unset DEBUG*/
#endif

/**
 * 
 * @param s
 * @return 
 */
int cs531_system(char *s);

/**
 * Function to handle SIG
 * @param sig
 */
void handle_signal(int signal);

/**
 * 
 * Function to parse the command line argument string into char pointer array
 * @param argv the char pointer array to store the arguments
 * @param s the command line argument string
 * @return the number of arguments in the argument string
 */
int argument_string_to_argv(char **argv, char *s);

/**
 * Debug function to get the hostname the program is running on
 * @return the hostname
 */
char printHostname();

int main(int argc, char *argv[]) {
    int status;
    status = cs531_system(argv[1]);
    return status;
}

int cs531_system(char *s) {
    if (DEBUG) printHostname();

    int original_handler; //stores the original signal handler before being changed by the program
    int temp_handler; //stores the temporary signal handler set by the program

    int child_status; //child exit status
    pid_t child_pid; //child process ID

    if (!s) { //Check if argument string is null. Return -1 if s==null
        fprintf(stderr, "Error: Command is %s.\n", s);
        return -1;
    }
    if (strlen(s) >= MAX_ARGS_LEN) { //check if s meets length limits
        fprintf(stderr, "Error: Argument string length exceeded (Max length = %d).\n", MAX_ARGS_LEN);
        return -1;
    }

    const char *argv[MAX_ARGS_LEN]; //array to store command line args parsed from s

    int argc = argument_string_to_argv(argv, s); //Parse command line argument string

    pid_t pid = fork(); //Fork child process to run execvp()

    if (pid == NULL) {//if child process, run execvp
        
        int tt = open("/dev/tty", O_RDWR);//open file descriptor for terminal
        close(0);//close stdin
        dup(tt);//duplicate terminal to stdin
        close(1);//close stdout
        dup(tt);//duplicate terminal to stdout
        close(2);//close stdterr
        dup(tt);//duplicate terminal to stderr
        close(tt);//close the tty
        
        //using execvp() so to be more flexible with argument count
        execvp(*argv, argv); //execute the command. Does not return if successful
        //If execvp fails, print an error
        
        fprintf(stderr, "Error: Unsupported command or command failed to execute.\n");
        return -1;

    } else {//if parent process
        if (pid == (pid_t) (-1)) { //error out if fork() failed
            fprintf(stderr, "Fork failed.\n");
            return -1;
        } else {
            //Make parent ignore SIGINT while child is still running
            if ((original_handler = signal(SIGINT, handle_signal)) == SIG_ERR) {
                fprintf(stderr, "signal not caught.");
            }
            if (DEBUG) printf("original SIGNINT handler = %d\n", original_handler);

            child_pid = wait(&child_status); //wait for child proc to complete

            //Restore SIGINT handler to previous state
            temp_handler = signal(SIGINT, original_handler);
            if (DEBUG) printf("temp SIGINT handler = %d\n", temp_handler);

            printf("Parent(%ld): Child(%ld) exited with status = %d\n", (long) getpid(), (long) child_pid, child_status);
        }
    }
    return 1; // return success
}

int argument_string_to_argv(char **argv, char *s) {
    int argc = 0;
    if (DEBUG) printf("Command : (");
    while (*s != NULL) { //move pointer through string
        while (*s == SPACE || *s == TAB || *s == NEW_LINE) { //replace spaces, tabs, and newlines with null
            if (DEBUG) printf("%c", *s);
            *s++ = NULL;
        }
        argc++;
        *argv++ = s; //set the current argument to current position of s
        while (*s != NULL && *s != SPACE && *s != TAB && *s != NEW_LINE) { //Eat all non-blank/null chars
            if (DEBUG) printf("%c", *s);
            s++;
        }
    }
    if (DEBUG) printf(")\n");
    argc++;
    argv = NULL;
    if (DEBUG) printf("argc = %d\n", argc);
    return argc;
}

char printHostname() {
    char hostname[1024];
    gethostname(hostname, 1024);
    printf("hostname : %s\n", hostname);
    return hostname;
}

void handle_signal(int signal) {
    switch (signal) {
        case SIGINT:
            fprintf(stderr, "Caught SIGINT for PID :%d\n", getpid());
            break;
        default:
            fprintf(stderr, "Signal not handled : %d", signal);
            break;
    }
}