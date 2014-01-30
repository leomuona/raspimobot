#include "camera.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BMP_OFFSET 54

int take_pic(char* pic, int width, int height)
{
	const int ARR_SIZE = width*height*3;

	FILE* pf = popen("raspistill -o - -e bmp -w 640 -h 480", "r");

	if (!pf){
		perror("Could not open pipe for read-only.");
		return -1;
	}

	fseek(pf, BMP_OFFSET, SEEK_SET);
	fgets(pic, ARR_SIZE, pf);

	pclose(pf);

	return 0;
}
