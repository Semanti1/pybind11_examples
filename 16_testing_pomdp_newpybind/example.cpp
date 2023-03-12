#include <string>
using namespace std;
using std::string;
#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>
#include <pybind11/stl_bind.h>
#include <list>
#include "pouct.h"
//#include "oopomdp.h"
//PYBIND11_MAKE_OPAQUE(std::map<std::shared_ptr<State>, float>);
//#include "basics.h"
//#include "planner.h"
//
// ------------------
// regular C++ header
// ------------------

// parent class
/*class State
{

};
class Observation
{

};
class Action {
public:
    string name_;
};
class History {

public:
    History() {};
    void add(Action act, Observation obs) {};
    History(list< tuple<Action, Observation>> _history) :history(_history) {}

private:
    //Action action;
    //Observation obs;
    list< tuple<Action, Observation>> history;


};
class ObservationModel {
public:
    virtual double probability(const Observation& observation,
        const State& next_state,
        const Action& action)=0;

    virtual State sample(const State& next_state,
        const Action& action)=0;

    virtual State argmax(const State& next_state,
        const Action& action)=0;
};

class TransitionModel {
public:
    //virtual TransitionModel()=0;
    virtual double probability(const State& next_state,
        const State& state,
        const Action& action)=0;

    virtual State sample(const State& state,
        const Action& action)=0 ;

    virtual State argmax(const State& state,
        const Action& action) {};
};

class RewardModel {
public:
    double probability(double reward,
        const State& state,
        const Action& action, const State& next_state) {};

    double sample(const State& state,
        const Action& action,
        const State& next_state) {};

    double argmax(const State& state,
        const Action& action,
        const State& next_state) {};
};

class PolicyModel {
public:
    double probability(const Action& action,
        const State& state
    ) {};
    
    Action sample(State& state) {};
    double argmax(const State& state) {};
    //virtual list<Action> getAllActions(State& state) = 0;
    list<Action> getAllActions(State& state, History& history) {};


};*/

/*
class PyObservationModel : public ObservationModel
{
public:

    // inherit the constructors
    using ObservationModel::ObservationModel;

    // trampoline (one for each virtual function)
    double probability(const Observation& observation,
        const State& next_state,
        const Action& action) override {
        PYBIND11_OVERLOAD_PURE(
            double,           // Return type 
            ObservationModel, // Parent class 
            probability,        // Name of function in C++ (must match Python name) 
            observation,      // Argument(s) 
            next_state,
            action
        );
    }

    State sample(const State& next_state,
        const Action& action) override {
        PYBIND11_OVERLOAD_PURE(
            State, // Return type //
            ObservationModel,      // Parent class //
            sample,        // Name of function in C++ (must match Python name) //
                           // Argument(s) //
            next_state,
            action
        );
    }
    State argmax(const State & next_state,
            const Action & action) override {
            PYBIND11_OVERLOAD_PURE(
                State, // Return type //
                ObservationModel,      // Parent class //
                argmax,        // Name of function in C++ (must match Python name) //
                               // Argument(s) //
                next_state,
                action
            );
    }
};

class PyTransitionModel : public TransitionModel
{
public:

    // inherit the constructors
    using TransitionModel::TransitionModel;

    // trampoline (one for each virtual function)
    double probability(const State& next_state,
        const State& state,
        const Action& action) override {
        PYBIND11_OVERLOAD_PURE(
            double, // Return type //
            TransitionModel,      // Parent class //
            probability,        // Name of function in C++ (must match Python name) //
            next_state,      // Argument(s) //
            state,
            action
        );
    }

    State sample(const State& state,
        const Action& action) override {
        PYBIND11_OVERLOAD_PURE(
            State, // Return type //
            TransitionModel,      // Parent class //
            sample,        // Name of function in C++ (must match Python name) //
                           // Argument(s) //
            state,
            action
        );
    }
    State argmax(const State& state,
        const Action& action) override {
        PYBIND11_OVERLOAD_PURE(
            State, // Return type //
            TransitionModel,      // Parent class //
            argmax,        // Name of function in C++ (must match Python name) //
                           // Argument(s) //
            state,
            action
        );
    }
};
*/

// ----------------
// Python interface
// ----------------
/*class PyState : public State

{
public:

    // inherit the constructors
    using State::State;
};*/
/*class PyActionPrior : public ActionPrior
{
public:

    // inherit the constructors
    using ActionPrior::ActionPrior;

    // trampoline (one for each virtual function)
    set<tuple<Action*, int, float>> get_preferred_actions(State* state, History* history
    ) override {
        PYBIND11_OVERLOAD_PURE(
            set<tuple<Action*, int, float>>,
            ActionPrior,
            get_preferred_actions,
            state,
            history

        );
    }



};*/
namespace py = pybind11;

PYBIND11_MODULE(example, m)
{
    /*py::class_<State>(m, "State", py::dynamic_attr())
        .def(py::init<string>())
        .def(py::init<>())
        .def_readwrite("name", &State::name);*/

    
    py::class_<State, PyState, std::shared_ptr<State>> state(m, "State", py::dynamic_attr());
    state
        .def(py::init<string>())
        .def(py::init<>())
        .def("__copy__", [](const State& self) {
        return State(self);
            })
        .def("__deepcopy__", [](const State& self, py::dict) {
                return State(self);
            },"memo" )
        .def("getname", &State::getname)
        .def_readwrite("name", &State::name);

    py::class_<Belief, PyBelief, std::shared_ptr<Belief>> belief_model(m, "Belief");
    belief_model
        .def(py::init<>())
        .def("random", &Belief::random);
    
        /*py::class_<Belief>(m, "Belief")
        .def("random", &Belief::random);*/
        //.def(py::init<>());
   
    
    py::class_<Action, PyAction, std::shared_ptr<Action>> action(m, "Action", py::dynamic_attr());
    action
        .def(py::init<string>())
        .def(py::init<>())
        .def_readwrite("name", &Action::name);
    /*py::class_<Action>(m, "Action", py::dynamic_attr())
        .def(py::init<string>())
        .def(py::init<>())
        .def_readwrite("name", &Action::name);*/

    py::class_<Observation,PyObservation, std::shared_ptr<Observation>>(m, "Observation")
        .def(py::init<string>());
    // py::class_<Planner, PyPlanner, std::shared_ptr<Planner>> planner(m, "Planner");
    // planner
    py::class_<History, PyHistory, std::shared_ptr<History>> history(m, "History", py::dynamic_attr());
    history
        
        .def(py::init<>())
        .def("getHist", &History::getHist)
        .def_readwrite("history", &History::history);
    py::class_<Planner, PyPlanner, std::shared_ptr<Planner>> pln(m, "Planner");
    pln
        //.def(py::init<>())
        .def("plan", &Planner::plan);

   

    /*py::class_<VNode>(m, "VNode")
        .def(py::init<int>())
        .def("getnumvis", &VNode::getnumvis);*/
    py::class_<ObservationModel, PyObservationModel, std::shared_ptr<ObservationModel>> omodel(m, "ObservationModel");
    omodel
        .def(py::init<>())
        .def("probability", &ObservationModel::probability)
        .def("sample", &ObservationModel::sample)
        .def("argmax", &ObservationModel::argmax);

    /*py::class_<TransitionModel, PyTransitionModel>(m, "TransitionModel")
        .def(py::init<>())
        .def("probability", &TransitionModel::probability)
        .def("sample", &TransitionModel::sample)
        .def("argmax", &TransitionModel::argmax);*/
    
    py::class_<TransitionModel, PyTransitionModel, std::shared_ptr<TransitionModel>> tmodel(m, "TransitionModel");
    tmodel
        .def(py::init<>())
        .def("probability", &TransitionModel::probability)
        .def("sample", &TransitionModel::sample);
       // .def("argmax", &TransitionModel::argmax);
    py::class_<RewardModel, PyRewardModel, std::shared_ptr<RewardModel>> rmodel(m, "RewardModel");
    rmodel
        .def(py::init<>())
        //.def("probability", &RewardModel::probability)
        .def("sample", &RewardModel::sample);
        //.def("argmax", &RewardModel::argmax);

    py::class_<PolicyModel, PyPolicyModel, std::shared_ptr<PolicyModel>> pmodel(m, "PolicyModel");
    pmodel
        .def(py::init_alias<>())
        .def("probability", &PolicyModel::probability)
        .def("sample", &PolicyModel::sample)
        .def("argmax", &PolicyModel::argmax)
        .def("get_all_actions", &PolicyModel::get_all_actions);

     //py::class_<RolloutPolicy, PyRolloutPolicy, std::shared_ptr<RolloutPolicy>> rollmodel(m, "RolloutPolicy", pmodel);
     py::class_<RolloutPolicy, PyRolloutPolicy, std::shared_ptr<RolloutPolicy>> rollmodel(m, "RolloutPolicy", pmodel);
     rollmodel
         .def(py::init_alias<>())
         .def("probability", &RolloutPolicy::probability)
         .def("sample", &RolloutPolicy::sample)
         .def("argmax", &RolloutPolicy::argmax)
         .def("get_all_actions", &RolloutPolicy::get_all_actions)
         .def("rollout", &RolloutPolicy::rollout);
   /* py::class_<RolloutPolicy>(m, "RolloutPolicy")
        .def(py::init<>())
        .def("rollout", &RolloutPolicy::rollout);*/
        

        
    /*py::class_<Environment>(m, "Environment")
        .def(py::init<State*, TransitionModel*, RewardModel* >())
        //.def(py::init<State&>())
        .def("getstate", &Environment::getstate)
        .def("transitionmodel", &Environment::transitionmodel)
        .def("reward_model", &Environment::reward_model)
        .def("state_transition", &Environment::state_transition)
        .def("state_transition_sim", &Environment::state_transition_sim)
        .def("apply_transition", &Environment::apply_transition)
        .def("execute", &Environment::execute)
        .def("provide_observation", &Environment::provide_observation);*/

    py::class_<Environment, PyEnvironment, std::shared_ptr<Environment>> environment(m, "Environment");
    environment
    
        .def(py::init<std::shared_ptr<State>, std::shared_ptr<TransitionModel>, std::shared_ptr<RewardModel> >())
        //.def(py::init<State&>())
        .def("getstate", &Environment::getstate)
        .def("transitionmodel", &Environment::transitionmodel)
        .def("reward_model", &Environment::reward_model)
        .def("state_transition", &Environment::state_transition)
        .def("state_transition_sim", &Environment::state_transition_sim)
        .def("apply_transition", &Environment::apply_transition)
        .def("execute", &Environment::execute)
        .def("provide_observation", &Environment::provide_observation);

    // py::class_<Histogram, Belief>(m, "Histogram")
    py::class_<Histogram, PyHistogram, std::shared_ptr<Histogram>> histogram(m, "Histogram", belief_model);
    histogram

        .def(py::init<std::map<std::shared_ptr<State>, float>>())
        .def("getHist", &Histogram::getHist)
        .def("lenHist", &Histogram::lenHist)
        .def("getitem", &Histogram::getitem)
        .def("setitem", &Histogram::setitem)
        .def("isEq", &Histogram::isEq)
        .def("mpe", &Histogram::mpe)
        .def("random", &Histogram::random)
        .def("isNormalized", &Histogram::isNormalized)
        .def("update_hist_belief", &Histogram::update_hist_belief);

//  py::class_<Histogram, Belief>(m, "Histogram")
//          .def(py::init<std::map<State*, float>>())
//          .def("getHist", &Histogram::getHist)
//          .def("lenHist", &Histogram::lenHist)
//          .def("getitem", &Histogram::getitem)
//          .def("isNormalized", &Histogram::isNormalized)
//          .def("update_hist_belief", &Histogram::update_hist_belief);
    py::class_<Agent, PyAgent, std::shared_ptr<Agent>> amodel(m, "Agent");
    amodel
        
        .def(py::init_alias<std::shared_ptr<Belief>, std::shared_ptr<PolicyModel>, std::shared_ptr<TransitionModel>,
            std::shared_ptr<ObservationModel>, std::shared_ptr<RewardModel> >())
        //.def(py::init<std::shared_ptr<Belief>, std::shared_ptr<TransitionModel>, std::shared_ptr<ObservationModel>,
         //   std::shared_ptr<RewardModel> >())
        .def("gethistory", &Agent::gethistory)
        .def("update_hist", &Agent::update_hist)
        .def("init_belief", &Agent::init_belief)
        .def("belief", &Agent::belief)
        .def("cur_belief", &Agent::cur_belief)
        .def("setbelief", &Agent::setbelief)        
        .def("sample_belief", &Agent::sample_belief)
        .def("getObsModel", &Agent::getObsModel)
        .def("getTransModel", &Agent::getTransModel)
        .def("getRewardModel", &Agent::getRewardModel)
        .def("getPolicyModel", &Agent::getPolicyModel)
        
        .def("validActions", &Agent::validActions);
        //.def("update", &Agent::update)
       // .def("getRewardModel", &Agent::getRewardModel);

    py::class_<ActionPrior>(m, "ActionPrior")
        //.def(py::init<>())
        .def("get_preferred_actions", &ActionPrior::get_preferred_actions);

    py::class_<POUCT, PyPOUCT, std::shared_ptr<POUCT> >(m, "POUCT", pln)
        .def(py::init<int, float, int, float, float, int, float, std::shared_ptr<RolloutPolicy>, bool, int, std::shared_ptr<Agent>>())
        .def("getAgent", &POUCT::getAgent)
        .def("setAgent", &POUCT::setAgent)
        .def("update", &POUCT::update)
        .def("plan", &POUCT::plan);
        /*.def("lenHist", &Histogram::lenHist)
        .def("getitem", &Histogram::getitem)
        .def("setitem", &Histogram::setitem)
        .def("isEq", &Histogram::isEq)
        .def("mpe", &Histogram::mpe)
        .def("random", &Histogram::random)
        .def("isNormalized", &Histogram::isNormalized)
        .def("update_hist_belief", &Histogram::update_hist_belief);*/
    /*py::class_<ObjectState, PyObjectState, std::shared_ptr<ObjectState>> objst(m, "ObjectState");
    objst

        .def(py::init<string, std::unordered_map<string, int> >())
        //.def(py::init<std::shared_ptr<Belief>, std::shared_ptr<TransitionModel>, std::shared_ptr<ObservationModel>,
         //   std::shared_ptr<RewardModel> >())
        .def("getItem", &ObjectState::getItem)
        .def("setItem", &ObjectState::setItem)
        .def("attrlen", &ObjectState::attrlen)
        .def("copy", &ObjectState::copy);

    py::class_<OOState, PyOOState, std::shared_ptr<OOState>> oost(m, "ObjectState");
    oost

        .def(py::init< std::unordered_map<string, std::shared_ptr<ObjectState>> >())
        //.def(py::init<std::shared_ptr<Belief>, std::shared_ptr<TransitionModel>, std::shared_ptr<ObservationModel>,
         //   std::shared_ptr<RewardModel> >())
        .def("get_object_state", &OOState::get_object_state)
        .def("set_object_state", &OOState::set_object_state)
        .def("get_object_class", &OOState::get_object_class)
        .def("get_object_attribute", &OOState::get_object_attribute);*/

    m.def("sample_generative_model", &sample_generative_model, py::arg("agent"), py::arg("state"),py::arg("action"),py::arg("discount_factor")=1);
    //m.def("sample_explict_models1", &sample_explict_models1, py::arg("T"), py::arg("O"), py::arg("R"), py::arg("state"), py::arg("a"),py::arg("discount_factor")=1);
    m.def("sample_explict_models", &sample_explict_models, py::arg("T"), py::arg("R"), py::arg("state"), py::arg("a"), py::arg("discount_factor")=1);
    /*py::class_<RandomRollout>(m, "RandomRollout")
        .def("rollout", &RandomRollout::rollout);*/

}
