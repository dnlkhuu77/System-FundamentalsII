#include "map_reduce.h"
#include <string.h>

//Space to store the results for analysis map
struct Analysis analysis_space[NFILES];
//Space to store the results for stats map
Stats stats_space[NFILES];

//Sample Map function action: Print file contents to stdout and returns the number bytes in the file.
int cat(FILE* f, void* res, char* filename) {
    char c;
    int n = 0;
    printf("%s\n", filename);
    while((c = fgetc(f)) != EOF) {
        printf("%c", c);
        n++;
    }
    printf("\n");
    return n;
}

int analysis(FILE* f, void* res, char* filename){
    struct Analysis a;
    struct Analysis* ptr = (struct Analysis*) res; //cast the void pointer

    a.filename = filename;
    char c;
    int n = 0; //calculates bytes
    int n_line = 0; //calculates characters per line
    int n_max = 0;
    int line_counter = 0; //the longest line
    int line_max = 0;
    //int ascii[128]; //count each ascii character

    printf("%s\n", filename);

    while((c = fgetc(f)) != EOF) {
        printf("%c", c);
        n++; //increment the bytes of the file
        n_line++; //increment the bytes of the line

        if(c == '\n'){
            if(n_line >= n_max){
                n_max = n_line;
                line_max = line_counter;
            }

            n_line = 0; //reset the bytes of a line 
            line_counter++; //increment the line count
        }

    }
    a.lnlen = n_max;
    a.lnno = line_max;
    *ptr = a;

    printf("\n");
    return n;
}

int main(int argc, char** argv) {
    /*
    int x = validateargs(argc, argv);
    printf("%d \n", x);
    if(x == -1){
        printf("Usage: ./mapreduce [h|v] FUNC DIR \n");
        printf("FUNC\tWhich operation you would like to run on the data:\n");
        printf("\tana - Analysis of various text files in a directory.\n");
        printf("\tstats - Calculates stats on files which contain only numbers.\n");
        printf("DIR\tThe directory in which the files are located.\n \n");
        printf("Options: \n");
        printf("-h\tPrints this help menu.\n");
        printf("-v\tPrints the map function's results, stating this file it's from.\n");
        return EXIT_FAILURE;
    }
    if(x == 0){
        printf("Usage: ./mapreduce [h|v] FUNC DIR \n");
        printf("FUNC\tWhich operation you would like to run on the data:\n");
        printf("\tana - Analysis of various text files in a directory.\n");
        printf("\tstats - Calculates stats on files which contain only numbers.\n");
        printf("DIR\tThe directory in which the files are located.\n \n");
        printf("Options: \n");
        printf("-h\tPrints this help menu.\n");
        printf("-v\tPrints the map function's results, stating this file it's from.\n");
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
    */

    /*
    int files = nfiles(argv[1]);
    printf("%d\n", files);

    if(files == 0){
        return EXIT_SUCCESS;
    }
    if(files == -1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS; // >1
    */

    int help = map(argv[1], analysis_space, sizeof(analysis_space), cat);
    printf("%d\n", help);
    return EXIT_SUCCESS;

    //call on the reduce function
    /*
    analysis_space = analysis_reduce(help, analysis_space);

    if(analysis_space == NULL)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
    */
}
