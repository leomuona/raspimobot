#include "logic.h"

#include "audio.h"
#include "bmp.h"
#include "camera.h"
#include "motor.h"
#include "motion_detect.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

int f_logic = 0;

int itid = 0;

int take_both = 1;

BMP *pic1 = NULL;
BMP *pic2 = NULL;

void logic_loop()
{
	printf("f_logic = %d\n", f_logic);

	if (f_logic){

		struct timeval start;
		gettimeofday(&start, 0);

		printf("logic is enabled\n");
		int width = 640;
		int height = 480;

		if (!pic1){
			pic1 = malloc(sizeof(BMP));
			memset(pic1, 0, sizeof(BMP));
			BMP_init(pic1, width, height);
		}
		if (!pic2){
			pic2 = malloc(sizeof(BMP));
			memset(pic2, 0, sizeof(BMP));
			BMP_init(pic2, width, height);
		}

		if (take_both) {
			take_pic(pic1);
			take_pic(pic2);
			take_both = 0;
		} else {
			BMP* bmp = pic2;
			pic2 = pic1;
			take_pic(bmp);
			pic1 = bmp;
		}

		DetectionDiff* diff = motion_detect(pic1, pic2, 50, 5);
		float angles[2];

		if (calc_rotation(diff, angles)) {
			struct timeval end;
			gettimeofday(&end, 0);
			long tdiff = end.tv_sec*1e3 + end.tv_usec/1e3 - start.tv_sec*1e3 - start.tv_usec/1e3;
			printf("cycle length in milliseconds: %ld\n", tdiff);
/*
			motion_detect_mark_frame(pic1, diff);
			motion_detect_mark_frame(pic2, diff);
			motion_detect_mark_frame(diff->first_pass, diff);
			motion_detect_mark_frame(diff->second_pass, diff);

			char name1[256] = {0};
			char name2[256] = {0};
			char name3[256] = {0};
			char name4[256] = {0};

			snprintf(name1, 256, "%d_pic1.bmp", itid);
			snprintf(name2, 256, "%d_pic2.bmp", itid);
			snprintf(name3, 256, "%d_first.bmp", itid);
			snprintf(name4, 256, "%d_second.bmp", itid);

			++itid;

			BMP_write(pic1, name1);
			BMP_write(pic2, name2);
			BMP_write(diff->first_pass, name3);
			BMP_write(diff->second_pass, name4);
*/
			printf("detected motion, horizontal rotation = %f radians\n", angles[0]);
			printf("detected motion, vertical rotation = %f radians\n", angles[1]);

			if (!is_playing()) {
				play_random_sample();
			}

			if (is_playing()) {
				int slept = 0;
				int max_sleep = 3000;
				while (slept < max_sleep && is_playing()) {
					delayms(100);
					slept += 100;
				}

				if (is_playing()) {
					kill(0, SIGINT);
				}
			}

			rotate_x(angles[0]);
			take_both = 1;
		}
		else{
			struct timeval end;
			gettimeofday(&end, 0);
			long tdiff = end.tv_sec*1e3 + end.tv_usec/1e3 - start.tv_sec*1e3 - start.tv_usec/1e3;
			printf("cycle length in milliseconds: %ld\n", tdiff);
		}

		BMP_free(diff->first_pass);
		BMP_free(diff->second_pass);
		free(diff);
	} else {
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
