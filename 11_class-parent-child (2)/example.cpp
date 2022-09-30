
#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
using namespace std;
namespace py = pybind11;
// ------------------
// regular C++ header
// ------------------

// parent class

class Animal
{
public:
  virtual std::string talk(int n_times) = 0;
  virtual ~Animal() {}
};
class AnEnv
{
public:
    AnEnv(Animal& an): initan(an) {}
    Animal& ani() { return initan; }
    Animal& chktalk(Animal& an, int t)
    {
        cout << an.talk(t)+ani().talk(t)<<endl;
        return initan;
    };
    virtual ~AnEnv() {}
private:
    Animal& initan;
};
// derived class
class Dog : public Animal
{
public:
  std::string talk(int n_times) override;

};

// derived class
class Cat : public Animal
{
public:
  std::string talk(int n_times) override;
};

// function that takes the parent and all derived classes
std::string talk(Animal& animal, int n_times = 1);
class State
{
public:
    State(string name_) :name(name_) {}
    State() {}
    //virtual Animal* chkst() = 0;
    virtual std::shared_ptr<Animal> chkst() = 0;
    //protected:
    string name;
    virtual ~State() {}
};
class TestAnimals
{
public:
    //TestAnimals(Animal* a, Animal* b) :inita(a), initb(b) {}
    TestAnimals(std::shared_ptr<Animal> a, std::shared_ptr<Animal> b) :inita(a), initb(b) {}
    //virtual void chkslicing(State* c)
    virtual void chkslicing(std::shared_ptr<State> c)
    {
        cout << inita->talk(2)<<endl;
        cout << initb->talk(3)<< endl;
        std::shared_ptr<Animal> ant = c->chkst();
        cout << c->chkst() << endl;
        cout << ant->talk(5) << endl;
    }
    virtual ~TestAnimals() {}
private:
    //Animal* inita;
    //Animal* initb;
    std::shared_ptr<Animal> inita;
    std::shared_ptr<Animal> initb;
};

// ----------------
// regular C++ code
// ----------------

inline std::string Dog::talk(int n_times)
{
  std::string result;

  for ( int i = 0 ; i < n_times ; ++i )
    result += "woof! ";

  return result;
}


inline std::string Cat::talk(int n_times)
{
  std::string result;

  for ( int i = 0 ; i < n_times ; ++i )
    result += "meow! ";

  return result;
}


std::string talk(Animal &animal, int n_times)
{
  return animal.talk(n_times);
}

// -----------------------------
// Python interface - trampoline
// -----------------------------

class PyAnimal : public Animal
{
public:
   // using py::wrapper<Animal>::wrapper;
  // inherit the constructors
  using Animal::Animal;

  // trampoline (one for each virtual function)
  std::string talk(int n_times) override {
    PYBIND11_OVERLOAD_PURE(
      std::string, /* Return type */
      Animal,      /* Parent class */
      talk,        /* Name of function in C++ (must match Python name) */
      n_times      /* Argument(s) */
    );
  }
};
class PyState :public State
{
public:

    // inherit the constructors
    //using py::wrapper<State>::wrapper;
    using State::State;

    // trampoline (one for each virtual function)
    std::shared_ptr<Animal> chkst() override {
        PYBIND11_OVERLOAD_PURE(
            std::shared_ptr<Animal>, /* Return type */
            State,      /* Parent class */
            chkst        /* Name of function in C++ (must match Python name) */
                  /* Argument(s) */
        );
    }
};
// ----------------
// Python interface
// ----------------
namespace py = pybind11;
class PyTestAnimals : public TestAnimals {
public:
    PyTestAnimals(std::shared_ptr<Animal> a, std::shared_ptr<Animal> b) : TestAnimals(a,b), pyobj(py::cast(a)), pyobj1(py::cast(b)) {}
    void chkslicing(std::shared_ptr<State> c) override {
        PYBIND11_OVERLOAD(
            void, /* Return type */
            TestAnimals,      /* Parent class */
            chkslicing,       /* Name of function in C++ (must match Python name) */
            c      /* Argument(s) */
        );
    }
private:
    py::object pyobj;
    py::object pyobj1;
};


PYBIND11_MODULE(example, m)
{
  py::class_<Animal, std::shared_ptr<Animal>, PyAnimal>(m, "Animal")
    //.def(py::init<>())
    .def(py::init_alias<>())
    .def("talk", &Animal::talk);

  py::class_<Dog, Animal, std::shared_ptr<Dog>>(m, "Dog")
      .def(py::init<>());
    //.def(py::init_alias<>());

  py::class_<Cat, Animal, std::shared_ptr<Cat>>(m, "Cat")
      //.def(py::init_alias<>());
    .def(py::init<>());

  py::class_<AnEnv, std::shared_ptr<AnEnv>>(m, "AnEnv")
      //.def(py::init_alias<Animal&>())
      .def(py::init<Animal&>())
      .def("ani", &AnEnv::ani)
      .def("chktalk", &AnEnv::chktalk);

  py::class_<TestAnimals, std::shared_ptr<TestAnimals>, PyTestAnimals>(m, "TestAnimals")
      //.def(py::init<std::shared_ptr<Animal>, std::shared_ptr<Animal>>())
      .def(py::init_alias<std::shared_ptr<Animal>, std::shared_ptr<Animal> >())
      //.def(py::init<Animal*, Animal*>())
      .def("chkslicing", &TestAnimals::chkslicing);
      
  py::class_<State, std::shared_ptr<State>,PyState>(m, "State", py::dynamic_attr())
      //.def(py::init<string>())
      //.def(py::init<>())
      .def(py::init_alias<string>())
      .def(py::init_alias<>())
      .def_readwrite("name", &State::name);
  m.def("talk", &talk, py::arg("animal") ,py::arg("n_times")=1);
}
