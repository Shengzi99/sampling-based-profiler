#include<stdint.h>
#include "utils.h"


timeval operator+(const timeval &a, const timeval &b){
    timeval res;
    int64_t usec = a.tv_usec + b.tv_usec;
    res.tv_sec = a.tv_sec + b.tv_sec + usec/1000000;
    res.tv_usec = usec%1000000;
    return res;
}
timeval operator-(const timeval &a, const timeval &b){
    timeval res;
    int64_t usec = a.tv_usec - b.tv_usec;
    res.tv_sec = a.tv_sec - b.tv_sec + (usec/1000000);
    res.tv_usec = usec % 1000000;
    if(res.tv_usec<0){
        res.tv_sec-=1;
        res.tv_usec+=1000000;
    }
    return res;
}
