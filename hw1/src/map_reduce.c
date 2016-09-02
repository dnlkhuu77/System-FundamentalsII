//**DO NOT** CHANGE THE PROTOTYPES FOR THE FUNCTIONS GIVEN TO YOU. WE TEST EACH
//FUNCTION INDEPENDENTLY WITH OUR OWN MAIN PROGRAM.
#include "map_reduce.h"
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

//Implement map_reduce.h functions here.
int validateargs(int argc, char** argv){ //2, 3, 4 arguments
	if(argc == 1)
		return -1;

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

int nfiles(char* dir){
	static int files = 0;
	DIR* ptr; //a pointer at the files (opdir)
	struct dirent *someptr; //redir
	char path[1024];

	if((ptr = opendir(dir)) == NULL)
		return -1;

	while((someptr = readdir(ptr)) != NULL){
		if(strcmp(someptr->d_name, ".") == 0)
			continue;
		else if (strcmp(someptr -> d_name, "..") == 0)
			continue;
		else if(someptr->d_type == DT_DIR){
			path[0] = '\0';
			strncat(path, dir, sizeof(path) - 1);
			strncat(path, "/", sizeof(path) - 1);
			strncat(path, someptr->d_name, sizeof(path) - 1);
			nfiles(path);
		}
		else if(someptr->d_type == DT_REG)
			files++;
	}

	closedir(ptr);

	if(files == 0){
		printf("No files present in the directory.\n");
		return 0;
	}
	else
		return files;
}