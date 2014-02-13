#include "logic.h"

#include "audio.h"
#include "bmp.h"
#include "camera.h"
#include "motor.h"
#include "motion_detect.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int f_logic = 0;

BMP *pic1 = 0;
BMP *pic2 = 0;

void logic_loop(char* audiofile)
{
	printf("f_logic = %d\n", f_logic);
	if (f_logic){
		if (audiofile != NULL)
			if (!is_playing())
				if (play_sound(audiofile) == 0)
					printf("Started playing: %s\n", audiofile);

		printf("logic is enabled\n");
		int width = 640;
		int height = 480;

		if (!pic1){
			pic1 = malloc(sizeof(BMP));
			memset(pic1, 0, sizeof(BMP));
			BMP_init(pic1, width, height);
			take_pic(pic1);
		}
		else if (!pic2){
			pic2 = pic1;
			pic1 = malloc(sizeof(BMP));
			memset(pic1, 0, sizeof(BMP));
			BMP_init(pic1, width, height);
			take_pic(pic1);
		}
		else{
			BMP* bmp = pic2;
			pic2 = pic1;
			take_pic(bmp);
			pic1 = bmp;
		}
		
		if (pic1 && pic2){
			DetectionDiff* diff = motion_detect(pic1, pic2, 50, 5);

			float angles[2];
			if (calc_rotation(diff, angles)){
				printf("detected motion, horizontal rotation = %f radians\n", angles[0]);
				printf("detected motion, vertical rotation = %f radians\n", angles[1]);
				// set both pic pointers to NULL
				pic1 = NULL;
				pic2 = NULL;
				//rotate_x(angles[0]);
				//rotate_y(angles[1]);
			}

			BMP_free(diff->first_pass);
			BMP_free(diff->second_pass);
			free(diff);
		}
	}
	else{
		printf("logic is disabled\n");
		sleep(1);
	}
}

void enable_logic()
{
	f_logic = 1;
}

void disable_logic()
{
	f_logic = 0;
}
