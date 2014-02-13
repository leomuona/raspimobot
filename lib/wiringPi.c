#include "wiringPi.h"

#include <stdio.h>

extern int wiringPiSetup (void)
{
	printf("INFO: STUB wiringPi.h: Called wiringPiSetup()\n");
}
extern void pinMode (int pin, int mode)
{
	printf("INFO: STUB wiringPi.h: Called pinMode() for pin %d with value %d\n", pin, mode);
}
extern void digitalWrite (int pin, int value)
{
	printf("INFO: STUB wiringPi.h: Called digitalWrite for pin %d with value %d\n", pin, value);
}

