#include "motor.h"

#include "gpio.h"
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

struct motor x_motor = {11, 12}; 
struct motor y_motor = {15, 16};

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
	struct timespec sleep_time;
	sleep_time.tv_sec = (time_t) time;
	sleep_time.tv_nsec = (long) (time * 1000000000);

	if (turn_right) {
		set_low(x_motor.a);
		set_high(x_motor.b);
		nanosleep(&sleep_time, NULL);
		set_low(x_motor.b);
	} else {
		set_low(x_motor.b);
		set_high(x_motor.a);
		nanosleep(&sleep_time, NULL);
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
	struct timespec sleep_time;
	sleep_time.tv_sec = (time_t) time;
	sleep_time.tv_nsec = (long) (time * 1000000000);

	if (turn_up) {
		set_low(y_motor.a);
		set_high(y_motor.b);
		nanosleep(&sleep_time, NULL);
		set_low(y_motor.b);
	} else {
		set_low(y_motor.b);
		set_high(y_motor.a);
		nanosleep(&sleep_time, NULL);
		set_low(y_motor.a);
	}
}

