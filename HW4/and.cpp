#include"interval.h"

const int SIGN_MASK = 0x1F;
const int SET_SIGN_MASK = 0x20;

#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
static const char LogTable64[64] = 
{ -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
  LT(4), LT(5), LT(5)};

//The ones present in the return values, 
//are present in all the values in the interval
//The zeros present in ther result can be ones or zeros
//
//The zeros present in the return value,
//are present in all the values of the interval
//Thus this zeros will be propagated to the result also
//The ones present in ther result can be ones or zeros
inline void knownBits(interval data, int *ones, int *zeros) {
  //int bits = (int)log2(data.hi - data.lo) + 1;
  int bits = (int)LogTable64[data.hi - data.lo] + 1;

  //Find the difference between hi and lo. Let it be x
  //Now this X numbers will differ in the LSB with
  //max of log2(X)+1 bits. So set those bits to 0
  //and then & with the data.hi & data.lo
  *ones = (data.hi & data.lo) & (-1 << bits);

  //Same logic as above but now | them
  *zeros = (data.hi | data.lo) | ((1 << bits) -1);
}

bool isMixed(interval data) {
  return ((data.lo < 0) && (data.hi >= 0));
}

inline interval joinIntervals(interval a, interval b)
{
  int lo, hi;
  lo = (a.lo < b.lo) ? a.lo : b.lo;
  hi = (a.hi > b.hi) ? a.hi : b.hi;
  return interval(lo, hi);
}

interval interval::operator&(const interval &other) {
  int lowest = interval::MIN, highest = interval::MAX;
  int knownZerosThis, knownZerosOther;
  int knownOnesThis, knownOnesOther;
  int resultKnownOnes, resultKnownZeros;

  // Working with mixed intervals looses precision
  // So split them into +ve and -ve. 
  // Apply & for each pair and then join the intervals
  if (isMixed(*this)) {
    if (isMixed(other)) {
    interval this1 = interval(lo, -1);
    interval this2 = interval(0, hi);
    interval other1 = interval(other.lo, -1);
    interval other2 = interval(0, other.hi);
    return joinIntervals(this1 & other1, 
            joinIntervals(this1 & other2,
             joinIntervals(this2 & other1, this2 & other2)));
    }
    else {
      interval this1 = interval(lo, -1);
      interval this2 = interval(0, hi);
      return joinIntervals(this1 & other, this2 & other);
    }
  } 
  else if (isMixed(other)) {
    interval other1 = interval(other.lo, -1);
    interval other2 = interval(0, other.hi);
      return joinIntervals(*this & other1, *this & other2);
  }

  // One of them is a +ve interval and one is -ve 
  //If so, change the negative interval to +ve interval and 
  //call & operator again
  //Since we know these -ve number will loose their
  //sign while doing &, we can move this interval to the 
  //positive boundary and get more precise LOW bound
  if ((other.hi < 0) && (lo >= 0)) {
    return (*this & interval(other.lo & SIGN_MASK, other.hi & SIGN_MASK));
  }
  else if ((hi < 0) && (other.lo >= 0)) {
    return (interval(lo & SIGN_MASK, hi & SIGN_MASK) & other);
  }

  knownBits(*this, &knownOnesThis, &knownZerosThis);
  knownBits(other, &knownOnesOther, &knownZerosOther);
  resultKnownOnes = knownOnesThis & knownOnesOther; 
  resultKnownZeros = knownZerosThis | knownZerosOther;
  
  //resultknownOnes has 1 set only if all the elements in the 
  //result interval has 1 set in that position. So its save to 
  //set lowest to resultKnownOnes
  lowest = resultKnownOnes;

  //For all (+ve, +ve intervals) and (-ve, -ve intervals), 
  //We can safely say, high <= max(hi, other.hi) because 
  //this is & operation, so the leading/trailing zeros 
  //will be lost 
  
  if (hi > other.hi) {
    // Get the min high, then 'OR' with the knownZeros of that interval
    // Now the high will have all the possibles ones present in the interval
    // AND the result with the knownZeros of the second interval
    //Sometimes 'OR' with the knownZeros bloats the high with values outside the
    //range of the given interval. So just check that with the below case.
    //If its bloated then replace the result with (min hi & resultKnownZeros) 
    highest = ((other.hi | (knownZerosOther & SIGN_MASK)) & knownZerosThis);
    highest = (highest > other.hi) ? (other.hi & resultKnownZeros) : highest;
  } else {
    highest = ((hi | (knownZerosThis & SIGN_MASK)) & knownZerosOther);
    highest = (highest > hi) ? (hi & resultKnownZeros) : highest;
  }

/*
  if((knownOnesThis & SIGN_MASK) == 31) {
    lowest = (knownOnesThis & other.lo) | (other.lo & SET_SIGN_MASK); 
  } else if ((knownOnesOther & SIGN_MASK) == 31) {
    lowest = (knownOnesOther & lo) | (lo & SET_SIGN_MASK);
  } else {
    return interval(lowest, highest);
  }

  if (lowest > highest) {
    lowest = resultKnownOnes;
  }*/
  return interval(resultKnownOnes, highest);
}
