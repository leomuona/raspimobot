#include "logic.h"

#include "audio.h"
#include "bmp.h"
#include "camera.h"
#include "motor.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int f_logic = 0;

void logic_loop(char* audiofile)
{
	printf("f_logic = %d\n", f_logic);
	if (f_logic){
		printf("logic is enabled\n");
		int width = 640;
		int height = 480;

		BMP *bmp = malloc(sizeof(BMP));
		memset(bmp, 0, sizeof(BMP));
		BMP_init(bmp, width, height);
		take_pic(bmp);
		BMP_write(bmp, "temp.bmp");
		BMP_free(bmp);
		free(bmp);

		if (audiofile != 0 && play_sound(audiofile) == 0) {
			while (is_playing()) {
				sleep(10);
				printf("Still playing...\n");
			}
		}

		rotate_x(1.0f);
		rotate_y(1.0f);
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
