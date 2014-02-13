#include "motor.h"

#include "gpio.h"
#include "util.h"
#include <time.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265f
#endif

// angular velocities of motors
#define X_MOTOR_ANG_V (M_PI/2)
#define Y_MOTOR_ANG_V (M_PI/2)

struct motor {
	int a;
	int b;
};

struct motor x_motor = {0, 1}; // GPIO0 = hardware 11 & GPIO1 hardware 12  
struct motor y_motor = {3, 4}; // GPIO3 = hardware 15 & GPIO4 hardware 16

int init_motors()
{
	int result = init_gpio();
	if (result < 0) {
		printf("Initializing GPIO failed with code %d\n", result);
		return -1;
	}
	// enable output pins
	result = 0;
	if (enable_output(x_motor.a) < 0) result = -1;
	if (enable_output(x_motor.b) < 0) result = -1;
	if (enable_output(y_motor.a) < 0) result = -1;
	if (enable_output(y_motor.b) < 0) result = -1;
	if (result < 0) {
		printf("Enabling motor output pins failed.\n");
		return -1;
	}
	// set pins to low by default
	set_low(x_motor.a);
	set_low(x_motor.b);
	set_low(y_motor.a);
	set_low(y_motor.b);

	return 0;
}

void rotate_x(float rad)
{
	int turn_right = 1;
	if (rad < 0) {
		rad = -rad;
		turn_right = 0;
	}
	float time = rad / X_MOTOR_ANG_V;
	if (turn_right) {
		set_low(x_motor.a);
		set_high(x_motor.b);
		delay((int) (time * 1000)); // msec
		set_low(x_motor.b);
	} else {
		set_low(x_motor.b);
		set_high(x_motor.a);
		delay((int) (time * 1000)); // msec
		set_low(x_motor.a);
	}
}

void rotate_y(float rad)
{
	int turn_up = 1;
	if (rad < 0) {
		rad = -rad;
		turn_up = 0;
	}
	float time = rad / Y_MOTOR_ANG_V;
	if (turn_up) {
		set_low(y_motor.a);
		set_high(y_motor.b);
		delay((int) (time * 1000)); // msec
		set_low(y_motor.b);
	} else {
		set_low(y_motor.b);
		set_high(y_motor.a);
		delay((int) (time * 1000)); // msec
		set_low(y_motor.a);
	}
}

