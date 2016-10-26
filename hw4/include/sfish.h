#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

/** The usage statement.*/
const char* USAGE[8] = { 
"These shell commands are defined internally. Type `help' to see this list.\n\n",
"help\n",
"exit\n",
"cd\n",
"pwd\n",
"prt\n",
"chpmt\n",
"chclr\n"
};

void print_help();

char* cmd_display(int, int);

void changeDir(char*);