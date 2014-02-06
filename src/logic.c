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
			DetectionDiff* diff = motion_detect(pic1, pic2, 45, 5);

			BMP_write(pic1, "pic1.bmp");
			BMP_write(pic2, "pic2.bmp");
			BMP_write(diff->first_pass, "first_pass.bmp");
			BMP_write(diff->second_pass, "second_pass.bmp");

			BMP_free(diff->first_pass);
			BMP_free(diff->second_pass);
			free(diff);
		}

		//rotate_x(1.0f);
		//rotate_y(1.0f);
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
