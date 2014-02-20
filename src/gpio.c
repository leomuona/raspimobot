#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>

int is_io_pin(int pin)
{
	switch (pin) {
	case 0 ... 6: // wiringPi lib pins are 0 .. 6
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
	char cmd[255] = {0};
	sprintf(cmd, "gpio mode %d out", pin);
        int rc = system(cmd);
	if (rc != 0) {
		printf("Error: system call returned value %d\n", rc);
		return -1;
	}
	return 0;
}

int set_high(int pin)
{
	if (!is_io_pin(pin)) {
                printf("Error: pin %d is not allowed i/o pin\n", pin);
                return -1;
        }
	char cmd[255] = {0};
	sprintf(cmd, "gpio write %d 1", pin);
        int rc = system(cmd);
	if (rc != 0) {
		printf("Error: system call returned value %d\n", rc);
		return -1;
	}
	return 0;
}

int set_low(int pin)
{
	if (!is_io_pin(pin)) {
                printf("Error: pin %d is not allowed i/o pin\n", pin);
                return -1;
        }
	char cmd[255] = {0};
	sprintf(cmd, "gpio write %d 0", pin);
        int rc = system(cmd);
	if (rc != 0) {
		printf("Error: system call returned value %d\n", rc);
		return -1;
	}
	return 0;
}

