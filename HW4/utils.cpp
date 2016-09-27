#include"utils.h"

// function which checks if the value is in range, else rotates the value.
// If there was a rotation returns true, else false
inline bool get_valid_value(int *a) {
  int b = *a;
  if (*a > interval::MAX) {
    *a = interval::MIN + (*a % (interval::MAX + 1));
    return true;
  } else if (*a < interval::MIN) {
    *a = interval::MAX + (*a % (interval::MIN - 1));
    return true;
  }
  return false;
}

//The ones present in the return values, 
//are present in all the values in the interval
//The zeros present in ther result can be ones or zeros
int knownOnes(interval data) {
  int ones = -1;
  for (int i = data.lo; i<= data.hi; i++) {
    ones &= i;
  }
  return ones;
}

//The zeros present in the return value,
//are present in all the values of the interval
//Thus this zeros will be propagated to the result also
//The ones present in ther result can be ones or zeros
int knownZeros(interval data) {
  int zeros = 0;
  for (int i = data.lo; i<= data.hi; i++) {
    zeros |= i;
  }
  return zeros;
}


