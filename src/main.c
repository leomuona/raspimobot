
#include "audio.h"
#include "camera.h"
#include "motor.h"
#include "remote.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	printf("Hello world\n");

	int width = 640;
	int height = 480;
	char* pic = malloc(width*height*3);
	memset(pic, 0, width*height*3);
	take_pic(pic, width, height);
	free(pic);

	if (argc >= 2 && play_sound(argv[1]) == 0) {
		while (is_playing()) {
			sleep(10);
			printf("Still playing...\n");
		}
	}

	move_x();
	move_y();

	remote_init();
	while (1){
		sleep(10);
	}

	return 0;
}
