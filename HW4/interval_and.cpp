#include"utils.h"

interval interval::operator&(const interval &other) {
  int lowest = interval::MIN, highest = interval::MAX;
  int knownZerosThis, knownZerosOther;
  int knownOnesThis, knownOnesOther;
  int resultKnownOnes, resultKnownZeros;

  knownBits(*this, &knownOnesThis, &knownZerosThis);
  knownBits(other, &knownOnesOther, &knownZerosOther);
  resultKnownOnes = knownOnesThis & knownOnesOther; 
  resultKnownZeros = knownZerosThis | knownZerosOther;
  
  //resultknownOnes has 1 set only if all the elements in the 
  //result interval has 1 set in that position. So its save to 
  // always OR resultKnownOnes with the lowest
  //But if the intervals were all negatives, then we have 
  //to or the result with interval::MIN, which will be handled
  //if the result is all negatives in the subsequent code
  lowest = resultKnownOnes;


  // Both of the intervals are positive, since lo >= 0
  if ((lo | other.lo) >= 0) {
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
  } 

  //Both of the intervals are negatives, since both of the high < 0
  else if ((hi & other.hi) < 0) {
    if (hi > other.hi) {
      lowest |= (interval::MIN);

      //Do the highest as we did for the positive case
      highest = ((other.hi | (knownZerosOther & SIGN_MASK)) & knownZerosThis);
      highest = (highest > other.hi) ? (other.hi & resultKnownZeros) : highest;
    } else {
      highest = ((hi | (knownZerosThis & SIGN_MASK)) & knownZerosOther);
      highest = (highest > hi) ? (hi & resultKnownZeros) : highest;
    }
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
    // result will always be positive. If the & of both the low is -ve, then the result will also be a mixed interval 
    if ((lo & other.lo) < 0) {
      lowest |= (interval::MIN);
    } 
  }

  return interval(lowest, highest);
}
