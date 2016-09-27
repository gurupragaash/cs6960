#include <assert.h>
#include"interval.h"
using namespace std;

interval abstract(int *values, int count);
void concreate(interval a, int *values, int *count);
int sub(int a, int b);
interval exhaustive_oper(interval a, interval b, int oper);
void exhaustive_sub();
void exhaustive_and();

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

interval abstract(int *values, int count) {
  int i, low = interval::MAX, high = interval::MIN;
  if (count == 0) {
    cout << "Count is 0";
    assert(false);
  }
  for (i = 0; i < count; i++) {
    low = (low > values[i]) ? values[i] : low;
    high = (high < values[i]) ? values[i] : high;
  }

  return interval(low, high);
}

void concreate(interval a, int *values, int *count) {
  int i;
  *count = 0;
  for (i = a.lo; i <= a.hi; i++) {
    values[*count] = i;
    *count += 1;
  }
}

int sub(int a, int b) {
  int c = a - b;
  get_valid_value(&c);
  return c;
}

interval exhaustive_oper(interval a, interval b, int oper) {
  int values1[interval::RANGE], values2[interval::RANGE];
  int count1 = 0, count2 = 0;
  int values3[interval::RANGE*interval::RANGE], count3 = 0;
  int x,y; 
  concreate(a, values1, &count1);
  concreate(b, values2, &count2);

  for(x = 0; x < count1; x++) {
    for (y = 0; y < count2; y++) {
      if (oper == 1) {
        values3[count3++] = sub(values1[x], values2[y]);
      } else {
        values3[count3++] = values1[x] & values2[y];
      }
    }
  }

  return abstract(values3, count3);
}

void exhaustive_sub() {
  int lo1, lo2, hi1, hi2;
  for( lo1 = interval::MIN; lo1 <= interval::MAX; lo1++) {
    for (lo2 = interval::MIN; lo2 <= interval::MAX; lo2++) {
      for (hi2 = lo2; hi2 <= interval::MAX; hi2++) {
        for (hi1 = lo1; hi1 <= interval::MAX; hi1++) {
          interval a = exhaustive_oper(interval(lo1, hi1), interval(lo2, hi2), 1);
          interval b = interval(lo1, hi1) - interval(lo2, hi2);
          if (a == b) {
            continue;
          }
          cout << "Subtraction" <<  a << "!=" << b;
          assert(false);
        }
      }
    } 
  }
  cout << "Subtraction Success" << endl;
}

void knownBitsExhaustive (interval data, int *ones, int *zeros) {
  int count;
  *ones = data.lo;
  *zeros = data.hi;
  for (count = data.lo; count <= data.hi; count++) {
    *ones &= count;
    *zeros |= count;
  }
}

/*
void exhaustive_knownBits() {
  int lo1, hi1;
  int ones, zeros, ones1, zeros1;
  for( lo1 = interval::MIN; lo1 <= interval::MAX; lo1++) {
    for (hi1 = lo1; hi1 <= interval::MAX; hi1++) {
      knownBits(interval(lo1, hi1), &ones1, &zeros1);
      knownBitsExhaustive(interval(lo1, hi1), &ones, &zeros);
      if ( ones != ones1) {
        cout << "Interval is " << interval(lo1, hi1) << endl;
        cout << "ones = " << ones1 << " Exhaustive is " << ones << endl;
      }
      if (zeros != zeros1) {
        cout << "Interval is " << interval(lo1, hi1) << endl;
        cout << "zeros = " << zeros1 << "exhaustive is " << zeros << endl;
      }
    }
  }
  cout <<"exhaustive known done" << endl;
}*/

void exhaustive_and() {
  int regular_bits = 0, exhaustive_bits = 0, old_bits = 0;
  int lo1, lo2, hi1, hi2;
  int knownZerosThis, knownZerosOther;
  int knownOnesThis, knownOnesOther;
  int resultKnownOnes, resultKnownZeros;
  for( lo1 = interval::MIN; lo1 <= interval::MAX; lo1++) {
    for (lo2 = interval::MIN; lo2 <= interval::MAX; lo2++) {
      for (hi2 = lo2; hi2 <= interval::MAX; hi2++) {
        for (hi1 = lo1; hi1 <= interval::MAX; hi1++) {
          interval b = interval(lo1, hi1) & interval(lo2, hi2);
          regular_bits += b.bits();

          interval a = exhaustive_oper(interval(lo1, hi1), interval(lo2, hi2), 2);
          exhaustive_bits += a.bits();
          //Uncomment to see the debug output 
          if (a.bits() + 2 >= b.bits()) {
            continue;
          }
          cout << "Exhaustive and of " << interval(lo1, hi1) << " and " << interval(lo2, hi2) << " resulted to " << a << endl;
          cout << "Regular and of " << interval(lo1, hi1) << " and " << interval(lo2, hi2) << " resulted to " << b << endl;
          if (a <= b) {
            continue;
          }
          assert(false);
        }
      }
    } 
  }
  cout << "And Success Regular bit " << regular_bits << " Exhaustive bit " << exhaustive_bits << "Ratio  " << (double)regular_bits/(double)exhaustive_bits << endl;
  //cout << "And Success Regular bit " << regular_bits << " Exhaustive bit " << exhaustive_bits << endl; 
}

int main() {
  //exhaustive_sub();
  exhaustive_and();
  return 1;
}
