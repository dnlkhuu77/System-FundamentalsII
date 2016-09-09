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
	DIR* ptr = NULL;
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
		return 0;
	}
	else
		return files;
}

int map(char* dir, void* results, size_t size, int(*act)(FILE* f, void* res, char* fn)){
	DIR* ptr = NULL;
	struct dirent *someptr;
	char path[1024];
	int result = 0;
	memset(results, '$', size); //initialize the array

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

			FILE* fp = NULL;
			if((fp = fopen(ans, "r")) == NULL){
				return -1; //error in opening file
			}
			else{		
				int acty = act(fp, results, someptr->d_name);
				results = results + size; //might have to increment by 4
				result = result + acty; //add up all the results of act
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

		for(int j = 0; j < 128; j++){
			ans.ascii[j] = ans.ascii[j] + imm.ascii[j];
		}

		ptr++; 
	}

	ans.lnlen = max_length;
	ans.lnno = max_line;
	ans.filename = filename;
	return ans;
}

Stats stats_reduce(int n, void* results){
	Stats ans = {0};
	Stats imm = {0};
	Stats* ptr = (Stats*) results;
	int n_count = 0; //n is total number of numbers; sum is total sum
	int sum = 0;

	for(int i = 0; i < n; i++){
		imm = *ptr;
		//printf("Reduce Sum: %d\n", imm.sum);
		sum = sum + imm.sum;
		n_count = n_count + imm.n;
		//printf("Reduce n: %d\n", imm.n);

		for(int j = 0; j < NVAL; j++){
			ans.histogram[j] = ans.histogram[j] + imm.histogram[j];
		}
		ptr++;
	}

	ans.sum = sum;
	ans.n = n_count;
	ans.filename = NULL;
	return ans;
}

void analysis_print(struct Analysis res, int nbytes, int hist){
	printf("File: %s\n", res.filename);
	printf("Longest line length: %d\n", res.lnlen);
	printf("Longest line number: %d\n", res.lnno);

	if(hist != 0){ //print the histogram
		printf("Total Bytes in directory: %d\n", nbytes);
		printf("Histogram: \n");

		for(int i = 0; i < 128; i++){
			if(res.ascii[i] != 0){
				printf("%d: ", i);

				for(int j = 0; j < res.ascii[i]; j++){
					printf("-");
				}
				//printf("%d", res.ascii[i]);
				printf("\n");
			}
		}
	}
}

void stats_print(Stats res, int hist){

	if(hist != 0){
		printf("Histogram: \n");

		for(int i = 0; i < NVAL; i++){
			if(res.histogram[i] != 0){
				printf("%d:", i);

				for(int j = 0; j < res.histogram[i]; j++){
					printf("-");
				}
				printf("\n");
			}

		}

	}

	if(res.filename != NULL){
		printf("File: %s\n", res.filename);
	}

	printf("Count: %d\n", res.n);
	//printf("Sum %d\n", res.sum);

	float mean = (float)res.sum / (float) res.n;
	printf("Mean: %.6f\n", mean);


	int max = 0; //use a array to count multiple occurence
	for(int i = 0; i < NVAL; i++){
		if(max < res.histogram[i])
			max = res.histogram[i];
	}
	printf("Mode: ");
	for(int i = 0; i < NVAL; i++){
		if(res.histogram[i] == max)
			printf("%d ", i);
	}
	printf("\n");


	float median = 0;
	int med_index = res.n / 2; //(the middle number in the list)
	int med_count = 0;
	int marker = 0;

	if(med_index % 1 != 0){ //if odd, pick the middle number
		med_index++;
		
		for(int i = 0; i < NVAL; i++){
			if(marker != 0)
				break;

			med_count = med_count + res.histogram[i];
			if(med_count >= med_index){
				median = i;
				marker = 1;
			}
		}
		
	}
	else{ //even number of integers

		for(int i = 0; i < NVAL; i++){
			if(marker != 0)
				break;
			med_count = med_count + res.histogram[i];

			if(med_count >= med_index){
				if(med_count == med_index){
					median = (float) (i + (i + 1)) / 2;
					marker = 1;
				}
				else{
					median = (float) (i + i) / 2;
					marker = 1;
				}
			}
		}

	}
	printf("Median: %.6f\n", median);


	float q1, q3 = 0;
	int count = 0;
	int marker_2 = 0;
	int q1_index = res.n * 0.25;
	int q3_index = res.n * 0.75;

	if(q1_index % 1 !=0 || q1_index == 0){
		q1_index++;

		for(int i = 0; i < NVAL; i++){
			if(marker_2 != 0)
				break;

			count = count + res.histogram[i];
			if(count >= q1_index){
				q1 = i;
				marker_2 = 1;
			}
		}
	}
	else{

		for(int i = 0; i < NVAL; i++){
			if(marker_2 != 0)
				break;

			count = count + res.histogram[i];
			if(count >= q1_index){
				if(count == q1_index){
					q1 = (float) (i + (i + 1)) / 2;
					marker_2 = 1;
				}
				else{
					q1 = (float) (i + i) / 2;
					marker_2 = 1;
				}
			}
		}
	}

	marker_2 = 0;
	count = 0;

	if(q3_index % 1 !=0 || q3_index == 1){
		q3_index++;

		for(int i = 0; i < NVAL; i++){
			if(marker_2 != 0)
				break;

			count = count + res.histogram[i];
			if(count >= q3_index){
				q3 = i;
				marker_2 = 1;
			}
		}
	}
	else{

		for(int i = 0; i < NVAL; i++){
			if(marker_2 != 0)
				break;

			count = count + res.histogram[i];
			if(count >= q3_index){
				if(count == q3_index){
					q3 = (float) (i + (i + 1)) / 2;
					marker_2 = 1;
				}
				else{
					q3 = (float) (i + i) / 2;
					marker_2 = 1;
				}
			}
		}
	}

	printf("Q1: %.6f\n", q1);
	printf("Q3: %.6f\n", q3);

	int min, max_c = 0;
	for(int i = 0; i < NVAL; i++){
		if(res.histogram[i] != 0)
			max_c = i;
	}
	for(int i = NVAL - 1; i >= 0; i--){
		if(res.histogram[i] != 0)
			min = i;
	}
	printf("Min: %d\n", min);
	printf("Max: %d\n", max_c);

}

int analysis(FILE* f, void* res, char* filename){
    struct Analysis a = {0};
    struct Analysis* ptr = (struct Analysis*) res; //cast the void pointer
    //printf("Currenly handing %s/n", filename);
    a.filename = strdup(filename);
    char c;
    int n = 0; //calculates bytes
    int n_line = 0; //calculates characters per line
    int n_max = 0;
    int line_counter = 0; //the longest line
    int line_max = 0;
    int ascii[128]; //count each ascii character
    memset(ascii, 0, 128); //initialize the array

    while((c = fgetc(f)) != EOF) {
        //printf("%c", c);
        n++; //increment the bytes of the file
        n_line++; //increment the bytes of the line

        a.ascii[(int) c]++;

        if(c == '\n'){
            if(n_line >= n_max){
                n_max = n_line;
                line_max = line_counter;
            }

            n_line = 0; //reset the bytes of a line 
            line_counter++; //increment the line count
        }
    }

    line_max = line_max + 1;
    n_max = n_max - 1;

    a.lnlen = n_max;
    a.lnno = line_max;

    *ptr = a;

    printf("\n");
    return n;
}

int stats(FILE* f, void* res, char* filename){
    Stats s = {0};
    Stats* ptr = (Stats*) res;

    s.filename = strdup(filename);
    int c; //the current individual number
    int n = 0; //count how many numbers are in the file
    int sum = 0;
    int histogram[NVAL];
    memset(histogram, 0, NVAL);

    if(fscanf(f, "%d", &c) == EOF)
    	return -1;
    rewind(f);

    while(fscanf(f, "%d", &c) != EOF) {
    	if(c >= 0 || c < NVAL){
        	n++; //increment the number counter
        	sum = sum + c; 

        	s.histogram[c]++;
    	}
    }

    s.sum = sum;
    //printf("Current Sum %d\n", sum);
    s.n = n;
    //printf("Current n: %d\n", n);

    *ptr = s;
    printf("\n");
    return 0;
}