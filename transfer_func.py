interval_range = 16
class Interval(tuple):
  def __new__(cls, a, b):
    if type(a) != int:
      raise Exception("Interval class doesnt take arguments which are not int")
    if type(b) != int:
      raise Exception("Interval class doesnt take arguments which are not int")
    if a > b:
      raise Exception("In Interval(a,b),  a<=b. Invalid input - "+
              "Interval({}, {})".format(a,b))
    if ((a >= interval_range) or (b >= interval_range)):
      raise Exception("Invalid range. Max value possible is {}".format(interval_range))
    return tuple.__new__(cls, (a, b))

  def __sub__(self, other):
    global interval_range
    new_min = self[0] - other[1]
    min_wrapped = False
    if new_min < 0:
      new_min = new_min % interval_range
      min_wrapped = True
    new_max = self[1] - other[0]
    max_wrapped = False
    if new_max < 0:
      new_max = new_max % interval_range
      max_wrapped = True

    try :
      if min_wrapped == max_wrapped:
        return Interval(new_min, new_max)
      return Interval(0, interval_range - 1)
    except :
      print("This did happen")
      #Means new_min > new_max, return full interval
      #I dont think this can happen. But we have to run
      #through all the cases
      return Interval(0, interval_range - 1)

#testcases

#For the below case the lowest possible value is 7-6 = 1 and 
#the highest will 5-6 = 15. So result is (1, 15)
print("{} - {} = {}".format(Interval(2,6), Interval(5,7), 
    (Interval(2,6) - Interval(5,7))))


#For the below case, the lowest possible will be 5-1 = 4 and 
#the highest could be 6-1 = 5. So result is (4,5)
print("{} - {} = {}".format(Interval(5,6), Interval(1,1), 
    (Interval(5,6) - Interval(1,1))))


#During one wrap around. ie the result set wraps around
print("{} - {} = {}".format(Interval(0,4), Interval(3, 3),
    (Interval(0,4) - Interval(3,3))))

#2 wrap around but the min is > max
print("{} - {} = {}".format(Interval(4,4), Interval(13, 15),
    (Interval(4,4) - Interval(13,15))))

#Two wrap arounds. ie all the entries in the result set wraps around
print("{} - {} = {}".format(Interval(4,4), Interval(13, 15),
    (Interval(4,4) - Interval(13,15))))
