#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <limits.h> 
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>


#define MAX_BYTES 4
#define SURROGATE_SIZE 4
#define NON_SURROGATE_SIZE 2
#define NO_FD -1
#define OFFSET 2

#define FIRST  0
#define SECOND 1
#define THIRD  2
#define FOURTH 3

#ifdef __STDC__
#define P(x) x
#else
#define P(x) ()
#endif

unsigned char a1;
			
unsigned char a2;

unsigned char a3;
			
unsigned char a4;

/** The enum for endianness. */
typedef enum {LITTLE, BIG, UTF8} endianness;

/** The struct for a codepoint glyph. */
typedef struct Glyph {
	unsigned char bytes[MAX_BYTES];
	endianness end;
	bool surrogate;
} Glyph;

/** The given filename. */
char filename[256];
char filename2[256];

/** The usage statement.*/
const char* USAGE[11] = { 
"Command line utility for converting files from UTF-16LE to UTF-16BE or vice versa.\n\n",
"Usage: ./utf [-h|--help] [-v|-vv] -u OUT_ENC | --UTF=OUT_ENC IN_FILE [OUT_FILE]\n\n",
"  Option arugments:\n",
"    -h, --help\t    Displays this usage.\n",
"    -v, -vv\t    Toggles the verbosity of the program to level 1 or 2\n\n",
"  Mandatory argument:\n",
"    -u OUT_ENC, --UTF=OUT_ENC\t Sets the output encoding.\n \t\t\t\t",
" Valid values for OUT_ENC: 16LE, 16BE\n",
"  Positional Arguments:\n",
"    IN_FILE\t    The file to convert.\n",
"    [OUT_FILE]\t    Output file name. If not present, defaults to stdout.\n"
};

/** Which endianness to convert to. */
endianness conversion;

/** Which endianness the source file is in. */
endianness source;


/**
 * A function that swaps the endianness of the bytes of an encoding from
 * LE to BE and vice versa.
 *
 * @param glyph The pointer to the glyph struct to swap.
 * @return Returns a pointer to the glyph that has been swapped.
 */
Glyph* swap_endianness P((Glyph*));

int how_many_bytes(unsigned char[]);

/**
 * Fills in a glyph with the given data in data[2], with the given endianness 
 * by end.
 *
 * @param glyph 	The pointer to the glyph struct to fill in with bytes.
 * @param data[2]	The array of data to fill the glyph struct with.
 * @param end	   	The endianness enum of the glyph.
 * @param fd 		The int pointer to the file descriptor of the input 
 * 			file.
 * @return Returns a pointer to the filled-in glyph.
 */
Glyph* fill_glyph P((Glyph*, unsigned char[], endianness, int*));

/**
 * Writes the given glyph's contents to stdout.
 *
 * @param glyph The pointer to the glyph struct to write to stdout.
 */
void write_glyph P((Glyph*,int));

/**
 * Calls getopt() and parses arguments.
 *
 * @param argc The number of arguments.
 * @param argv The arguments as an array of string.
 */
void parse_args P((int, char**));

/**
 * Prints the usage statement.
 */
void print_help P((void));

/**
 * Closes file descriptors and frees list and possibly does other
 * bookkeeping before exiting.
 *
 * @param The fd int of the file the program has opened. Can be given
 * the macro value NO_FD (-1) to signify that we have no open file
 * to close.
 */
void quit_converter P((int));

void verb1 P((char*));

void verb2 P((char*));

Glyph* convert P((Glyph*, endianness));
