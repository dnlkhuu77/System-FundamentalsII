#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

 typedef struct Assign{
 	char** args;
 	struct Assign *next;
 } Assign;

/** The usage statement.*/
const char* USAGE[9] = { 
	"SFISH Bash, v.1\n"
	"These shell commands are defined internally. Type `help' to see this list.\n\n",
	"help [-dms] [pattern ...]\n",
	"exit [n]\n",
	"cd [-L|[-P [-e]] [-@]] [dir]\n",
	"pwd [-LP]\n",
	"prt\n",
	"chpmt\n",
	"chclr\n"
};

void print_help();

int doesFileExist(char*);

void redirection(char**);

void making_linked(Assign*, char**);

char** remove_pipe(char**);

void piping_action(Assign*, int);

char* cmd_display(int, int, int, int, int, int);

void changeDir(char*);