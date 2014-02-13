/*
 * This is a stub library file for wiringPi.h
 */

#ifndef	__WIRING_PI_H__
#define	__WIRING_PI_H__

#define OUTPUT 1

extern int wiringPiSetup (void);
extern void pinMode (int pin, int mode);
extern void digitalWrite (int pin, int value);

#endif // __WIRING_PI_H__

