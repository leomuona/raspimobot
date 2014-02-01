#ifndef MOTION_DETECT_H
#define MOTION_DETECT_H

#include "bmp.h"

struct DetectionDiff
{
	// for detection frame
	int min_x, min_y;
	int max_x, max_y;

	BMP *first_pass;
	BMP *second_pass;
};
typedef struct DetectionDiff DetectionDiff;

/**
 * Try to detect motion between picture a and b using frame diffence method.
 *
 * http://codeding.com/articles/motion-detection-algorithm
 *
 * @param a
 * @param b
 * @param difference_threshold
 * @param noise_filter_size
 *
 * @return DetectionDiff images
 */
DetectionDiff *motion_detect(BMP *a, BMP *b, int difference_threshold, int noise_filter_size);

/**
 * Mark the motion frame into given bmp image.
 *
 * @param image to mark
 * @param motion detection diff
 */
void motion_detect_mark_frame(BMP *bmp, DetectionDiff *diff);

#endif // MOTION_DETECT_H
