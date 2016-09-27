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
//
//The zeros present in the return value,
//are present in all the values of the interval
//Thus this zeros will be propagated to the result also
//The ones present in ther result can be ones or zeros
void knownBits(interval data, int *ones, int *zeros) {
  if (data.hi == data.lo) {
    *ones = data.hi;
    *zeros = data.hi;
  } else {
    int bits = (int)log2(data.hi - data.lo) + 1;
    //Find the difference between hi and lo. Let it be x
    //Now this X numbers will differ in the LSB with
    //max of log2(X)+1 bits. So set those bits to 0
    //and then & with the data.hi & data.lo
    *ones = (data.hi & data.lo) & (-1 << bits);

    //Same logic as above but now | them
    *zeros = (data.hi | data.lo) | ((1 << bits) -1);
  }
}
