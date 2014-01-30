#ifndef GPIO_H_
#define GPIO_H_

/* This file includes the GPIO related low level code.
   This makes the gpio pin controlling a bit easier. */

/* Mandatory for everything else.
   Returns -1 if error. */
int init_gpio();

// is pin an i/o pin? 1 = true, 0 = false
int is_io_pin(int pin);

/* Enable pin as output.
   Returns -1 if error. */
int enable_output(int pin);

/* Set pin to high.
   Returns -1 if error. */
int set_high(int pin);

/* Set pin to low.
   Returns -1 if error. */
int set_low(int pin);

#endif // GPIO_H_ 
