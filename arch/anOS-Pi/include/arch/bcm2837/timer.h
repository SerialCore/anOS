
// address
#define SYSTMR_LO           (volatile unsigned int*) 0x3F003004
#define SYSTMR_HI           (volatile unsigned int*) 0x3F003008

// for 32 bits os, get low 32 bits time
unsigned int _getL_time_stamp();

// for 32 bits os, get high 32 bits time
unsigned int _getH_time_stamp();

// get whole 64 bits time
unsigned long _get_time_stamp();

// delay in microseconds by bcm system timer
void _delay_microseconds(unsigned int delay);

// delay in milliseconds by bcm system timer
void _delay_milliseconds(unsigned int delay);

// delay in n CPU cycles
void _delay_cycles(unsigned int n);