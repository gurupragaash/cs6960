#include <assert.h>
#include<iostream>
#include"utils.cpp"
#include"interval_sub.cpp"
#include"interval_and.cpp"
using namespace std;

interval abstract(int *values, int count);
void concreate(interval a, int *values, int *count);
int sub(int a, int b);
interval exhaustive_oper(interval a, interval b, int oper);
void exhaustive_sub();
void exhaustive_and();

interval abstract(int *values, int count) {
  int i, low = interval::MAX, high = interval::MIN;
  if (count == 0) {
    cout << "Count is 0";
    //assert(false);
  }
  for (i = 0; i < count; i++) {
    low = (low > values[i]) ? values[i] : low;
    high = (high < values[i]) ? values[i] : high;
  }

  return interval(low, high);
}

interval bitwise_and(interval first, interval second) {
  int lowest = interval::MIN, highest = interval::MAX;
  
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
          //assert(false);
        }
      }
    } 
  }
  cout << "Subtraction Success" << endl;
}

void exhaustive_and() {
  int regular_bits = 0, exhaustive_bits = 0, old_bits = 0;
  int lo1, lo2, hi1, hi2;
  for( lo1 = interval::MIN; lo1 <= interval::MAX; lo1++) {
    for (lo2 = interval::MIN; lo2 <= interval::MAX; lo2++) {
      for (hi2 = lo2; hi2 <= interval::MAX; hi2++) {
        for (hi1 = lo1; hi1 <= interval::MAX; hi1++) {
          interval a = exhaustive_oper(interval(lo1, hi1), interval(lo2, hi2), 2);
          interval b = interval(lo1, hi1) & interval(lo2, hi2);
          interval c = bitwise_and(interval(lo1, hi1), interval(lo2, hi2));
          //Uncomment to see the optimized output 
          exhaustive_bits += a.bits();
          regular_bits += b.bits();
          old_bits += c.bits();
          if (a == b) {
            continue;
          }
          //cout << "Exhaustive and of " << interval(lo1, hi1) << " and " << interval(lo2, hi2) << " resulted to " << a << endl;
          //cout << "Regular and of " << interval(lo1, hi1) << " and " << interval(lo2, hi2) << " resulted to " << b << endl;
          //cout << "First Known ones and zeros are " << (knownOnes(interval(lo1, hi1))) << " , " << (knownZeros(interval(lo1, hi1))) << endl;
          //cout << "Other Known ones and zeros are " << (knownOnes(interval(lo2, hi2))) << " , " << (knownZeros(interval(lo2, hi2))) << endl;
          //cout << "Result OR Known ones and zeros are " << (knownOnes(interval(lo1, hi1)) & knownOnes(interval(lo2, hi2))) << " , " << (knownZeros(interval(lo1, hi1)) | knownZeros(interval(lo2, hi2))) << endl;
          //cout << "Result AND Known ones and zeros are " << (knownOnes(interval(lo1, hi1)) & knownOnes(interval(lo2, hi2))) << " , " << (knownZeros(interval(lo1, hi1)) & knownZeros(interval(lo2, hi2))) << endl;
          if (a <= b) {
            continue;
          }
          //assert(false);
        }
      }
    } 
  }
  cout << "And Success Regular bit " << regular_bits << " Exhaustive bit " << exhaustive_bits << "Ratio  " << (double)regular_bits/(double)exhaustive_bits << endl;
  cout << "And Success Regular bit " << old_bits << " Exhaustive bit " << exhaustive_bits << "Ratio  " << (double)old_bits/(double)exhaustive_bits << endl;
}

int main() {
  //exhaustive_sub();
  exhaustive_and();
/*
  cout <<"case 1: bitwise and of all positive integers" << endl;
  cout << exhaustive_oper(interval(1, 15), interval(0,3),2)<< endl;
  cout <<"case 2: bitwise and of all negative integers" << endl;
  cout << exhaustive_oper(interval(-8, -6), interval(-12, -3),2)<< endl;
  cout <<"case 3: bitwise and of negative interval and positive interval" << endl;
  cout << exhaustive_oper(interval(-16, 0), interval(0, 15),2)<< endl;
  cout <<"case 4: bitwise and of mix of intervals" << endl;
  cout << exhaustive_oper(interval(-10, 10), interval (-16, 3),2)<< endl;
  cout <<"case 5: bitwise and full intervals" << endl;
  cout << exhaustive_oper(interval(-16, 15), interval(-16, 15),2)<< endl;
 */
  return 1;
}
