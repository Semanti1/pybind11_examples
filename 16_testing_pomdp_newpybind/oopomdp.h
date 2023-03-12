#include "basics.h"
#include <string>


#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
#include <list>
#include <map>
#include <pybind11/stl.h>

class ObjectState :public State {
public:

	string objclass;
	std::unordered_map<string, int> attributes;
	
	ObjectState(string _objclass, std::unordered_map<string, int> _attributes) :objclass(_objclass), attributes(_attributes) {}
	int getItem(string attr) { return attributes[attr]; }
	void setItem(string attr, int value) { attributes[attr] = value; }
	int attrlen() { return attributes.size(); }
	ObjectState copy() { return ObjectState(objclass, attributes); }
	virtual ~ObjectState() {}
};

class OOState :public State {
public:

	
	std::unordered_map<string, std::shared_ptr<ObjectState>> object_states;
	OOState(std::unordered_map<string, std::shared_ptr<ObjectState>> object_states_) :object_states(object_states_) {}
	std::shared_ptr<ObjectState> get_object_state(string objid) { return object_states[objid]; }
	void set_object_state(string objid, std::shared_ptr<ObjectState> newobj) { object_states[objid] = newobj; }
	string get_object_class(string objid) { return object_states[objid]->objclass; }
	int get_object_attribute(string objid, string attr) { object_states[objid]->attributes[attr]; }
};

class OOTransitionModel :public TransitionModel {
public:
	std::unordered_map<string, std::shared_ptr<TransitionModel>> transition_models;
	OOTransitionModel(std::unordered_map<string, std::shared_ptr<TransitionModel>> transition_models_):transition_models(transition_models_){}
	double probability(State* next_state,State* state,Action* action);
	std::shared_ptr<State> sample(State* state,	Action* action);
	//State argmax(const State& state,const Action& action) {};
	//std::shared_ptr<State> argmax(std::shared_ptr<State> state,
		//std::shared_ptr<Action> action);
};
class PyOOTransitionModel : public py::wrapper<OOTransitionModel> {
public:
	/* Inherit the constructors */
	using py::wrapper<OOTransitionModel>::wrapper;
	double probability(State* next_state,
		State* state,
		Action* action) override {
		PYBIND11_OVERLOAD_PURE(
			double, /* Return type */
			OOTransitionModel,      /* Parent class */
			probability,        /* Name of function in C++ (must match Python name) */
			next_state,      /* Argument(s) */
			state,
			action
		);
	}

	std::shared_ptr<State> sample(State* state,
		Action* action) override {
		PYBIND11_OVERLOAD_PURE(std::shared_ptr<State>, OOTransitionModel, sample, state, action);
	}
};
class PyObjectState : public py::wrapper<ObjectState> {
public:
	/* Inherit the constructors */
	using py::wrapper<ObjectState>::wrapper;

};
class PyOOState : public py::wrapper<OOState> {
public:
	/* Inherit the constructors */
	using py::wrapper<OOState>::wrapper;

};