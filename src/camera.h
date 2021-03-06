#ifndef CAMERA_H_
#define CAMERA_H_

#include "bmp.h"
#include "motion_detect.h"

/**
 * Takes a picture using raspistill.
 * 
 * @param bmp image where to save the taken image.
 */
int take_pic(BMP *bmp);

/**
 * Calculates rotation angles from the center of motion difference.
 *
 * @param diff DetectionDiff to be used in calculation.
 * @param angles two dimension float array that is filled with the results.
 * Index 0 will contain horizontal angle and index 1 will contain vertical angle.
 *
 * @return 1 if rotation angles are greater than 0, otherwise returns 0.
 */
int calc_rotation(DetectionDiff *diff, float angles[2]);

#endif // CAMERA_H_
