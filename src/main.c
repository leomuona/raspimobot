
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

	remote_init();

	logic_loop(argc > 1 ? argv[1] : 0);

	return 0;
}
