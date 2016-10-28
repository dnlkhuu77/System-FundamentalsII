#include "sfish.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define RED "\e[0;31m"
#define RED_B "\e[1;31m"
#define BLUE "\e[0;34m"
#define BLUE_B "\e[1;34m"
#define GREEN "\e[0;32m"
#define GREEN_B "\e[1;32m"
#define YELLOW "\e[0;33m"
#define YELLOW_B "\e[1;33m"
#define CYAN "\e[0;36m"
#define CYAN_B "\e[1;36m"
#define MAGENTA "\e[0;35m"
#define MAGENTA_B "\e[1;35m"
#define BLACK "\e[0;30m"
#define BLACK_B "\e[1;30m"
#define WHITE "\e[0;37m"
#define WHITE_B "\e[1;37m"
#define RESET "\e[0m"

int u_toggle = 1;
int m_toggle = 1;
int cursor_count = 0;
int built_flag = -1;
int u_color_toggle = -1;
int u_bold_toggle = -1;
int m_color_toggle = -1;
int m_bold_toggle = -1;
int child_status = 0;
char* prevDir = NULL;
char* shellDir;
char* shellName;
char* getting_arg;
char * userInput[100];
int arg_count = 0;

int main(int argc, char** argv) {
    //DO NOT MODIFY THIS. If you do you will get a ZERO.
    rl_catch_signals = 0;
    //This is disable readline's default signal handlers, since you are going
    //to install your own.

    char* cmd;
    pid_t pid;
    shellName = cmd_display(u_toggle, m_toggle, u_color_toggle, u_bold_toggle, m_color_toggle, m_bold_toggle);

    while((cmd = readline(shellName)) != NULL){
        arg_count = 0;

        getting_arg = strtok(cmd, " ");
        userInput[0] = getting_arg;
        for(int i = 1; i < 100; i++){
            getting_arg = strtok(NULL, " ");
            if(getting_arg != NULL){
                userInput[i] = getting_arg;
            }
            else
                break;
        }
        for(int i = 0; i < 100; i++){
            if(userInput[i] != NULL)
                arg_count++;
        }

        built_flag = -1;

        if(userInput[0] == NULL){
            ;
        }
        else if(strcmp(userInput[0], "help") == 0){
            built_flag = 0;
            pid = fork();
            if(pid == 0){
                print_help();
                exit(pid);
            }
            else
                waitpid(pid, &child_status, 0);
        }
        else if(strcmp(userInput[0], "exit") == 0){
            built_flag = 0;
            exit(pid);
            break;
        }
        else if(strcmp(userInput[0], "cd") == 0){
            changeDir(userInput[1]);
            built_flag = 0;
        }
        else if(strcmp(userInput[0], "pwd") == 0){
            built_flag = 0;
            pid = fork();
            if(pid == 0){
                char* currentDir = getcwd(shellDir, 1024);
                printf("%s\n", currentDir);
                exit(pid);
            }
            else{
                waitpid(pid, &child_status, 0);
            }
        }
        else if(strcmp(userInput[0], "prt") == 0){
            built_flag = 0;
            pid = fork();
            if(pid == 0){
                int s = WEXITSTATUS(child_status);
                printf("%d\n", s);

                exit(pid);
            }
            else{
                waitpid(pid, &child_status, 0);
            }
        }
        else if(strcmp(userInput[0], "chpmt") == 0){
            built_flag = 0;
            char* settings = userInput[1];
            char* togg = userInput[2];

            if(settings == NULL || togg == NULL)
                ;
            else if(strcmp(settings, "user") == 0){
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
        else if(strcmp(userInput[0], "chclr") == 0){
            built_flag = 0;
            char* settings = userInput[1];
            char* color = userInput[2];
            char* bold = userInput[3];

            if(settings == NULL || color == NULL || bold == NULL)
                ;
            else if(strcmp(settings, "user") == 0){
                if(strcmp(color, "red") == 0){
                    if(strcmp(bold, "1") == 0){
                        u_color_toggle = 1;
                        u_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        u_color_toggle = 1;
                        u_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "blue") == 0){
                    if(strcmp(bold, "1") == 0){
                        u_color_toggle = 2;
                        u_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        u_color_toggle = 2;
                        u_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "green") == 0){
                    if(strcmp(bold, "1") == 0){
                        u_color_toggle = 3;
                        u_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        u_color_toggle = 3;
                        u_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "yellow") == 0){
                    if(strcmp(bold, "1") == 0){
                        u_color_toggle = 4;
                        u_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        u_color_toggle = 4;
                        u_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "cyan") == 0){
                    if(strcmp(bold, "1") == 0){
                        u_color_toggle = 5;
                        u_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        u_color_toggle = 5;
                        u_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "magneta") == 0){
                    if(strcmp(bold, "1") == 0){
                        u_color_toggle = 6;
                        u_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        u_color_toggle = 6;
                        u_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "black") == 0){
                    if(strcmp(bold, "1") == 0){
                        u_color_toggle = 7;
                        u_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        u_color_toggle = 7;
                        u_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "white") == 0){
                    if(strcmp(bold, "1") == 0){
                        u_color_toggle = 8;
                        u_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        u_color_toggle = 8;
                        u_bold_toggle = 0;
                    }
                }
            }
            else if(strcmp(settings, "machine") == 0){
                if(strcmp(color, "red") == 0){
                    if(strcmp(bold, "1") == 0){
                        m_color_toggle = 1;
                        m_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        m_color_toggle = 1;
                        m_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "blue") == 0){
                    if(strcmp(bold, "1") == 0){
                        m_color_toggle = 2;
                        m_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        m_color_toggle = 2;
                        m_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "green") == 0){
                    if(strcmp(bold, "1") == 0){
                        m_color_toggle = 3;
                        m_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        m_color_toggle = 3;
                        m_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "yellow") == 0){
                    if(strcmp(bold, "1") == 0){
                        m_color_toggle = 4;
                        m_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        m_color_toggle = 4;
                        m_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "cyan") == 0){
                    if(strcmp(bold, "1") == 0){
                        m_color_toggle = 5;
                        m_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        m_color_toggle = 5;
                        m_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "magneta") == 0){
                    if(strcmp(bold, "1") == 0){
                        m_color_toggle = 6;
                        m_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        m_color_toggle = 6;
                        m_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "black") == 0){
                    if(strcmp(bold, "1") == 0){
                        m_color_toggle = 7;
                        m_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        m_color_toggle = 7;
                        m_bold_toggle = 0;
                    }
                }
                else if(strcmp(color, "white") == 0){
                    if(strcmp(bold, "1") == 0){
                        m_color_toggle = 8;
                        m_bold_toggle = 1;
                    }else if(strcmp(bold, "0") == 0){
                        m_color_toggle = 8;
                        m_bold_toggle = 0;
                    }
                }
            }
        }

        //EXECUTABLES
        if(built_flag != 0){
            pid = fork();
            if(pid == 0){
                char* custom_arg[arg_count + 1];
                memset(custom_arg, 0, arg_count);
                for(int i = 0; i < arg_count; i++){
                    custom_arg[i] = userInput[i];
                }
                custom_arg[arg_count] = NULL;

                char* f_test = getenv("PATH");
                char* att = strtok(f_test, ":"); //att is a path (or f_test)
                char* y[1024] = {0};
                y[0] = att;

                for(int i =1; i < 1025; i++){ //getting the paths from the PATH string into an array
                    y[i] = strtok(NULL, ":");

                    if(y[i] == NULL)
                        break;
                }

                char* g = malloc(1024);

                if(doesFileExist(userInput[0]) == 0){ //if it already has the /
                        execv(userInput[0], custom_arg);
                        break;
                }

                for(int i = 0; i < 1024; i++){
                    if(y[i] == NULL)
                        break;

                    strcpy(g, y[i]);
                    strcat(g, "/");
                    strcat(g, userInput[0]);

                    if(doesFileExist(g) == 0){
                        execv(g, custom_arg);
                        break;
                    }
                    memset(g, 0, sizeof(char));
                }
                free(g);
                exit(pid);
            }
            else
                waitpid(pid, &child_status, 0);

        }


        //All your debug print statments should be surrounded by this #ifdef
        //block. Use the debug target in the makefile to run with these enabled.
        #ifdef DEBUG
        fprintf(stderr, "Length of command entered: %ld\n", strlen(cmd));
        #endif
        //You WILL lose points if your shell prints out garbage values.

        for(int i = 0; i < 100; i++){
            userInput[i] = NULL;
        }
        shellName = cmd_display(u_toggle, m_toggle, u_color_toggle, u_bold_toggle, m_color_toggle, m_bold_toggle);
    }

    //Don't forget to free allocated memory, and close file descriptors.
    free(shellDir);
    //WE WILL CHECK VALGRIND!

    return EXIT_SUCCESS;
}

void print_help(){
    int j = 0;
    for(j = 0; j < 9; j++){
        printf("%s\n", USAGE[j]);
    }
}

int doesFileExist(char* s){
    struct stat buff;
    int x = stat(s, &buff);
    return x;

}

char* cmd_display(int u_togg, int m_togg, int uc_togg, int ub_togg, int mc_togg, int mb_togg){
    char* hostname = (char*) malloc(1024); //hostmachine is machine
    gethostname(hostname, 1023);

    char* hello = getenv("USER"); //hello is user
    char* home = getenv("HOME");
    char* currentDir = getcwd(shellDir, 1024);

    char* ans = (char*) malloc(1024);
    
    strcat(ans,"sfish");
    cursor_count = cursor_count + 5;

    if(u_togg == 1){
        strcat(ans, "-");
        cursor_count++;

        if(uc_togg == 1){ //red
            if(ub_togg == 0)
                strcat(ans, RED);
            else if(ub_togg == 1)
                strcat(ans, RED_B);
        }
        else if(uc_togg == 2){ //blue
            if(ub_togg == 0)
                strcat(ans, BLUE);
            else if(ub_togg == 1)
                strcat(ans, BLUE_B);
        }
        else if(uc_togg == 3){ //green
            if(ub_togg == 0)
                strcat(ans, GREEN);
            else if(ub_togg == 1)
                strcat(ans, GREEN_B);
        }
        else if(uc_togg == 4){ //yellow
            if(ub_togg == 0)
                strcat(ans, YELLOW);
            else if(ub_togg == 1)
                strcat(ans, YELLOW_B);
        }
        else if(uc_togg == 5){ //cyan
            if(ub_togg == 0)
                strcat(ans, CYAN);
            else if(ub_togg == 1)
                strcat(ans, CYAN_B);
        }
        else if(uc_togg == 6){ //magneta
            if(ub_togg == 0)
                strcat(ans, MAGENTA);
            else if(ub_togg == 1)
                strcat(ans, MAGENTA_B);
        }
        else if(uc_togg == 7){ //black
            if(ub_togg == 0)
                strcat(ans, BLACK);
            else if(ub_togg == 1)
                strcat(ans, BLACK_B);
        }
        else if(uc_togg == 8){ //white
            if(ub_togg == 0)
                strcat(ans, WHITE);
            else if(ub_togg == 1)
                strcat(ans, WHITE_B);
        }

        strcat(ans, hello);
        cursor_count = cursor_count + strlen(hello);

        if(uc_togg != -1 && ub_togg != -1)
        strcat(ans, "\e[0m");
    }

    if(u_togg == 1 && m_togg == 1){
        strcat(ans, "@");
        cursor_count++;
    }

    if(m_togg == 1){
        if(u_togg == 0)
            strcat(ans, "-");
        if(mc_togg == 1){ //red
            if(mb_togg == 0)
                strcat(ans, RED);
            else if(mb_togg == 1)
                strcat(ans, RED_B);
        }
        else if(mc_togg == 2){ //blue
            if(mb_togg == 0)
                strcat(ans, BLUE);
            else if(mb_togg == 1)
                strcat(ans, BLUE_B);
        }
        else if(mc_togg == 3){ //green
            if(mb_togg == 0)
                strcat(ans, GREEN);
            else if(mb_togg == 1)
                strcat(ans, GREEN_B);
        }
        else if(mc_togg == 4){ //yellow
            if(mb_togg == 0)
                strcat(ans, YELLOW);
            else if(mb_togg == 1)
                strcat(ans, YELLOW_B);
        }
        else if(mc_togg == 5){ //cyan
            if(mb_togg == 0)
                strcat(ans, CYAN);
            else if(mb_togg == 1)
                strcat(ans, CYAN_B);
        }
        else if(mc_togg == 6){ //magneta
            if(mb_togg == 0)
                strcat(ans, MAGENTA);
            else if(mb_togg == 1)
                strcat(ans, MAGENTA_B);
        }
        else if(mc_togg == 7){ //black
            if(mb_togg == 0)
                strcat(ans, BLACK);
            else if(mb_togg == 1)
                strcat(ans, BLACK_B);
        }
        else if(mc_togg == 8){ //white
            if(mb_togg == 0)
                strcat(ans, WHITE);
            else if(mb_togg == 1)
                strcat(ans, WHITE_B);
        }
        strcat(ans, hostname);
        cursor_count = cursor_count + strlen(hostname);

        if(mc_togg != -1 && mb_togg != -1)
            strcat(ans, "\e[0m");
    }

    strcat(ans, ":[");
    if(strcmp(currentDir, home) != 0)
        strcat(ans, currentDir);
    else
        strcat(ans, "~");
    strcat(ans, "]> ");
    cursor_count = cursor_count + strlen(currentDir);
    cursor_count = cursor_count + 6;

    free(hostname);
    free(ans);
    
    return ans;
}

void changeDir(char* d){
    if(d == NULL){
        if(chdir(getenv("HOME")) == -1){
            printf("Invalid directory\n");
        }
        else
            prevDir = getcwd(shellDir, 1024);
    }else{
        if(strcmp(d, ".") == 0){
            prevDir = getcwd(shellDir, 1024);
            chdir(".");
        }
        else if(strcmp(d, "..") == 0){
            prevDir = getcwd(shellDir, 1024);
            chdir("..");
        }
        else if(strcmp(d, "-") == 0){
            if(prevDir != NULL)
                chdir(prevDir);
            else
                printf(": OLDPWD not set\n");
            prevDir = NULL;
        }
        else if(d != NULL){
            prevDir = getcwd(shellDir, 1024);
            chdir(d);
        }
    }
}