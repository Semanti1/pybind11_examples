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
	virtual Action* plan(Agent* agent) = 0;
	//virtual void update(Agent agent, Action real_action, Observation real_observation) = 0;
};

class PyPlanner : public Planner
{
public:

    // inherit the constructors
    using Planner::Planner;

    // trampoline (one for each virtual function)
    Action* plan(Agent* agent
        ) override {
        PYBIND11_OVERLOAD_PURE(
            Action*, /* Return type */
            Planner,      /* Parent class */
            plan,        /* Name of function in C++ (must match Python name) */
            agent      /* Argument(s) */
            
        );
    }

    
    
};