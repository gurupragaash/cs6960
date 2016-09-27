#ifndef _UTILS_

#define _UTILS_
#include"interval.h"

const int SIGN_MASK = 0x1F;
inline bool get_valid_value(int *a);
int knownOnes(interval data);
int knownZeros(interval data); 

#endif
