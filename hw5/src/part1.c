#include "lott.h"
#include "lott2.h"

static void* map(void*);
static void* reduce(void*);
static char* name(char*, int);

//MAKE SURE YOU USE THREAD-SAFE FUNCTIONS (USE strtok_r(3) and not strtok())!!

int part1(){
    //open and search the directory for a file
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
    reduce(NULL);

    current = head;
    while(current != NULL){
        printf("ROBO: %s\n", current->filename);
        pthread_join(current->tid, NULL);
        current = current->next;
    }

    printf(
        "Part: %s\n"
        "Query: %s\n",
        PART_STRINGS[current_part], QUERY_STRINGS[current_query]);

    return 0;
}

static void* map(void* v){ //the static makes the function accessible to part1
    File_stats* abc = (File_stats*) v; //this is the 
    printf("Printing file name: %s\n", abc->filename);
    //FILE* current_file = NULL;
    //current_file = fopen(abc->filename, "r");

    //parse the text into its individual stats

    return NULL;
}

static void* reduce(void* v){
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


