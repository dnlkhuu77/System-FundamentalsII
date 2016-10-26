#include "sfish.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BLACK "\x1b[30m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"
#define RESET "\x1b[0m"

int u_toggle, m_toggle = 1; //show both machine
int child_status;
char* userColor = NULL;
char* machineColor = NULL;
char* prevDir;
char* shellDir;

int main(int argc, char** argv) {
    //DO NOT MODIFY THIS. If you do you will get a ZERO.
    rl_catch_signals = 0;
    //This is disable readline's default signal handlers, since you are going
    //to install your own.

    char* cmd;
    pid_t pid;
    u_toggle = 1;
    m_toggle = 1;

    while((cmd = readline(cmd_display(u_toggle, m_toggle))) != NULL){
        char* userInput = strtok(cmd, " ");

        if(strcmp(userInput, "help") == 0){
            pid = fork();
            if(pid == 0){
                print_help();
                exit(pid);
            }
            else
                waitpid(pid, &child_status, 0);
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
                exit(3);
            }
            else{
                waitpid(pid, &child_status, 0);
            }
        }
        else if(strcmp(userInput, "ptr") == 0){
            pid = fork();
            if(pid == 0){
                printf("%d\n", child_status);
                exit(3);
            }
            else{
                waitpid(pid, &child_status, 0);
            }
        }
        else if(strcmp(userInput, "chpmt") == 0){
            char* settings = strtok(NULL, " ");
            char* togg = strtok(NULL, " ");

            if(strcmp(settings, "user") == 0){
                if(strcmp(togg, "1") == 0)
                    u_toggle = 1;
                else if(strcmp(togg, "0") == 0)
                    u_toggle = 0;
            }else if(strcmp(settings, "machine") == 0){
                if(strcmp(togg, "1") == 0)
                    m_toggle = 1;
                else if(strcmp(togg, "0") == 0)
                    m_toggle = 0;
            }

        }
        else if(strcmp(userInput, "chclr") == 0){
            printf("STUDD\n");
        }

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

char* cmd_display(int u_togg, int m_togg){
    char* hostname = (char*) malloc(1024); //hostmachine is machine
    gethostname(hostname, 1023);
    //char* hello = (char*) malloc(1024);

    char* hello = getenv("USER"); //hello is user
    char* currentDir = getcwd(shellDir, 1024);

    char* ans = (char*) malloc(1024);
    
    strcpy(ans,"sfish");

    if(u_togg == 1){
        strcat(ans, "-");
        strcat(ans, hello);
    }

    if(u_togg == 1 && m_togg == 1)
        strcat(ans, "@");

    if(m_togg == 1){
        strcat(ans, "-");
        strcat(ans, hostname);
    }

    strcat(ans, ":[");
    if(strcmp(currentDir, "/home") != 0)
        strcat(ans, currentDir);
    else
        strcat(ans, "~");
    strcat(ans, "]> ");

    free(hostname);
    free(ans);
    
    return ans;
}

void changeDir(char* d){
    //printf("%s %s\n", "changeDir: ", d);
    //printf("HOME: %s\n", getenv("HOME"));

    if(d == NULL){
        if(chdir(getenv("HOME")) == -1){
            printf("Invalid directory\n");
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
            chdir("-");
        }
        else if(d != NULL){
            chdir(d);
        }
    }
}