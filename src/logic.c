#include "logic.h"

#include "camera.h"
#include "motor.h"
#include "audio.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void logic_loop(char* audiofile)
{
	f_loop = 1;

	while (f_loop){
		int width = 640;
		int height = 480;
		char* pic = malloc(width*height*3);
		memset(pic, 0, width*height*3);
		take_pic(pic, width, height);
		free(pic);

		if (audiofile != 0 && play_sound(audiofile) == 0) {
			while (is_playing()) {
				sleep(10);
				printf("Still playing...\n");
			}
		}

		move_x();
		move_y();
	}
}
