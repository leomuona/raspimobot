
#include "audio.h"
#include "camera.h"
#include "motor.h"
#include "remote.h"
#include "logic.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	srand(time(NULL));

	remote_init();
	
	enable_logic();

	init_motors();
	init_samples("samples");

	while(1){
		logic_loop();
	}

	return 0;
}
