#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/* Using modified code examples from
   http://elinux.org/RPi_Low-level_peripherals#C_2 */

#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define GPIO_PAGE_SIZE (4*1024)
#define GPIO_BLOCK_SIZE (4*1024)

int gpio_mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7) // sets bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

// setup memory regions to access GPIO
int init_gpio()
{
	/* open /dev/mem */
	if ((gpio_mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
		printf("Error: can't open /dev/mem\n");
		return -1;
	}

	/* mmap GPIO */
	gpio_map = mmap(
		NULL, // any adddress in our space will do
		GPIO_BLOCK_SIZE, // map length
		PROT_READ|PROT_WRITE, // enable reading & writting to mapped memory
		MAP_SHARED, // shared with other processes
		gpio_mem_fd, // file to map
		GPIO_BASE // offset to GPIO peripheral
	);

	close(gpio_mem_fd); // no need to keep gpio_mem_fd open after mmap

	if (gpio_map == MAP_FAILED) {
		printf("mmap error %p\n", gpio_map); // errno also set!
		return -1;
	}

	// always use volatile pointer!
	gpio = (volatile unsigned *)gpio_map;
	return 0;
}

int is_io_pin(int pin)
{
	switch (pin) {
	case 7:
	case 11 ... 13:
	case 15 ... 16:
	case 18:
	case 22:
		return 1; // is i/o pin
	default:
		return 0; // not allowed pin
	}
}

int enable_output(int pin)
{
	if (is_io_pin(pin)) {
		printf("Error: pin %d is not allowed i/o pin\n", pin);
		return -1;
	}

	INP_GPIO(pin); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(pin);

	return 0;
}

int set_high(int pin)
{
	if (is_io_pin(pin)) {
                printf("Error: pin %d is not allowed i/o pin\n", pin);
                return -1;
        }
	GPIO_SET = 1<<pin;
	return 0;
}

int set_low(int pin)
{
	if (is_io_pin(pin)) {
                printf("Error: pin %d is not allowed i/o pin\n", pin);
                return -1;
        }
        GPIO_CLR = 1<<pin;
        return 0;
}

