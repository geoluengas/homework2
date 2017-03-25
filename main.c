/*
 * File:   HW2-CS531-Gxxx.c
 * Author: yluengas
 *
 * Created on March 21, 2017, 7:57 PM
 */

//TODO: add handling for SIGINT

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
void handle_signal(int sig);

/**
 * 
 * Function to parse the command line argument string into char pointer array
 * @param argv the char pointer array to store the arguments
 * @param s the command line argument string
 * @return the number of arguments in the argument string
 */
int argument_string_to_argv(char **argv, char *s);

/**
 * Get the hostname the program is running on
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

    int child_status; //child exit status
    pid_t child_pid; //child process ID

    if (!s) { //Check if s null. Return -1 if s==null
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
        execvp(*argv, argv); //execute the command. Does not return if successful
        //If execvp fails, print an error
        fprintf(stderr, "Error: Unsupported command or command failed to execute.\n");
    } else {//if parent process wait for child
        if (pid == (pid_t) (-1)) {
            fprintf(stderr, "Fork failed.\n");
        } else {
            child_pid = wait(&child_status); //wait for child proc to complete
            printf("Parent(%ld): Child(%ld) exited with status = %d\n", (long) getpid(), (long) child_pid, child_status);
        }
    }
    return 1;
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

void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("Received SIGINT\n");
    }
}