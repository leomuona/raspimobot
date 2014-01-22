#include <stdio.h>

#include "audio.h"
#include "camera.h"
#include "motor.h"

int main()
{
	printf("Hello world\n");

	take_pic();

	play_sound();

	move_x();
	move_y();

	return 0;
}
