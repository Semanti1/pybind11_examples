from example import *
import example

#print(example.talk(example.Dog(),2))
#print(example.talk(example.Cat(),3))

class Deer(Animal):
    def talk(self, n_times):
        return "bark! " * n_times
    def __del__(self):
        print("Deer was deleted")

class AnState(State):
    def __init__(self, name):
        State.__init__(self,name)
        self.stashed_st = None
        #self.name = name
    def chkst(self):
        self.stashed_st = Deer()
        return self.stashed_st
    def __del__(self):
        print("Anstate was deleted")

if __name__ == "__main__":
    '''d = Deer()
    print(d.talk(5))
    print(example.talk(example.Dog(),2))
    dg = Dog()
    print(d.talk(6))
    env = AnEnv(d)
    returnedobj = env.chktalk(dg,2)    
    print("checking obj slicing with Deer")
    print(example.talk(d,2))
    print(returnedobj.talk(5))'''
    dg = Dog()
    print ("check agn")
    
    print("DEER alive?")
    tigst = AnState("tiger")
    print("DEER alive?after")
    d2 = Deer()
    tani = TestAnimals(d2,dg)
    tani.chkslicing(tigst);
    #tigst = AnState("tiger")


