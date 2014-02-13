
#include "audio.h"
#include "camera.h"
#include "motor.h"
#include "remote.h"
#include "logic.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	remote_init();
	
	enable_logic();

	// uncomment this in raspi to test motors
	//init_motors();

	while(1){
		logic_loop(argc > 1 ? argv[1] : 0);
	}

	while (is_playing()) {
		usleep(10000);
	}

	return 0;
}
