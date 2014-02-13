#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

int init_gpio()
{
	if (wiringPiSetup() == -1) {
		printf("Error: wiringPiSetup() returned an error code.\n");
		return -1;
	}
	return 0;
}

int is_io_pin(int pin)
{
	switch (pin) {
	case 0 ... 6: // wiringPi pins are 0 .. 6
		return 1; // is i/o pin
	default:
		return 0; // not allowed pin
	}
}

int enable_output(int pin)
{
	if (!is_io_pin(pin)) {
		printf("Error: pin %d is not allowed i/o pin\n", pin);
		return -1;
	}
	pinMode(pin, OUTPUT);
	return 0;
}

int set_high(int pin)
{
	if (!is_io_pin(pin)) {
                printf("Error: pin %d is not allowed i/o pin\n", pin);
                return -1;
        }
	digitalWrite(pin, 1); // on/high
	return 0;
}

int set_low(int pin)
{
	if (!is_io_pin(pin)) {
                printf("Error: pin %d is not allowed i/o pin\n", pin);
                return -1;
        }
	digitalWrite(pin, 0); // off/low
        return 0;
}

