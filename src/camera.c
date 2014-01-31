#include "camera.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void read_output(FILE *fd, size_t count, char *buffer)
{
	size_t left = count;
	while (left != 0) {
		ssize_t r = fread(buffer, sizeof(char), count, fd);
		if (r == 0)
			break;
		else
			left -= r;
	}
}

int take_pic(BMP *bmp)
{
	char cmd[255] = {0};
	sprintf(cmd, "raspistill -o - -e bmp -w %d -h %d", bmp->width, bmp->height);
	printf("exec: %s\n", cmd);
	FILE* pf = popen(cmd, "r");
	if (!pf) {
		perror("Could not open pipe for read-only.");
		return -1;
	}

	char header[54] = {0};
	read_output(pf, 54, header);
	read_output(pf, bmp->data_size, bmp->data);
	// TODO: rows are in wrong order, swap dem

	return 0;
}
