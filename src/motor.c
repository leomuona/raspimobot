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
#define X_MOTOR_ANG_V 1.337f
#define X_MOTOR_DELTA_MAX (M_PI/8)

// motor radius delta
float x_motor_delta = 0.0f;

struct motor {
	int a;
	int b;
};

struct motor x_motor = {0, 1}; // GPIO0 = hardware 11 & GPIO1 hardware 12  

int init_motors()
{
	// enable output pins
	int result = 0;
	if (enable_output(x_motor.a) < 0) result = -1;
	if (enable_output(x_motor.b) < 0) result = -1;
	if (result < 0) {
		printf("Enabling motor output pins failed.\n");
		return -1;
	}
	// set pins to low by default
	set_low(x_motor.a);
	set_low(x_motor.b);

	return 0;
}

float safe_rad(float rad)
{
	if (x_motor_delta + rad > X_MOTOR_DELTA_MAX) {
		printf("DEBUG safe_rad: changing rad %f -> %f\n",
			rad, (X_MOTOR_DELTA_MAX - x_motor_delta));
		rad = X_MOTOR_DELTA_MAX - x_motor_delta;
	} else if (x_motor_delta + rad < -X_MOTOR_DELTA_MAX) {
		printf("DEBUG safe_rad: changing rad %f -> %f\n",
			rad, -(X_MOTOR_DELTA_MAX + x_motor_delta));
		rad = -(X_MOTOR_DELTA_MAX + x_motor_delta);
	}
	return rad;
}

void rotate_x(float rad)
{
	int turn_left = 0;
	rad = safe_rad(rad);
	if (rad == 0)
		return;
	
	x_motor_delta += rad;
	if (rad < 0) {
		rad = -rad;
		turn_left = 1;
	}
	float time = rad / X_MOTOR_ANG_V;
	
	if (turn_left) {
		printf("DEBUG rotate_x: turning LEFT %f seconds\n", time);
		set_low(x_motor.a);
		set_high(x_motor.b);
		delayms((int) (time * 1000)); // msec
		set_low(x_motor.b);
	} else {
		printf("DEBUG rotate_x: turning RIGHT %f seconds\n", time);
		set_low(x_motor.b);
		set_high(x_motor.a);
		delayms((int) (time * 1000)); // msec
		set_low(x_motor.a);
	}
}

