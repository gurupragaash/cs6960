#include <iostream>
#include <cmath>
#include <cassert>

/*
 * 6-bit signed intervals. it is required that hi >= lo. the bounds
 * are inclusive.
 */
struct interval {

  static const int MIN = -32;
  static const int MAX = 31;
  static const int RANGE = MAX - MIN + 1;
  const int lo, hi;

  interval(int _lo, int _hi) : lo(_lo), hi(_hi) {
    assert(lo >= MIN && lo <= MAX);
    assert(hi >= MIN && hi <= MAX);
    assert(lo <= hi);
  }

  static interval top() { return interval(MIN, MAX); }

  double bits() { return log2(1 + hi - lo); }

  friend std::ostream &operator<<(std::ostream &os, const interval &i) {
    os << "[" << i.lo << ".." << i.hi << "]";
    return os;
  }

  bool operator==(const interval &other) const {
    return lo == other.lo && hi == other.hi;
  }

  bool operator!=(const interval &other) const {
    return lo != other.lo || hi != other.hi;
  }

  bool operator<=(const interval &other) const {
    return lo >= other.lo && hi <= other.hi;
  }

  bool operator>=(const interval &other) const {
    return lo <= other.lo && hi >= other.hi;
  }

  /*
   * requirement: be sound and fully precise
   * implement this function in subtract.cpp
   */
  interval operator-(const interval &);

  /*
   * requirement: be sound and don't always return top
   * implement this function in and.cpp
   */
  interval operator&(const interval &);
};
