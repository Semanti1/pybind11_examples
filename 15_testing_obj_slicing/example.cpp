
#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
using namespace std;
namespace py = pybind11;


class Animal {
public:
    virtual ~Animal() { }
    //virtual std::string go(int n_times) = 0;
    virtual std::shared_ptr<Animal> go(int n_times) = 0;
};

class PyAnimal : public py::wrapper<Animal> {
public:
    /* Inherit the constructors */
    using py::wrapper<Animal>::wrapper;
    std::shared_ptr<Animal> go(int n_times) override {
        PYBIND11_OVERLOAD_PURE(std::shared_ptr<Animal>, Animal, go, n_times);
    }
};

class Cage {
public:
    void add(std::shared_ptr<Animal> animal) {
        animal_ = animal;
    }
    std::shared_ptr<Animal> release() {
        //return std::move(animal_);
        shared_ptr<Animal> newan = animal_->go(3);
        //cout << "from c++" << newan->go(1);
        return newan;
    }
private:
    std::shared_ptr<Animal> animal_;
};

PYBIND11_MODULE(example, m) {
    py::class_<Animal, PyAnimal, std::shared_ptr<Animal>> animal(m, "Animal");
    animal
        .def(py::init<>())
        .def("go", &Animal::go);

    py::class_<Cage, std::shared_ptr<Cage>> cage(m, "Cage");
    cage
        .def(py::init<>())
        .def("add", &Cage::add)
        .def("release", &Cage::release);
}