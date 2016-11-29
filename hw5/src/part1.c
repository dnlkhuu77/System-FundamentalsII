#include "lott.h"
#include "lott2.h"

static void* map(void*);
static void* reduce(void*);
static char* name(char*, int);

//MAKE SURE YOU USE THREAD-SAFE FUNCTIONS (USE strtok_r(3) and not strtok())!!

int part1(){
    char* name_now;
    name_now = calloc(1024, sizeof(char));
    File_stats* head = NULL;
    head = malloc(sizeof(File_stats));
    head->duration = -2;
    int naming_number = 1;

    DIR* ptr = NULL;
    struct dirent *someptr;

    if((ptr = opendir(DATA_TEST)) == NULL) //CHANGE ALL DATA_TEST TO DATA_DIR
        return -1;

    File_stats* current = head;
    while((someptr = readdir(ptr)) != NULL){ //this will go through each individual file
        if(strcmp(someptr->d_name, "..") == 0)
            continue;
        else if (strcmp(someptr -> d_name, ".") == 0)
            continue;
        else if(someptr->d_type == DT_REG){
            pthread_t tid;
            if(current == head && current->duration == -2){
                current->filename = someptr->d_name;
                current->duration = 0; //this will change when we actually implment map
                pthread_create(&tid, NULL, map, current);
                name_now = name(name_now, naming_number);
                pthread_setname_np(tid, name_now);
                current->tid = tid;
            }else{
                current->next = malloc(1024);
                current = current->next;

                current->filename = someptr->d_name;
                pthread_create(&tid, NULL, map, current);
                name_now = name(name_now, naming_number);
                pthread_setname_np(tid, name_now);
                current->tid = tid;
            }
            naming_number++;
        }
    }

    closedir(ptr);

    current = head;
    while(current != NULL){
        printf("ROBO: %s\n", current->filename);
        pthread_join(current->tid, NULL);
        current = current->next;
    }

    reduce(NULL);

    printf(
        "Part: %s\n"
        "Query: %s\n",
        PART_STRINGS[current_part], QUERY_STRINGS[current_query]);

    return 0;
}

static void* map(void* v){ //the static makes the function accessible to part1
    File_stats* abc = (File_stats*) v;
    time_t now;
    struct tm ts;

    char* country_index[10];
    int country_counter[10];
    for(int i = 0; i < 10; i++){
        country_index[i] = NULL;
        country_counter[i] = 0;
    }
    int year_check[138];
    for(int i = 0; i < 138; i++){
        year_check[i] = 0;
    }

    char* opening = calloc(1024, sizeof(char));
    strcat(opening, DATA_TEST);
    strcat(opening, "/");

    strcat(opening, abc->filename);
    printf("Printing file name: %s\n", opening);

    FILE* current_file;
    current_file = fopen(opening, "r");

    //parse the text into its individual stats
    int user_counter = 0;
    int year = 0;
    char* year_string = calloc(1024, sizeof(char));
    int duration = 0;

    char* total_string = calloc(1024, sizeof(char));
    char* unix_string = calloc(1024, sizeof(char));
    char* dur_string = calloc(1024, sizeof(char));
    char* country = calloc(1024, sizeof(char));
    char* ip = calloc(1024, sizeof(char));
    char* rest;

    while(fscanf(current_file, "%s", total_string) != EOF){
        printf("TOTAL STRING: %s\n", total_string);
        int c_duration = 0;
        unix_string = strtok_r(total_string, ",", &rest);
        ip = strtok_r(NULL, ",", &rest);
        dur_string = strtok_r(NULL, ",", &rest);
        country = strtok_r(NULL, ",", &rest);
        printf("UNIX: %s\n", unix_string);

        user_counter++; //raise the counter of users
        c_duration = atoi(dur_string);
        duration = duration + c_duration; //raise the total duration

        now = (time_t) atoi(unix_string); //change the UNIX to time_n
        localtime_r(&now, &ts);

        strftime(year_string, sizeof(year_string), "%Y", &ts);
        printf("Year %s\n", year_string);
        year = atoi(year_string);
        year = year - 1901;
        year_check[year]++;

        int index = 0;
        int fit = 0;
        while(country_index[index] != NULL){
            if(strcmp(country, country_index[index]) == 0){
                country_counter[index]++;
                fit = 1;
                break;
            }
            index++;
        }
        if(fit == 1){
            country_index[index] = strdup(country);
            country_counter[index]++;
        }

    }

    double avg_duration = duration / user_counter;
    abc->duration = avg_duration;

    int nonzero_years = 0;
    for(int i = 0; i < 138; i++){
        if(year_check[i] != 0)
            nonzero_years++;
    }
    abc->user_count = user_counter;
    abc->nonzero_years = nonzero_years;
    abc->avg_usercount = (double) (user_counter/nonzero_years);

    for(int i = 0; i < 10; i++){
        abc->country_index[i] = country_index[i];
        abc->country_counter[i] = country_counter[i];
    }

    int max = 0;
    int max_index = 0;
    for(int i = 0; i < 10; i++){
        if(max < country_counter[i]){ //the first one will be the max
            max = country_counter[i];
            max_index = i;
        }
    }
    abc->country = country_index[max_index];

    fclose(current_file);
    // free(opening);
    // free(total_string);
    // free(unix_string);
    free(ip);
    // free(dur_string);
    // free(country);

    return abc;
}

static void* reduce(void* v){
   /* if(strcmp(QUERY_STRINGS[current_query], "A") == 0 || strcmp(QUERY_STRINGS[current_query], "B") == 0){
        printf("%s\n", A_REDUCE);
    }
    else if(strcmp(QUERY_STRINGS[current_query], "C") == 0 || strcmp(QUERY_STRINGS[current_query], "D") == 0){
        printf("%s\n", B_REDUCE);
    }
    else if(strcmp(QUERY_STRINGS[current_query], "E") == 0){
        printf("%s\n", C_REDUCE);
    }*/
    return NULL;
}

static char* name(char* s, int thread_num){
    memset(s, 0, strlen(s));
    strcat(s, "map");

    char* t = malloc(5);
    sprintf(t, "%d", thread_num);

    strcat(s, t);
    return s;
}


