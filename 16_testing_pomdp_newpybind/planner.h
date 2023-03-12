#include "basics.h"

#include <string>


#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
#include <list>
#include <map>
#include <pybind11/stl.h>
using namespace std;
using std::string;
//#include "pouct.h"
class Planner {
public:
	//virtual std::shared_ptr<Action> plan(std::shared_ptr<Agent> agent) = 0;
    virtual std::shared_ptr<Action> plan() = 0;
    virtual ~Planner() {}
	//virtual void update(Agent agent, Action real_action, Observation real_observation) = 0;
};

class PyPlanner : public py::wrapper<Planner>
{
public:

    // inherit the constructors
    using py::wrapper<Planner>::wrapper;

    // trampoline (one for each virtual function)
   // std::shared_ptr<Action> plan(std::shared_ptr<Agent> agent
        std::shared_ptr<Action> plan(
        ) override {
        PYBIND11_OVERLOAD_PURE(
            std::shared_ptr<Action>, /* Return type */
            Planner,      /* Parent class */
            plan       /* Name of function in C++ (must match Python name) */
                  /* Argument(s) */
            
        );
    }

    
    
};