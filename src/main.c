
#include "audio.h"
#include "camera.h"
#include "motor.h"

#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	printf("Hello world\n");

	take_pic();

	if (argc >= 2 && play_sound(argv[1]) == 0) {
		while (is_playing()) {
			sleep(10);
			printf("Still playing...\n");
		}
	}

	move_x();
	move_y();

	return 0;
}
