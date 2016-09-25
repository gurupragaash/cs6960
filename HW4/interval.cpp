#include"interval.h"
#include<iostream>
using namespace std;

const bool DEBUG = false;
const int MASK = 0x1F;

// function which checks if the value is in range, else rotates the value.
// If there was a rotation returns true, else false
bool get_valid_value(int *a) {
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

void get_info(interval data, bool &isPositive, bool &isNegative, bool &isConstant) {
  if (data.lo == data.hi) {
    isConstant = true;
  } 
  
  if (data.lo >= 0) {
    isPositive = true;
    return;
  }

  if (data.hi < 0) {
    isNegative = true;
  }
}

bool is_mixed(interval first, interval second) {
  if ((first.hi < 0) && (second.lo >= 0)) 
    return true;
  if ((second.hi < 0) && (first.lo >= 0)) 
    return true;
  if ((first.lo < 0) && (first.hi >= 0))
    return true;
  if ((second.lo < 0) && (second.hi >= 0))
    return true;
  return false;
}

int knownOnes(interval data) {
  int ones = -1;
  for (int i = data.lo; i<= data.hi; i++) {
    ones &= i;
  }
  return ones;
}

int knownZeros(interval data) {
  int zeros = 0;
  for (int i = data.lo; i<= data.hi; i++) {
    zeros |= i;
  }
  return zeros;
}

/*
interval interval::operator&(const interval &other) {
  int resultKnownOnes, resultKnownZeros;
  resultKnownOnes = knownOnes(*this) & knownOnes(other);
  resultKnownZeros = knownZeros(*this) & knownZeros(other);

  cout << "Result known ones, zeros(" << resultKnownOnes << " , " << resultKnownZeros <<")" << endl;
  //cout << "Result ~negated known ones, zeros(" << ~resultKnownOnes << " , " << ~resultKnownZeros <<")" << endl;
  return interval::top();
}*/

interval interval::operator-(const interval &other) {
  int lowest, highest;
  bool lowRotated = false, highRotated = false;
  lowest = lo - other.hi;
  highest = hi - other.lo; 
  lowRotated = get_valid_value(&lowest);
  highRotated = get_valid_value(&highest);


  // Means one of the values(low/high) is rotated. So we have to have the
  // full set or
  // After rotation low > high, then it means the new interval is wrapped.
  // so result is full interval
  if ((lowRotated != highRotated) || (lowest > highest)){
    return interval::top();
  } else { 
    // Means both the values(low, high) have rotated. Room to optimize and 
    // give a smaller set
    
    // After rotation low < high, then this low, high is the 
    // new precise interval
    return interval(lowest, highest);
  }
}


interval interval::operator&(const interval &other) {
  int lowest = interval::MIN, highest = interval::MAX;
  int resultKnownOnes, resultKnownZeros;
  resultKnownOnes = knownOnes(*this) & knownOnes(other);
  resultKnownZeros = knownZeros(*this) | knownZeros(other);

/*
  if (lo == hi) {
    resultKnownZeros = knownZeros(*this) & knownZeros(other);
  } else if(other.lo == other.hi) {
    resultKnownZeros = knownZeros(*this) & knownZeros(other);
  } else {
    resultKnownZeros = knownZeros(*this) | knownZeros(other);
  }*/
  
  //For all (+ve intervals, +ve intervals) and 
  //(-ve intervals, -ve intervals), 
  //We can safely say, high <= max(hi, other.hi) because 
  //this is an and operatiom, so all the value can never increase.
  //At best it can stay at the max
  //IF both of the intervals belong to different signs, then the 
  //below statement is not valid. That case will be handled below
  //which will overwrite the high
  
  if (hi > other.hi) {
    highest = ((other.hi | (knownZeros(other) & MASK)) & knownZeros(*this));
    highest = (highest > other.hi) ? (other.hi & resultKnownZeros) : highest;
  } else {
    highest = ((hi | (knownZeros(*this) & MASK)) & knownZeros(other));
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
    lowest = (interval::MIN) | resultKnownOnes;
  }

  //This is all positive numbers
  else if ((lo >= 0) && (other.lo >= 0)) {
    //lowest = 0;
    lowest = resultKnownOnes;
  }

  // This is the mixed interval
  // So set the highest to the highest of both the intervals
  // since this is a mixed interval and this could have a -ve number
  // & highest == highest
  else {
    if (hi > other.hi) {
      highest = ((hi | (knownZeros(*this) & MASK)) & knownZeros(other));
      highest = (highest > hi) ? (hi & resultKnownZeros) : highest;
    } else {
      highest = ((other.hi | (knownZeros(other) & MASK)) & knownZeros(*this));
      highest = (highest > other.hi) ? (other.hi & resultKnownZeros) : highest;
    }
    // If either one of the interval's low has a positive value, then we can say
    // result will always be positive
    if ((lo & other.lo) >= 0) {
      //lowest = 0;
      lowest = resultKnownOnes;
    } else {
      lowest = (interval::MIN) | resultKnownOnes;
    } 
  }

/*
  //if an interval is just -16, then high has to be 0. Low can be -16 
  if (((lo == interval::MIN) && (hi == interval::MIN)) ||
      ((other.lo == interval::MIN) && (other.hi == interval::MIN))) {
    highest = 0;
  }*/



  return interval(lowest, highest);
}


/*
int main() {

  cout <<"case 1: bitwise and of all positive integers" << endl;
  interval(1, 15) & interval(0,3);
  cout <<"case 2: bitwise and of all negative integers" << endl;
  interval(-8, -6) & interval(-12, -3);
  cout <<"case 3: bitwise and of negative interval and positive interval" << endl;
  interval(-16, 0) & interval(0, 15);
  cout <<"case 4: bitwise and of mix of intervals" << endl;
  interval(-10, 10) & interval (-16, 3);
  cout <<"case 5: bitwise and full intervals" << endl;
  interval(-16, 15) & interval(-16, 15);
 
  return 1;
}*/
