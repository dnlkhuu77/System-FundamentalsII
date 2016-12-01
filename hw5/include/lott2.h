#include <limits.h>
#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>

typedef struct File_stats{
	pthread_t tid;
	char* filename;
	char* filename_t;
	char* country;
	double duration;
	int user_count;
	int nonzero_years;
	double avg_usercount;
	char* country_index[10];
	int country_counter[10];
	struct File_stats* next;
} File_stats;

typedef struct Reduce_stats{
	char* max_file;
	char* min_file;
	double max_durr;
	double min_durr;
	double max_users;
	double min_users;
	char* country;
} Reduce_stats;