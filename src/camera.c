#include "camera.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WIDTH 640
#define HEIGHT 480
#define ARR_SIZE WIDTH*HEIGHT*3
#define BMP_OFFSET 54

void take_pic()
{
	char* pic = malloc(ARR_SIZE);
	memset(pic, 0, ARR_SIZE);
	// take picture calling raspistill and put it to char array

	FILE* pf = popen("raspistill -o - -e bmp -w 640 -h 480", "r");
	//FILE* pf = popen("echo wololoo", "r");

	if (!pf){
		perror("Could not open pipe for read-only.");
		return;
	}

	fseek(pf, BMP_OFFSET, SEEK_SET);
	fgets(pic, ARR_SIZE, pf);

	// this is just testing so free the array here
	free(pic);
}
