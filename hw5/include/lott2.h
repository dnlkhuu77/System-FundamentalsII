#include <limits.h>

typedef struct File_stats{
	char* filename;
	char* country;
	int duration;
	int user_count;
} File_stats;

typedef struct Reduce_stats{
	char* filename;
	char* country;
	int max_duration;
	int max_user_count;
} Reduce_stats;