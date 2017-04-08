/*
 * File:   HW2-CS531-G00872188.c
 *
 * Yobani Luengas G00872188
 * Erik Arvidson G00379567
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
#include <errno.h>

#ifndef DEBUG
#define DEBUG 0
#endif

typedef void (*sighandler_t)(int);

/**
 * 
 * @param s
 * @return 
 */
int cs531_system(const char *comm);

/**
 * Function to handle SIG
 * @param sig
 */
void handle_signal(int signal);

int main(int argc, char *argv[]) {
    int status;
    status = cs531_system(argv[1]);
    return status;
}

int cs531_system(const char *comm) {

    if (strlen(&comm) == 0) {
        fprintf(stderr, "Error: no command provided. Exiting.\n");
        return -1;
    }

    sighandler_t int_handler; //stores the original SIGINT signal handler before being changed by the program
    sighandler_t quit_handler; //stores the original SIGQUIT signal handler before being changed by the program

    int child_status; //child exit status
    pid_t child_pid; //child process ID

    //Ignore SIGINT while child is running
    if ((int_handler = signal(SIGINT, handle_signal)) == SIG_ERR) {
        perror("signal");
        return -1;
    }

    //Ignore SIGQUIT while child is running
    if ((quit_handler = signal(SIGQUIT, handle_signal)) == SIG_ERR) {
        perror("signal");
        return -1;
    }

    pid_t pid = fork(); //Fork child process to run execvp()

    if (pid == (pid_t) 0) {//if child process, run execvp
        int tt = open("/dev/tty", O_RDWR); //open file descriptor for terminal
        close(STDIN_FILENO); //close stdin
        dup(tt); //duplicate terminal to stdin
        close(STDOUT_FILENO); //close stdout
        dup(tt); //duplicate terminal to stdout
        close(STDERR_FILENO); //close stderr
        dup(tt); //duplicate terminal to stderr
        close(tt); //close the tty

        //execute the command. Does not return if successful
        execlp("/bin/sh", "/bin/sh", "-c", comm, NULL);

        //If execlp fails, print an error
        perror("execlp");
        return -1;

    } else {//if parent process
        if (pid == (pid_t) - 1) { //error out if fork() failed
            perror("fork");
            return -1;
        } else {
            //wait for child proc to complete
            child_pid = wait(&child_status);

            //Restore SIGINT handler to previous state
            signal(SIGINT, int_handler);
            signal(SIGQUIT, quit_handler);

#if DEBUG
            printf("Parent(%ld): Child(%ld) exited with status = %d\n", (long) getpid(), (long) child_pid, child_status);
#endif
        }
    }
    return child_status; // return exit status of child process
}

void handle_signal(int signal) {
    switch (signal) {
        case SIGINT:
#if DEBUG
            fprintf(stderr, "Caught SIGINT for PID :%d\n", getpid());
#endif
            break;
        case SIGQUIT:
#if DEBUG
            fprintf(stderr, "Caught SIGQUIT for PID :%d\n", getpid());
#endif
            break;
        default:
            fprintf(stderr, "Signal not handled : %d", signal);
            break;
    }
}
