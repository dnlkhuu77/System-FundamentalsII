//**DO NOT** CHANGE THE PROTOTYPES FOR THE FUNCTIONS GIVEN TO YOU. WE TEST EACH
//FUNCTION INDEPENDENTLY WITH OUR OWN MAIN PROGRAM.
#include "map_reduce.h"
#include <string.h>
#include <dirent.h>

//Implement map_reduce.h functions here.
int validateargs(int argc, char** argv){ //2, 3, 4 arguments
	if (strcmp(argv[1], "-h") == 0){ //check to see if first flag is -h
		return 0;
	}
	if(argc < 2 || argc > 4){
		return -1;
	}

	if(strcmp(argv[1], "ana") == 0){
		if(argv[2] == NULL || argv[3] != NULL)
			return -1;

		DIR* dir = opendir(argv[2]);
		if(dir){
			closedir(dir);
			return 1;
		}
		else
			return -1;
	}

	if(strcmp(argv[1], "stats") == 0){
		if(argv[2] == NULL || argv[3] != NULL)
			return -1;

		DIR* dir = opendir(argv[2]);
		if(dir){
			closedir(dir);
			return 2;
		}
		else
			return -1;
	}

	if(strcmp(argv[1], "-v") == 0){
		if(argv[2] == NULL || argv[3] == NULL)
			return -1;

		if(strcmp(argv[2], "ana") == 0){
			DIR* dir = opendir(argv[3]);
				if(dir){
					closedir(dir);
					return 3;
				}
				else
					return -1;
		}

		if(strcmp(argv[2], "stats") == 0){
			DIR* dir = opendir(argv[3]);
				if(dir){
					closedir(dir);
					return 4;
				}
				else
					return -1;
		}
	}

	return -1;
}
/*int nfiles(char* dir){
	int files = 0;
	DIR *ptr;
	struct 
}*/