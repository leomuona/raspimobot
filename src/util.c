#include "util.h"

#include <time.h>

void delay(unsigned int howLong)
{
	struct timespec sleeper, dummy;
	sleeper.tv_sec  = (time_t)(howLong / 1000) ;
	sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

	nanosleep (&sleeper, &dummy) ;
}
