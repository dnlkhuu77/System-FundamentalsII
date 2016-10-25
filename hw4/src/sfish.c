#include "sfish.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int return_code = 0;
char* prevDir;

int main(int argc, char** argv) {
    //DO NOT MODIFY THIS. If you do you will get a ZERO.
    rl_catch_signals = 0;
    //This is disable readline's default signal handlers, since you are going
    //to install your own.

    char *cmd;
    pid_t pid;
    int child_status;
    char* shellDir = calloc(1024, sizeof(char));
    //char* prompt = calloc(1024, sizeof(char));

    while((cmd = readline(strcat(getcwd(shellDir, 1024), "<sfish> "))) != NULL){
        char* userInput = strtok(cmd, " ");

        if (strcmp(userInput,"quit")==0)
            break;
        else if(strcmp(userInput, "help") == 0){
            pid = fork();
            if(pid == 0)
                print_help();
            else
                wait(&child_status);
        }
        else if(strcmp(userInput, "exit") == 0){
            exit(pid);
            break;
        }
        else if(strcmp(userInput, "cd") == 0){
            changeDir(strtok(NULL, " "));
        }
        else if(strcmp(userInput, "pwd") == 0){
            pid = fork();
            if(pid == 0){
                char* currentDir = getcwd(shellDir, 1024);
                printf("%s\n", currentDir);
            }
            else{
                wait(&child_status);
            }
        }
        else if(strcmp(userInput, "ptr") == 0){
            pid = fork();
            if(pid == 0){
                printf("%d\n", return_code);
            }
            else{
                wait(&child_status);
            }
        }
        else if(strcmp(userInput, "chpmt") == 0){
            printf("TIME\n");
        }
        else if(strcmp(userInput, "chclr") == 0){
            printf("STUDD\n");
        }

        //printf("%s\n",cmd);

        //All your debug print statments should be surrounded by this #ifdef
        //block. Use the debug target in the makefile to run with these enabled.
        #ifdef DEBUG
        fprintf(stderr, "Length of command entered: %ld\n", strlen(cmd));
        #endif
        //You WILL lose points if your shell prints out garbage values.

    }

    //Don't forget to free allocated memory, and close file descriptors.
    free(shellDir);
    //WE WILL CHECK VALGRIND!

    return EXIT_SUCCESS;
}

void print_help(){
    int j = 0;
    for(j = 0; j < 8; j++){
        printf("%s\n", USAGE[j]);
    }
}

void changeDir(char* d){
    //printf("%s %s\n", "changeDir: ", d);
    //printf("HOME: %s\n", getenv("HOME"));

    if(d == NULL){
        if(chdir(getenv("HOME")) == -1){
            printf("Invalid directory\n");
            return_code = 1;
        }
        else
            prevDir = ".";
    }else{
        if(strcmp(d, ".") == 0){
            prevDir = "shellDir";
            chdir(".");
        }
        else if(strcmp(d, "..") == 0){
            prevDir = "shellDir";
            chdir("..");
        }
        else if(strcmp(d, "-") == 0){
            //if(indicaotr == .) 
            chdir(prevDir);
        }
        else if(d != NULL){
            chdir(d);
        }
    }
}