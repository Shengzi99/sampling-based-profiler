#include<sys/time.h>
#define TV2SEC(TV) ((double)(TV).tv_sec + (TV).tv_usec/1000000.0)

timeval operator+(const timeval &a, const timeval &b);
timeval operator-(const timeval &a, const timeval &b);