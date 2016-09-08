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


int main(int argc, char** argv) {
    
    int x = validateargs(argc, argv);

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

    int files = 0;

    if(x == 1 || x==2 )
        files = nfiles(argv[2]);
    if (x==3 || x==4)
        files = nfiles(argv[3]);

    if(files == 0){ //no files in the directory
        return EXIT_SUCCESS;
    }
    if(files == -1)
        return EXIT_FAILURE;

    if(x == 1){
        struct Analysis test = {0};

        int help = map(argv[2], analysis_space, sizeof(test), analysis);
        struct Analysis a = analysis_reduce(nfiles(argv[2]), analysis_space);
        analysis_print(a, help, 1);
        return EXIT_SUCCESS;
    }

    
    if(x == 2){
        Stats test_1 = {0};

        map(argv[2], stats_space, sizeof(test_1), stats);

        Stats ans = stats_reduce(nfiles(argv[2]), stats_space);
        stats_print(ans, 1);
        return EXIT_SUCCESS;
    }
    
    if(x == 3){
        struct Analysis test_2 = {0};
        int help_2 = map(argv[3], analysis_space, sizeof(test_2), analysis);

        
        struct Analysis* ptr = analysis_space;

        for(int i = 0; i < files; i++){
            analysis_print(*ptr, 10, 0);
            printf("\n");
            ptr++;
        }

        struct Analysis ans = analysis_reduce(nfiles(argv[3]), analysis_space);
        analysis_print(ans, help_2, 1);
        return EXIT_SUCCESS;
        
    }

    if(x == 4){
        Stats stats_2 = {0};

        map(argv[3], stats_space, sizeof(stats_2), stats);

        Stats* stats_ptr = stats_space;

        for(int i = 0; i < files; i++){
            stats_print(*stats_ptr, 0);
            printf("\n");
            stats_ptr++;
        }

        map(argv[3], stats_space, sizeof(stats_2), stats);

        Stats ans = stats_reduce(nfiles(argv[3]), stats_space);
        stats_print(ans, 1);
        return EXIT_SUCCESS;
    }
    
}
