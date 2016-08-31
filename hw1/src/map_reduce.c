//**DO NOT** CHANGE THE PROTOTYPES FOR THE FUNCTIONS GIVEN TO YOU. WE TEST EACH
//FUNCTION INDEPENDENTLY WITH OUR OWN MAIN PROGRAM.
#include "map_reduce.h"
#include <string.h>

//Implement map_reduce.h functions here.
int validateargs(int argc, char** argv){ //2, 3, 4 arguments

	if (strcmp(argv[1], "-h") == 0){
		printf("FUNC\tWhich operation you would like to run on the data:\n");
		printf("ana - Analysis of various text files in a directory.\n");
		printf("stats - Calculates stats on files which contain only numbers.\n");
		printf("DIR\tThe directory in which the files are located.\n \n");
		printf("Options: \n");
		printf("-h\tPrints this help menu.\n");
		printf("-v\tPrints the map function's results, stating this file it's from.\n");
		return EXIT_SUCCESS; //or 0
	}

	if(argc < 2 || argc > 4){
		printf("FUNC\nWhich operation you would like to run on the data:\n");
		printf("ana - Analysis of various text files in a directory.\n");
		printf("stats - Calculates stats on files which contain only numbers.\n");
		printf("DIR\nThe directory in which the files are located.\n \n");
		printf("Options: \n");
		printf("-h\tPrints this help menu.\n");
		printf("-v\tPrints the map function's results, stating this file it's from.\n");
		return -1; //1
	}

	if(strcmp(argv[1], "ana") == 0){
		return 1;
	}
	if(strcmp(argv[1], "stats") == 0){
		return 2;
	}
	if(strcmp(argv[1], "-v") == 0){
		if(argv[2] == NULL)
			return -1;
		if(strcmp(argv[2], "ana") == 0)
			return 3;
		if(strcmp(argv[2], "stats") == 0)
			return 4;
	}

	printf("FUNC\tWhich operation you would like to run on the data:\n");
	printf("ana - Analysis of various text files in a directory.\n");
	printf("stats - Calculates stats on files which contain only numbers.\n");
	printf("DIR\tThe directory in which the files are located.\n \n");
	printf("Options: \n");
	printf("-h\tPrints this help menu.\n");
	printf("-v\tPrints the map function's results, stating this file it's from.\n");
	return -1;
}

