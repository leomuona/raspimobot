#include "bmp.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void BMP_init(BMP *bmp, int width, int height)
{
	bmp->width     = width;
	bmp->height    = height;
	bmp->fluff     = ((4-(width*3)%4)%4);
	bmp->data_size = 3*width*height+bmp->fluff*height;
	bmp->filesize  = 54 + bmp->data_size;
	bmp->data      = malloc(bmp->data_size);
	memset(bmp->data, 0, bmp->data_size);
}

void BMP_free(BMP *bmp)
{
	free(bmp->data);
}

void BMP_put(BMP *bmp, int x, int y, RGB *rgb)
{
	int idx = 3*(x+y*bmp->width) + y*bmp->fluff;
	bmp->data[idx+2] = rgb->r;
	bmp->data[idx+1] = rgb->g;
	bmp->data[idx+0] = rgb->b;
}

void BMP_get(BMP *bmp, int x, int y, RGB *rgb)
{
	int idx = 3*(x+y*bmp->width) + y*bmp->fluff;
	rgb->r = bmp->data[idx+2];
	rgb->g = bmp->data[idx+1];
	rgb->b = bmp->data[idx+0];
}

void BMP_write(BMP *bmp, const char *filename)
{
	char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

	bmpfileheader[ 2] = (unsigned char)(bmp->filesize    );
	bmpfileheader[ 3] = (unsigned char)(bmp->filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(bmp->filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(bmp->filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(bmp->width    );
	bmpinfoheader[ 5] = (unsigned char)(bmp->width>> 8);
	bmpinfoheader[ 6] = (unsigned char)(bmp->width>>16);
	bmpinfoheader[ 7] = (unsigned char)(bmp->width>>24);
	bmpinfoheader[ 8] = (unsigned char)(bmp->height    );
	bmpinfoheader[ 9] = (unsigned char)(bmp->height>> 8);
	bmpinfoheader[10] = (unsigned char)(bmp->height>>16);
	bmpinfoheader[11] = (unsigned char)(bmp->height>>24);

	FILE *fd = fopen(filename, "w");
	if (!fd) {
		fprintf(stderr, "Unable to create file %s: %s\n", filename, strerror(errno));
		return;
	}

	fwrite(&bmpfileheader[0], sizeof(char), 14, fd);
	fwrite(&bmpinfoheader[0], sizeof(char), 40, fd);

	int w = bmp->width;
	int h = bmp->height;
	int f = bmp->fluff;
	int i;

	for (i=0; i<bmp->height; ++i) {
		fwrite(bmp->data+(w*(h-i-1)*3)+(h-i-1)*f, sizeof(char), 3*w+f, fd);
	}

	fclose(fd);
}

void BMP_read(BMP *bmp, const char *filename)
{
	FILE* fd = fopen(filename, "rb");
	if (!fd) {
		fprintf(stderr, "Unable to open file %s: %s\n", filename, strerror(errno));
		return;
	}
	char header[54];
	fread(header, sizeof(char), 54, fd);

	BMP_init(bmp, *(int*) &header[18], *(int*) &header[22]);

	int w = bmp->width*3+bmp->fluff;
	int i;

	for (i=0; i<bmp->height; ++i) {
		fread(bmp->data+i*w, sizeof(char), w, fd);
	}

	fclose(fd);

	// array swap, bmp's are saved upside down
	for (i=0; i<bmp->height/2; i++) {
		unsigned char *tmp = malloc(w);
		memcpy(tmp, bmp->data+w*i, w);
		memcpy(bmp->data+w*i, bmp->data+w*(bmp->height-i-1), w);
		memcpy(bmp->data+w*(bmp->height-i-1), tmp, w);
		free(tmp);
	}
}
