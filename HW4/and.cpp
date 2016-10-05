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

inline interval andOper(const interval &first, const interval &other) {
  int highest;
  int knownZerosThis, knownZerosOther;
  int knownOnesThis, knownOnesOther;
  int resultKnownOnes, resultKnownZeros;

  knownBits(first, &knownOnesThis, &knownZerosThis);
  knownBits(other, &knownOnesOther, &knownZerosOther);
  resultKnownOnes = knownOnesThis & knownOnesOther; 
  resultKnownZeros = knownZerosThis | knownZerosOther;
  
  //The intervals reaching this function will either be
  //both +ve, or both -ve
  //So If one of them is +31 or -1, then the result will 
  //be the other interval
  if((knownOnesThis & SIGN_MASK) == interval::MAX) {
    return other; 
  } else if ((knownOnesOther & SIGN_MASK) == interval::MAX) {
    return first;
  }

  //For all (+ve, +ve intervals) and (-ve, -ve intervals), 
  //We can safely say, high <= max(hi, other.hi) because 
  //this is & operation, so the leading/trailing zeros 
  //will be lost 
  
  if (first.hi > other.hi) {
    // Get the min high, then 'OR' with the knownZeros of that interval
    // Now the high will have all the possibles ones present in the interval
    // AND the result with the knownZeros of the second interval
    //Sometimes 'OR' with the knownZeros bloats the high with values outside the
    //range of the given interval. So just check that with the below case.
    //If its bloated then replace the result with (min hi & resultKnownZeros) 
    highest = ((other.hi | (knownZerosOther )) & knownZerosThis);
    highest = (highest > other.hi) ? (other.hi & resultKnownZeros) : highest;
  } else {
    highest = ((first.hi | (knownZerosThis )) & knownZerosOther);
    highest = (highest > first.hi) ? (first.hi & resultKnownZeros) : highest;
  }


  //resultknownOnes has 1 set only if all the elements in the 
  //result interval has 1 set in that position. So its save to 
  //set lowest to resultKnownOnes
  return interval(resultKnownOnes, highest);

}

inline interval andOperWrapper(const interval &first, const interval &other) {
  // One of them is a +ve interval and one is -ve 
  //If so, change the negative interval to +ve interval and 
  //call & operator again
  //Since we know these -ve number will loose their
  //sign while doing &, we can move this interval to the 
  //positive boundary and get more precise LOW bound
  if ((other.hi < 0) && (first.lo >= 0)) {
    return andOper(first, interval((other.lo & SIGN_MASK), 
                                   (other.hi & SIGN_MASK)));
  }
  else if ((first.hi < 0) && (other.lo >= 0)) {
    return andOper(interval((first.lo & SIGN_MASK), 
                            (first.hi & SIGN_MASK)), 
                   other);
  }

  return andOper(first, other);
}

interval interval::operator&(const interval &other) {

  // Working with mixed intervals looses precision
  // So split them into +ve and -ve. 
  // Apply & for each pair and then join the intervals
  if (isMixed(*this)) {
    if (isMixed(other)) {
    interval this1 = interval(lo, -1);
    interval this2 = interval(0, hi);
    interval other1 = interval(other.lo, -1);
    interval other2 = interval(0, other.hi);
    return joinIntervals(andOperWrapper(this1,  other1), 
            joinIntervals(andOperWrapper(this1, other2),
             joinIntervals(andOperWrapper(this2, other1), 
                            andOperWrapper(this2, other2))));
    }
    else {
      interval this1 = interval(lo, -1);
      interval this2 = interval(0, hi);
      return joinIntervals(andOperWrapper(this1, other), 
                            andOperWrapper(this2, other));
    }
  } 
  else if (isMixed(other)) {
    interval other1 = interval(other.lo, -1);
    interval other2 = interval(0, other.hi);
      return joinIntervals(andOperWrapper(*this, other1), 
                            andOperWrapper(*this, other2));
  }

  return andOperWrapper(*this, other);
}
