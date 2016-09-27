#include"utils.h"

interval interval::operator&(const interval &other) {
  int lowest = interval::MIN, highest = interval::MAX;
  int knownZerosThis, knownZerosOther;
  int resultKnownOnes, resultKnownZeros;
  knownZerosThis = knownZeros(*this);
  knownZerosOther = knownZeros(other);
  resultKnownOnes = knownOnes(*this) & knownOnes(other);
  resultKnownZeros = knownZerosThis | knownZerosOther;
  
  //resultknownOnes has 1 set only if all the elements in the 
  //result interval has 1 set in that position. So its save to 
  // always OR resultKnownOnes with the lowest
  //But if the intervals were all negatives, then we have 
  //to or the result with interval::MIN, which will be handled
  //if the result is all negatives in the subsequent code
  lowest = resultKnownOnes;

  //For all (+ve, +ve intervals) and //(-ve, -ve intervals), 
  //We can safely say, high <= max(hi, other.hi) because 
  //this is and operation, so all the value can never increase.
  //At best it can stay at the max
  //IF both of the intervals belong to different signs, then the 
  //below statement is not valid. That case will be handled later 
  //which will overwrite the high
  
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


  //if interval is just a constant, then and both of them and output
  if ((lo == hi) && (other.lo == other.hi)) {
    lowest = lo & other.lo;
    highest = lowest;
  }

  //If both of the highest is negative, then the result will be all
  //negatives with the highest of the result smallest of both of them
  else if ((hi < 0) && (other.hi < 0)) {
    lowest |= (interval::MIN);
  }

  // This is the mixed interval
  // So set the highest to the highest of both the intervals
  // since this is a mixed interval and this could have a -ve number
  // & highest == highest
  else {
    // Get the max high, then 'OR' with the knownZeros of that interval
    // Now the high will have all the possibles ones present in the interval
    // AND the result with the knownZeros of the second interval
    //Sometimes 'OR' with the knownZeros bloats the high with values outside the
    //range of the given interval. So just check that with the below case.
    //If its bloated then replace the result with (max hi & resultKnownZeros) 
    if (hi > other.hi) {
      highest = ((hi | (knownZerosThis & SIGN_MASK)) & knownZerosOther);
      highest = (highest > hi) ? (hi & resultKnownZeros) : highest;
    } else {
      highest = ((other.hi | (knownZerosOther & SIGN_MASK)) & knownZerosThis);
      highest = (highest > other.hi) ? (other.hi & resultKnownZeros) : highest;
    }
    // If either one of the interval's low has a positive value, then we can say
    // result will always be positive. Not of it might be negative
    if (!((lo & other.lo) >= 0)) {
      lowest |= (interval::MIN);
    } 
  }

  return interval(lowest, highest);
}
