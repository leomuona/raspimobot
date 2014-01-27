
#include "audio.h"
#include "camera.h"
#include "motor.h"

#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("Hello world\n");

	take_pic();

	if (play_sound("01.wav") == 0) {
		while (is_playing()) {
			sleep(10);
			printf("Still playing...\n");
		}
	}

	move_x();
	move_y();

	return 0;
}
