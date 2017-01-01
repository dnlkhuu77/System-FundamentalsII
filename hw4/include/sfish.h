#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

 typedef struct Assign{
 	char** args;
 	struct Assign *next;
 } Assign;

typedef struct job{
  	char *name;
  	pid_t pid;
  	int jid; 
  	int number;
  	int status;
  	struct job *next; 
} job;

/** The usage statement.*/
const char* USAGE[9] = { 
	"\nSFISH Bash, v.1",
	"These shell commands are defined internally. Type `help' to see this list.\n",
	"help [-dms] [pattern ...]",
	"exit [n]",
	"cd [-L|[-P [-e]] [-@]] [dir]",
	"pwd [-LP]",
	"prt",
	"chpmt",
	"chclr"
};

const char* SFISH[6] = {
	"\n----Info----",
	"help",
	"prt",
	"----CTRL----",
	"cd\nchclr\nchpmt\npwd\nexit",
	"---Number of Commands Run----"
};

void print_help();

int print_help2(int, int);

int print_sfish(int, int);

int doesFileExist(char*);

void redirection(char**);

void making_linked(Assign*, char**);

void piping_action(Assign*, int);

char* cmd_display(int, int, int, int, int, int);

void changeDir(char*);
