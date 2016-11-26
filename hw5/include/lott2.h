#include <limits.h>
#include <pthread.h>
#include <dirent.h>

typedef struct File_stats{
	pthread_t tid;
	char* filename;
	char* country;
	int duration;
	int user_count;
	struct File_stats* next;
} File_stats;

typedef struct Reduce_stats{
	char* filename;
	char* country;
	int max_duration;
	int max_user_count;
	struct Reduce_stats* next;
} Reduce_stats;