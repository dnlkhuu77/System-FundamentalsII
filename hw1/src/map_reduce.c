//**DO NOT** CHANGE THE PROTOTYPES FOR THE FUNCTIONS GIVEN TO YOU. WE TEST EACH
//FUNCTION INDEPENDENTLY WITH OUR OWN MAIN PROGRAM.
#include "map_reduce.h"
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

//Implement map_reduce.h functions here.
int validateargs(int argc, char** argv){ //2, 3, 4 arguments
	if(argc == 1 || argc == 0)
		return -1;

	if (strcmp(argv[1], "-h") == 0){ //check to see if first flag is -h
		return 0;
	}
	if(argc > 4){
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
	DIR* ptr;
	struct dirent *someptr;
	char path[1024];

	if((ptr = opendir(dir)) == NULL)
		return -1;

	while((someptr = readdir(ptr)) != NULL){
		if(strcmp(someptr->d_name, "..") == 0)
			continue;
		else if (strcmp(someptr -> d_name, ".") == 0)
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

int map(char* dir, void* results, size_t size, int(*act)(FILE* f, void* res, char* fn)){
	DIR* ptr;
	struct dirent *someptr;
	char path[1024];
	int result = 0;
	int* x = (int*) results; //has to be int because the struct will be this size
	memset(x, '$', size); //initialize the array


	if((ptr = opendir(dir)) == NULL)
		return -1;

	while((someptr = readdir(ptr)) != NULL){
		if(strcmp(someptr->d_name, "..") == 0)
			continue;
		else if (strcmp(someptr -> d_name, ".") == 0)
			continue;
		else if(someptr->d_type == DT_DIR){
			path[0] = '\0';
			strncat(path, dir, sizeof(path) - 1);
			strncat(path, "/", sizeof(path) - 1);
			strncat(path, someptr->d_name, sizeof(path) - 1);
			map(path, results, size, act);
		}
		else if(someptr->d_type == DT_REG){ //it's a regular file
			char *ans = malloc(strlen(dir) + strlen(someptr->d_name) + 2);
			strcpy(ans, dir);
			strcat(ans, "/");
			strcat(ans, someptr->d_name);

			//printf("%s\n", ans); //testing shit

			FILE* fp = NULL;
			if((fp = fopen(ans, "r")) == NULL){
				return -1; //error in opening file
			}
			else{		
				int acty = act(fp, x, someptr->d_name);
				*x = acty;//store the result into a space in results[]
				*x = *x + size; //might have to increment by 4
				result = result + acty; //add up all the results of act
				//results[] = result;
				//results = results + size;
			}
			fclose(fp);
		}
	}
	closedir(ptr);
	return result; //go through the array and add them up
}

struct Analysis analysis_reduce(int n, void* results){
	struct Analysis ans = {0}; //make a struct
	struct Analysis imm = {0};
	struct Analysis* ptr = (struct Analysis*) results;
	int current_length, max_length = 0;
	int max_line = 0;
	char* filename;

	for(int i = 0; i < n; i++){
		imm = *ptr;
		current_length = imm.lnlen;

		if(current_length >= max_length){
			max_length = current_length;
			max_line = imm.lnno;
			filename = imm.filename;
		}

		for(int j = 0; j < 127; j++){
			ans.ascii[j] = ans.ascii[j] + imm.ascii[j];
		}

		ptr++;
	}

	ans.lnlen = max_length;
	ans.lnno = max_line;
	ans.filename = filename;
	return ans;
}


int analysis(FILE* f, void* res, char* filename){
    struct Analysis a = {0};
    struct Analysis* ptr = (struct Analysis*) res; //cast the void pointer

    a.filename = strdup(filename);
    char c;
    int n = 0; //calculates bytes
    int n_line = 0; //calculates characters per line
    int n_max = 0;
    int line_counter = 0; //the longest line
    int line_max = 0;
    int ascii[128]; //count each ascii character
    memset(ascii, 0, 128); //initialize the array

    printf("%s\n", filename);

    while((c = fgetc(f)) != EOF) {
        printf("%c", c);
        n++; //increment the bytes of the file
        n_line++; //increment the bytes of the line

        for(int i = 0; i < 127; i++){
            if(c == i)
                ascii[i]++;
        }

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

    for(int i = 0; i < 127; i++){
    	a.ascii[i] = ascii[i];
    }

    *ptr = a;

    printf("\n");
    return n;
}

int stats(FILE* f, void* res, char* filename){
    Stats s = {0};
    struct Stats* ptr = (struct Stats*) res;

    s.filename = strdup(filename);
    int c; //the current individual number
    int n = 0; //count how many numbers are in the file
    int sum = 0;
    int histogram[NVAL];
    memset(histogram, 0, NVAL);

    printf("%s\n", filename);

    if(fscanf(f, "%d", &c) == EOF)
    	return -1;
    rewind(f);

    while(fscanf(f, "%d", &c) != EOF) {
        n++; //increment the number counter
        sum = sum + c; 

        for(int i = 0; i < NVAL; i++){
        	if(c == i){
        		histogram[i]++;
        	}
        }
    }

    s.sum = sum;
    s.n = n;
    printf("n %d\n", n);
    printf("Final Sum %d\n", sum);

    for(int i = 0; i < NVAL; i++){
    	s.histogram[i] = histogram[i];
    }

    *ptr = s;
    printf("\n");
    return 0;
}