#include<stdio.h>
#include <assert.h>
#include<iostream>
using namespace std;

const bool DEBUG = false;
const int LOW = -16, HIGH = 15, RANGE = 32;
bool is_value_valid(int a);
bool get_valid_value(int *a);

class interval {
  // 5-bit signed intervals: valid values for lo and hi are -16..15 and it is
  // required that hi >= lo. the bounds are inclusive.
  int lo, hi;

public:
  interval(int, int);
	friend ostream& operator<< (ostream& out, interval const& obj); 
  friend bool operator== (const interval& l, const interval& r);
  friend bool operator<= (const interval& l, const interval& r);
  int getLow();
  int getHigh();

  // these functions must trigger an assertion violation when presented with any
  // invalid interval, and must never return an invalid interval

  // requirement: be sound and fully precise
  // the second argument is to be subtracted from the first
  static interval subtract(interval, interval);

  // requirement: be sound and don't always return top
  static interval bitwise_and(interval, interval);
};

interval::interval(int a, int b) {
  if (!is_value_valid(a)) {
    cout << "Value " << a << "not in range " << LOW << " - " << HIGH << endl;
    assert(false);
  }
  if (!is_value_valid(b)) {
    cout << "Value " << a << "not in range " << LOW << " - " << HIGH << endl;
    assert(false);
  }
  if (a > b) {
	  cout << "Low: " << a << " > High: " << b << endl;	
		assert(false);
	}
  lo = a;
  hi = b;
}

int interval::getLow() {
  return lo;
}

int interval::getHigh() {
  return hi;
}

std::ostream& operator<< (std::ostream & out, interval const& data) {
    out << "(" << data.lo << ", "<< data.hi << ")";
    return out ;
}

bool operator== (const interval& l, const interval& r) {
  if ((l.lo == r.lo) && (l.hi == r.hi)) {
    return true;
  } 
  return false;
}

bool operator<= (const interval& l, const interval& r) {
  if (((l.lo >= r.lo) && (l.lo <= r.hi)) && 
      ((l.hi >= r.lo) && (l.hi <= r.hi))) {
    return true;
  }
  return false;
}
// function which checks if the value is within the range. 
// returns false on failure else true
bool is_value_valid(int a) {
  if ((a >= LOW) && (a <= HIGH)) 
    return true;
  return false;
} 

// function which checks if the value is in range, else rotates the value.
// If there was a rotation returns true, else false
bool get_valid_value(int *a) {
  int b = *a;
  if (*a > HIGH) {
    *a = LOW + (*a % (HIGH + 1));
    if (DEBUG) {
      cout << "Value " << b << " rotated to " << *a << endl;
    }
    return true;
  } else if (*a < LOW) {
    *a = HIGH + (*a % (LOW - 1));
    if (DEBUG) {
      cout << "Value " << b << " rotated to " << *a << endl;
    }
    return true;
  }
  if (DEBUG) {
    cout << "Value " << *a << " not rotated" << endl;
  }
  return false;
}

interval interval::subtract(interval first, interval second) {
  int lowest, highest;
  bool lowRotated = false, highRotated = false;
  lowest = first.lo - second.hi;
  highest = first.hi - second.lo; 
  if (DEBUG) {
    cout << "Low = " << lowest << " before rotation" << endl;
    cout << "High = " << highest << " before rotation" << endl;
  }
  lowRotated = get_valid_value(&lowest);
  highRotated = get_valid_value(&highest);
  if (DEBUG) {
    cout << "Low = " << lowest << " after rotation" << endl;
    cout << "High = " << highest << " after rotation" << endl;
  }


  // Means one of the values(low/high) is rotated. So we have to have the
  // full set
  if (lowRotated != highRotated) {
    return interval(LOW, HIGH);
  } else { 
    // Means both the values(low, high) have rotated. Room to optimize and 
    // give a smaller set
    
    // After rotation low < high, then this low, high is the 
    // new precise interval
    if (lowest <= highest) {
      return interval(lowest, highest);
    } else {
      // After rotation low > high, then it means the new interval is wrapped.
      // so result is full interval
      return interval(LOW, HIGH);
    }
  }
}


interval interval::bitwise_and(interval first, interval second) {
  int lowest = LOW, highest = HIGH;
  
  //We can safely say, high <= max(first.hi, second.hi) because 
  //this is an and operatiom, so all the value can never increase.
  //At best it can stay at the max
  highest = (first.hi > second.hi) ? first.hi : second.hi;


  // If either one of the interval's low has a positive value, then we can say
  // result will always be positive
  if ((first.lo >= 0) || (second.lo >= 0)) {
    lowest = 0;
  }

  // If both the intervals are positive, then the highest of the new interval
  // will be the smallest of both, lowest will be 0.
  // Highest will be the smallest of the both, the numbers aftert the 
  // smallest high will be brought down to value <= smallest due to and 
  if ((first.lo >= 0) && (second.lo >= 0)) {
    lowest = 0;
    highest = (first.hi > second.hi) ? second.hi : first.hi; 
  }

  //if an interval is just -16, then high has to be 0. Low can be -16 
  if (((first.lo == -16) && (first.hi == -16)) ||
      ((second.lo == -16) && (second.hi == -16))) {
    highest = 0;
  }

  //If both of the highest is negative, then the result will be all
  //negatives with the highest of the result smallest of both of them
  if ((first.hi < 0) && (second.hi < 0)) {
    lowest = LOW;
    highest = (first.hi > second.hi) ? second.hi : first.hi;
  }

  //if an interval is just 0's, then the result is 0
  if (((first.lo == 0) && (first.hi == 0)) ||
      ((second.lo == 0) && (second.hi == 0))) {
    lowest = 0;
    highest = 0;
  }

  //if interval is just one digit, then and both of them and output
  if ((first.lo == first.hi) && (second.lo == second.hi)) {
    lowest = first.lo & second.lo;
    highest = lowest;
  }
  return interval(lowest, highest);
}


