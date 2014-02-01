#ifndef CAMERA_H_
#define CAMERA_H_

#include "bmp.h"

/**
 * Takes a picture using raspistill.
 * 
 * @param bmp image where to save the taken image.
 */
int take_pic(BMP *bmp);

#endif // CAMERA_H_
