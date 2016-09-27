#include"interval.h"
using namespace std;

void test_subtract(interval a, interval b) {
  interval c = a - b; 
  cout << a << " - " << b << " = " << c << endl;
}


void test_bitwise_and(interval a, interval b) {
  interval c = a & b; 
  cout << a << " & " << b << " = " << c << endl;
}

int main() {
  
  cout <<"case 1: no wrap around +ve integers with +ve integers as input" << endl;
  test_subtract(interval(5,10) , interval(0, 4));
  cout <<"case 2: no wrap around -ve integers with +ve integers as input" << endl; 
  test_subtract(interval(5,10) , interval(0, 12));
  cout <<"case 3: wrap around which results in full set" << endl;
  test_subtract(interval(14,15) , interval(-1, 2));
  cout <<"case 4: wrap around which results in smaller set" << endl;
  test_subtract(interval(12,15) , interval(-10, -4));
  cout <<"case 5: one of the sets is full set " << endl;
  test_subtract(interval(12,15) , interval(-16, 15));
  cout <<"case 6: both of the sets are full set " << endl;
  test_subtract(interval(-16,15) , interval(-16, 15));



  cout <<"case 1: bitwise and of all positive integers" << endl;
  test_bitwise_and(interval(1, 15), interval(0,3));
  cout <<"case 2: bitwise and of all negative integers" << endl;
  test_bitwise_and(interval(-8, -6), interval(-12, -3));
  cout <<"case 3: bitwise and of negative interval and positive interval" << endl;
  test_bitwise_and(interval(-16, 0), interval(0, 15));
  cout <<"case 4: bitwise and of mix of intervals" << endl;
  test_bitwise_and(interval(-10, 10), interval (-16, 3));
  cout <<"case 5: bitwise and full intervals" << endl;
  test_bitwise_and(interval(-16, 15), interval(-16, 15));
  return 1;
}


