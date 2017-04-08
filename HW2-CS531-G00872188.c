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

    int original_handler; //stores the original signal handler before being changed by the program
    int temp_handler; //stores the temporary signal handler set by the program

    int child_status; //child exit status
    pid_t child_pid; //child process ID

    if (!comm) { //Check if argument string is null. Return -1 if comm==null
        fprintf(stderr, "Error: Command is %s.\n", comm);
        return -1;
    }

    //Ignore SIGINT while child is running
    if ((original_handler = signal(SIGINT, handle_signal)) == SIG_ERR) {
        fprintf(stderr, "signal not caught.");
    }

    pid_t pid = fork(); //Fork child process to run execvp()

    if (pid == NULL) {//if child process, run execvp

        int tt = open("/dev/tty", O_RDWR); //open file descriptor for terminal
        close(0); //close stdin
        dup(tt); //duplicate terminal to stdin
        close(1); //close stdout
        dup(tt); //duplicate terminal to stdout
        close(2); //close stdterr
        dup(tt); //duplicate terminal to stderr
        close(tt); //close the tty

        //execute the command. Does not return if successful
        execlp("/bin/sh", "/bin/sh", "-c", comm, NULL);

        //If execlp fails, print an error
        fprintf(stderr, "Error: Unsupported command or command failed to execute.\n");
        return -1;

    } else {//if parent process
        if (pid == (pid_t) (-1)) { //error out if fork() failed
            fprintf(stderr, "Error: Fork failed.\n");
            return -1;
        } else {
            //wait for child proc to complete
            child_pid = wait(&child_status);

            //Restore SIGINT handler to previous state
            temp_handler = signal(SIGINT, original_handler);

            printf("DEBUG{ Parent(%ld): Child(%ld) exited with status = %d }\n", (long) getpid(), (long) child_pid, child_status);
        }
    }
    return child_status; // return exit status of child process
}

void handle_signal(int signal) {
    switch (signal) {
        case SIGINT:
            fprintf(stderr, "DEBUG{ Caught SIGINT for PID :%d }\n", getpid());
            break;
        default:
            fprintf(stderr, "Signal not handled : %d", signal);
            break;
    }
}