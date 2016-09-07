class Interval(tuple):
  def __new__(cls, a, b):
    if type(a) != int:
      raise Exception("Interval class doesnt take arguments which are not int")
    if type(b) != int:
      raise Exception("Interval class doesnt take arguments which are not int")
    if a > b:
      raise Exception("In Interval(a,b),  a<=b. Invalid input - "+
              "Interval({}, {})".format(a,b))
    return tuple.__new__(cls, (a, b))

  def __sub__(self, other):
    c = list()
    c.append((self[0] - other[0])%16)
    c.append((self[0] - other[1])%16)
    c.append((self[1] - other[0])%16)
    c.append((self[1] - other[1])%16)

    return Interval(min(c), max(c))

#testcases

#For the below case the lowest possible value is 7-6 = 1 and 
#the highest will 5-6 = 15. So result is (1, 15)
print("{} - {} = {}".format(Interval(2,6), Interval(5,7), 
    (Interval(2,6) - Interval(5,7))))


#For the below case, the lowest possible will be 5-1 = 4 and 
#the highest could be 6-1 = 5. So result is (4,5)
print("{} - {} = {}".format(Interval(5,6), Interval(1,1), 
    (Interval(5,6) - Interval(1,1))))
