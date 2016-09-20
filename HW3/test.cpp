#include<stdio.h>
#include <assert.h>
#include<iostream>
#include"interval.cpp"
using namespace std;

const int SIZE = 5;
interval abstract(int *values, int count);
void concreate(interval a, int *values, int *count);
int sub(int a, int b);
interval exhaustive_sub(interval a, interval b, int oper);
void exhaustive_sub();
void exhaustive_and();

interval abstract(int *values, int count) {
  int i, low = HIGH, high = LOW;
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
  for (i = a.getLow(); i <= a.getHigh(); i++) {
    values[*count] = i;
    *count += 1;
  }
}

int sub(int a, int b) {
  int c = a - b;
  get_valid_value(&c);
  return c;
}

void to_bits(int a, int bit_array[]) {
  int d, old_a = a;
  for (d = 0; d < SIZE; d++) {
    bit_array[d] = ((a%2) == 1) ? 1 : (((a%2) == -1) ? 1 :0) ;
    a = a >> 1;
  }
  if (old_a < 0) {
    bit_array[SIZE - 1] = 1;
  }
}

void print_bits(int bit_array[]) {
  int count = SIZE - 1;
  for (; count >= 0; count--) {
    cout << "pos:" << count << "value:" << bit_array[count] << endl;
  }
}
int int_from_bits(int bit_array[]) {
  int x = 0, count = SIZE - 1;
  bool isNegative = false;
  if (bit_array[count--] == 1) {
    isNegative = true;
  }
  for (; count >= 0; count--) {
    x = x << 1;
    if (isNegative) {
      if (bit_array[count] == 0) {
        x = x | 1;
      }
    } else {
      if (bit_array[count] == 1) {
        x = x | 1;
      }
    }
  }

  if (isNegative) {
    x += 1;
    x = x * -1;
  }
  return x;
}

int and_operation(int a, int b) {
  int a_bit_array[SIZE], b_bit_array[SIZE], count = 0, c_bit_array[SIZE];
  to_bits(a, a_bit_array);
  to_bits(b, b_bit_array);
  for (count = 0; count < SIZE; count ++) {
    c_bit_array[count] = a_bit_array[count] & b_bit_array[count];
  }
  return int_from_bits(c_bit_array);
}

interval exhaustive_oper(interval a, interval b, int oper) {
  int values1[RANGE], values2[RANGE], count1 = 0, count2 = 0, values3[RANGE*RANGE], count3 = 0;
  int x,y; 
  concreate(a, values1, &count1);
  concreate(b, values2, &count2);

  for(x = 0; x < count1; x++) {
    for (y = 0; y < count2; y++) {
      if (oper == 1) {
        values3[count3++] = sub(values1[x], values2[y]);
      } else {
        values3[count3++] = and_operation(values1[x], values2[y]);
      }
    }
  }

  return abstract(values3, count3);
}

void exhaustive_sub() {
  int lo1, lo2, hi1, hi2;
  for( lo1 = LOW; lo1 <= HIGH; lo1++) {
    for (lo2 = LOW; lo2 <= HIGH; lo2++) {
      for (hi2 = lo2; hi2 <= HIGH; hi2++) {
        for (hi1 = lo1; hi1 <= HIGH; hi1++) {
          interval a = exhaustive_oper(interval(lo1, hi1), interval(lo2, hi2), 1);
          interval b = interval::subtract(interval(lo1, hi1), interval(lo2, hi2));
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

void exhaustive_and() {
  int lo1, lo2, hi1, hi2;
  for( lo1 = LOW; lo1 <= HIGH; lo1++) {
    for (lo2 = LOW; lo2 <= HIGH; lo2++) {
      for (hi2 = lo2; hi2 <= HIGH; hi2++) {
        for (hi1 = lo1; hi1 <= HIGH; hi1++) {
          interval a = exhaustive_oper(interval(lo1, hi1), interval(lo2, hi2), 2);
          interval b = interval::bitwise_and(interval(lo1, hi1), interval(lo2, hi2));
          //Uncomment to see the optimized output 
          if (a <= b) {
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
  cout << "And Success" << endl;
}

int main() {
  exhaustive_sub();
  exhaustive_and();
  return 1;
}
