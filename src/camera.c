#include "camera.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

#define CAMERA_FOV_VERTICAL 41*(M_PI/180)
#define CAMERA_FOV_HORIZONTAL 54*(M_PI/180)

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
	sprintf(cmd, "raspistill -o - -e bmp -w %d -h %d -t 0", bmp->width, bmp->height);
	printf("exec: %s\n", cmd);
	FILE* pf = popen(cmd, "r");
	if (!pf) {
		perror("Could not open pipe for read-only.");
		return -1;
	}

	char header[54] = {0};
	read_output(pf, 54, header);
	read_output(pf, bmp->data_size, bmp->data);

	// rows are in wrong order, swap dem
	int w = bmp->width*3+bmp->fluff;
	unsigned char *tmp = malloc(w);
	int i;
	for (i=0; i<bmp->height/2; i++) {
		memcpy(tmp, bmp->data+w*i, w);
		memcpy(bmp->data+w*i, bmp->data+w*(bmp->height-i-1), w);
		memcpy(bmp->data+w*(bmp->height-i-1), tmp, w);
	}
	free(tmp);

	return 0;
}

int calc_rotation(DetectionDiff* diff, float angles[2])
{
	if (diff->min_x > diff->max_x || diff->min_y > diff->max_y){
		// no diff
		return 1;
	}
	int max_x = diff->first_pass->width;
	int max_y = diff->first_pass->height;

	int center_x, center_y;

	center_x = (diff->min_x + diff->max_x) / 2;
	center_y = (diff->min_y + diff->max_y) / 2;

	int orig_x = max_x / 2;
	int orig_y = max_y / 2;

	float relative_angle_hor, relative_angle_ver;

	if (center_x == orig_x && center_y == orig_y){
		// diff center is in origo
		return 1;
	}

	if (center_x != orig_x){
		float relative_angle_hor = center_x / (float) orig_x - 1.0f;
	}
	else{
		relative_angle_hor = .0f;
	}

	if (center_y != orig_y){
		float relative_angle_ver = center_y / (float) orig_y - 1.0f;
	}
	else{
		relative_angle_ver = .0f;
	}

	angles[0] = (CAMERA_FOV_HORIZONTAL / 2.0f) * relative_angle_hor;
	angles[1] = (CAMERA_FOV_VERTICAL / 2.0f) * relative_angle_ver;

	return 0;
}
