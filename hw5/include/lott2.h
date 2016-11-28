#include <limits.h>
#include <pthread.h>
#include <dirent.h>

char* country_index[10];
int country_counter[10];

typedef struct File_stats{
	pthread_t tid;
	char* filename;
	char* country;
	double duration;
	int user_count;
	struct File_stats* next;
} File_stats;

typedef struct Reduce_stats{
	char* filename;
	char* country;
	double max_duration;
	int max_user_count;
	struct Reduce_stats* next;
} Reduce_stats;