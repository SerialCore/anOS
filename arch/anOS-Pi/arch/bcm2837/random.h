
// address
#define RANDOM              (volatile unsigned int*) 0x3F104000
#define RNG_CTRL            (volatile unsigned int*) 0x3F104000
#define RNG_STATUS          (volatile unsigned int*) 0x3F104004
#define RNG_DATA            (volatile unsigned int*) 0x3F104008
#define RNG_INT_MASK        (volatile unsigned int*) 0x3F104010

void _rand_init();

// Return a random number between [min..max]
unsigned int _rand_get(unsigned int min, unsigned int max);
