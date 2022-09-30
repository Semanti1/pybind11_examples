from example import *
import example


#from examples import *

class Cat(Animal):
    def __init__(self,name):
        Animal.__init__(self)
        self.name = name
    def go(self, n_times):
        #return "meow! " * n_times
        print("meow! " * n_times)
        return Cat(self.name+str(n_times))
    def getname(self):
        return self.name #Cat(self.name+str(2))


c = Cage()
cat = Cat("billy")
c.add(cat)
d = c.release()
#print(d.go(3))
print("name ", d.name)