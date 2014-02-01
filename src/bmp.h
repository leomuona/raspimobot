#ifndef BMP_H
#define BMP_H

struct RGB
{
	char r, g, b;
};
typedef struct RGB RGB;

struct BMP
{
	int width, height;
	int filesize;

	int fluff;

	char *data;
	int data_size;
};
typedef struct BMP BMP;

void BMP_init(BMP *bmp, int width, int height);
void BMP_free(BMP *bmp);

void BMP_put(BMP *bmp, int x, int y, RGB *rgb);

void BMP_get(BMP *bmp, int x, int y, RGB *rgb);

void BMP_write(BMP *bmp, const char *filename);

void BMP_read(BMP *bmp, const char *filename);

void RGB_diff(RGB *a, RGB *b, RGB *diff);

#endif // BMP_H
