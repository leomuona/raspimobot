#ifndef __UTIL_H__
#define __UTIL_H__

struct DirEntry
{
	char **files;
	int n;
};
typedef struct DirEntry DirEntry;

void delayms(unsigned int howLong);

DirEntry *get_file_entries(const char *directory);

#endif // __UTIL_H__
