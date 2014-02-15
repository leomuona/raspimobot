#include "util.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void delayms(unsigned int howLong)
{
	struct timespec sleeper, dummy;
	sleeper.tv_sec  = (time_t)(howLong / 1000) ;
	sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

	nanosleep (&sleeper, &dummy) ;
}

DirEntry *get_file_entries(const char *directory)
{
	DIR *dirp = opendir(directory);
	if (dirp == NULL) {
		fprintf(stderr, "Unable to opendir %s: %s\n", directory, strerror(errno));
		return NULL;
	}
	closedir(dirp);

	struct dirent **namelist;
	int n;

	n = scandir(directory, &namelist, 0, alphasort);
	if (n < 0) {
		fprintf(stderr, "scandir %s: %s\n", directory, strerror(errno));
		return NULL;
	}

	DirEntry *entry = malloc(sizeof(DirEntry));
	memset(entry, 0, sizeof(DirEntry));

	while (n--) {
		if (namelist[n]->d_type == DT_REG) {
			int i = entry->n++;
			entry->files = realloc(entry->files, sizeof(char *) * (i+1));
			entry->files[i] = malloc(sizeof(char) * 256);
			snprintf(entry->files[i], 256, "%s/%s", directory, namelist[n]->d_name);
		}

		free(namelist[n]);
	}

	free(namelist);

	return entry;
}
