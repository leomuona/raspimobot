#include "motion_detect.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

RGB background = { 0, 0, 0 };
RGB foreground = { 255, 255, 255 };
RGB framecolor = { 255, 0, 0 };

DetectionDiff *motion_detect(BMP *a, BMP *b, int difference_threshold, int noise_filter_size)
{
	assert(a->width == b->width && a->height == b->height);

	int i, j;
	int min_x = INT_MAX;
	int min_y = INT_MAX;
	int max_x = INT_MIN;
	int max_y = INT_MIN;

	// first-pass marks the pixels that are changed between the pixels.
	BMP *first_pass  = malloc(sizeof(BMP));
	BMP *second_pass = malloc(sizeof(BMP));
	BMP_init(first_pass, a->width, a->height);
	BMP_init(second_pass, a->width, a->height);

	RGB diff, rgb_a, rgb_b;

	for (i=0; i<a->width; ++i) {
		for (j=0; j<a->height; ++j) {
			BMP_get(a, i, j, &rgb_a);
			BMP_get(b, i, j, &rgb_b);
			RGB_diff(&rgb_a, &rgb_b, &diff);

			if ((diff.r + diff.g + diff.b)/3 >= difference_threshold) {
				BMP_put(first_pass, i, j, &foreground);
			} else {
				BMP_put(first_pass, i, j, &background);
			}
		}
	}

	// second-pass is erosion filter => ignore minor differences.
	int m = noise_filter_size, n = (m - 1)/2;
	for (i=n; i<a->width-n; ++i) {
		for (j=n; j<a->height-n; ++j) {
			//count the number of marked pixels in current window
			int marked = 0;
			int x, y;
			for (x=i-n; x<i+n; ++x) {
				for (y=j-n; y<j+n; ++y) {
					BMP_get(first_pass, x, y, &diff);
					if (diff.r == foreground.r)
						marked += 1;
				}
			}

			if (marked >= m) {
				for (x=i-n; x<i+n; ++x) {
					for (y=j-n; y<j+n; ++y) {
						BMP_put(second_pass, x, y, &foreground);
						min_x = MIN(x, min_x);
						min_y = MIN(y, min_y);
						max_x = MAX(x, max_x);
						max_y = MAX(y, max_y);
					}
				}
			}
		}
	}

	DetectionDiff *img_diff = malloc(sizeof(DetectionDiff));
	img_diff->first_pass = first_pass;
	img_diff->second_pass = second_pass;
	img_diff->min_x = min_x;
	img_diff->max_x = max_x;
	img_diff->min_y = min_y;
	img_diff->max_y = max_y;
	return img_diff;
}


void motion_detect_mark_frame(BMP *bmp, DetectionDiff *diff)
{
	int x, y;
	for (x=diff->min_x; x<diff->max_x; ++x) {
		BMP_put(bmp, x, diff->min_y, &framecolor);
		BMP_put(bmp, x, diff->max_y, &framecolor);
	}
	for (y=diff->min_y; y<diff->max_y; ++y) {
		BMP_put(bmp, diff->min_x, y, &framecolor);
		BMP_put(bmp, diff->max_x, y, &framecolor);
	}
}
