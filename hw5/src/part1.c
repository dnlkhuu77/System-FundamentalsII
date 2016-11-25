#include "lott.h"
#include "lott2.h"

static void* map(void*);
static void* reduce(void*);
static char* name(int, int);

//MAKE SURE YOU USE THREAD-SAFE FUNCTIONS (USE strtok_r(3) and not strtok())!!

int part1(){
    //open and search the directory for a file (called map1 ...)
    pthread_t map;
    File_stats* fp;
    fp = malloc(4096*sizeof(File_stats)); //making an array of structs

    static int files = 0;
    DIR* ptr = NULL;
    struct dirent *someptr;
    char path[1024];

    if((ptr = opendir(DATA_TEST)) == NULL) //CHANGE ALL DATA_TEST TO DATA_DIR
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

    printf(
        "Part: %s\n"
        "Query: %s\n",
        PART_STRINGS[current_part], QUERY_STRINGS[current_query]);

    return 0;
}

static void* map(void* v){
    return NULL;
}

static void* reduce(void* v){
    return NULL;
}

static char* name(int type, int thread_num){
    char* s = calloc(30, sizeof(char));
    if(type == 1){ //map
        char* s = "map";
        char* t = calloc(5, sizeof(char));
        sprintf(t, "%d", thread_num);

        strcat(s, t);
    }
    else if(type == 2){
        char* s = "thread";
        char* t = calloc(5, sizeof(char));
        sprintf(t, "%d", thread_num)

        strcat(s, t);
    }
    return s;
}


