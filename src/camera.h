#ifndef CAMERA_H_
#define CAMERA_H_

/**
 * Takes a picture using raspicam.
 * 
 * Parameters: 
 * pic is an char array with size of width*height*3. 
 * The picture date is written there.
 * 
 * width is the width of the picture in pixels.
 *
 * height is the height of the picture in pixels.
 */
int take_pic(char* pic, int width, int height);

#endif
