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

int cs531_system(char *s); /* run command line s*/
int argument_string_to_argv(char **argv, char *s);
char printHostname();

int main(int argc, char *argv[]) {
    //printHostname();
    int status;
    status = cs531_system(argv[1]);
    return status;
}

int cs531_system(char *s) {

    if (!s) { //Check is s null. Return -1 if s==null
        printf("Error: Command is %s.\n", s);
        return -1;
    }
    if (strlen(s) >= MAX_ARGS_LEN) { //check if s meets length limits
        printf("Error: Argument string length exceeded (Max length = %d).\n", MAX_ARGS_LEN);
        return -1;
    }

    const char *argv[MAX_ARGS_LEN]; //array to store command line args

    int argc = argument_string_to_argv(argv, s); //Parse command line argument string

    execvp(*argv, argv); //execute the command. Does not return if successful

    //If execvp fails, print an error
    printf("Error: Unsupported command or command failed to execute.\n");

    return 1;
}

/**
 * Function to parse the command line argument string
 * @param argv the char pointer array to store the arguments
 * @param s the command line argument string
 * @return the number of arguments in the argument string
 */
int argument_string_to_argv(char **argv, char *s) {
    int argc = 0;
    printf("Command : (");
    while (*s != NULL) { //move pointer through string
        while (*s == SPACE || *s == TAB || *s == NEW_LINE) { //replace spaces, tabs, and newlines with null
            printf("%c", *s);
            *s++ = NULL;
        }
        argc++;
        *argv++ = s; //set the current argument to current position of s
        while (*s != NULL && *s != SPACE && *s != TAB && *s != NEW_LINE) { //Eat all non-blank/null chars
            printf("%c", *s);
            s++;
        }
    }
    printf(")\n");
    argc++;
    argv = NULL;
    printf("\nargc = %d\n", argc);
    
    for (int i = 0; i < argc + 1; i++) {
        printf("(%s)", &argv[i]);
    }
    
    printf("\n");
    return argc;
}

/**
 * Get the hostname the program is running on
 * @return the hostname
 */
char printHostname() {
    char hostname[1024];
    gethostname(hostname, 1024);
    printf("\nhostname : %s\n", hostname);
    return hostname;
}