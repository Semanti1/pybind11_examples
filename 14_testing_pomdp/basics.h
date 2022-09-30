#include <string>
using namespace std;
using std::string;
#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
#include <list>
#include <map>
#include <pybind11/stl.h>
using namespace std;
//#include "pouct.h"
class VNode;
class State
{
public:
    State(string name_) :name(name_) {}
    State() {}

//protected:
    string name;
};
class Observation
{

};
class Belief {
public:
   virtual State* random() = 0;
};


class Action {
public:
    Action(string n) :name(n) {}
    Action() {}
    //virtual ~Action() {}
    string name;
//private:
    
};
class History {

public:
    History() :history() {}
   
    void add(Action* act, Observation* obs);
    History(list< tuple<Action*, Observation*>> _history) :history(_history) {}

private:
    //Action action;
    //Observation obs;
    list< tuple<Action*, Observation*>> history;


};

class ObservationModel {
public:
    virtual double probability(Observation* observation,
        State* next_state,
        Action* action) = 0;

    virtual Observation* sample(State* next_state,
        Action* action) = 0;

    State argmax(const State& next_state,
        const Action& action) {};
};

class TransitionModel {
public:
    //TransitionModel() {};
    virtual double probability(State* next_state,
         State* state,
        Action* action) = 0;

    virtual State* sample(State* state,
        Action* action)=0;
    //virtual State* sample(const State* state,
      //   const Action* action) =0;
    State argmax(const State& state,
        const Action& action) {};
};

class RewardModel {
public:
    double probability(double reward,
        State* state,
        Action* action, State* next_state) {};

    /*double sample(const State& state,
        const Action& action,
        const State& next_state) {};*/

    virtual double sample(State* state,
        Action* action,
        State* next_state) =0 ;
    

    double argmax(const State& state,
        const Action& action,
        const State& next_state) {};
};

class PolicyModel {
public:
    virtual double probability(Action* action,
        State* state
    ) {};

    virtual Action* sample(State* state) {};
    double argmax(const State* state) {};
    //virtual list<Action> getAllActions(State& state) = 0;
    virtual vector<Action*>* get_all_actions(State* state, History* history) {};


};

class Agent {
public:
    //VNode& tree(nullptr);
    Agent(Belief* init_belief, PolicyModel* pi, TransitionModel* T,
        ObservationModel* O, RewardModel* R)
        : belief_(init_belief), pi_(pi), T_(T), O_(O), R_(R) {
        _cur_belief = init_belief;
         hist = new History();
    }
    Agent() {}
    /*Agent(PolicyModel* pi, TransitionModel* T,
        ObservationModel* O, RewardModel* R)
        : pi_(pi), T_(T), O_(O), R_(R) {
       // _cur_belief = init_belief;
        hist = new History();
    }*/
    History* gethistory();
    void update_hist(Action* act, Observation* obs);
    Belief* init_belief();
    Belief* belief();
    Belief* cur_belief();
    void setbelief(Belief* bel, bool prior);
    State* sample_belief();
    ObservationModel* getObsModel();
    TransitionModel* getTransModel();
    RewardModel* getRewardModel();
    PolicyModel* getPolicyModel();
    //virtual void update(Action* act, Observation* obs);
    std::vector<Action*>* validActions(State* state, History* history);
    //History hist;
    //virtual ~Agent();
private:
    Belief* belief_;
    TransitionModel* T_;
    ObservationModel* O_;
    RewardModel* R_;
    PolicyModel* pi_;
    History* hist;
    Belief* _cur_belief;
};

class Environment {
public:
    Environment(State* init_state, TransitionModel* T, RewardModel* R)
        : state_(init_state), T_(T), R_(R) {}
    //Environment(State& init_state) : state_(init_state) {}
    State* getstate();
    /*{
        cout << state_;
        return state_;
    };*/
    TransitionModel* transitionmodel();
    RewardModel* reward_model();
    double state_transition(Action* action, float discount_factor = 1.0);
    tuple<State*, double> state_transition_sim(Action* action, float discount_factor = 1.0);
    void apply_transition(State* next_st);
    tuple<Observation*, double> execute(Action* act, ObservationModel* Omodel);
    Observation* provide_observation(ObservationModel* Omodel, Action* act);

private:
    State* state_;
    TransitionModel* T_;
    RewardModel* R_;
};

class Histogram : public Belief
{
private:
    std::map<State*, float> _histogram;
public:
    Histogram(std::map<State*, float> histogram) :_histogram(histogram) {}
    std::map<State*, float> getHist();
    int lenHist();
    float getitem(State* st);
    void setitem(State* st, float prob);
    bool isEq(Histogram* b);
    State* mpe();
    State* random();
    bool isNormalized(double eps);
    Histogram* update_hist_belief(Action* real_act, Observation* real_obs, ObservationModel* O, TransitionModel* T, bool normalize, bool static_transition);
    //Histogram update_hist_belief(Histogram current_hist, Action real_act, Observation real_obs, ObservationModel O, TransitionModel T, bool normalize = true, bool static_transition = false);


};
/*class POMDP {
public:
    Agent agent;
    Environment env;
    string name;
    POMDP(Agent ag_, Environment env_, string name_) :agent(ag_), env(env_), name(name_) {}
};*/

class PyObservationModel : public ObservationModel
{
public:

    // inherit the constructors
    using ObservationModel::ObservationModel;

    // trampoline (one for each virtual function)
    double probability(Observation* observation,
        State* next_state,
        Action* action) override {
        PYBIND11_OVERLOAD_PURE(
            double, /* Return type */
            ObservationModel,      /* Parent class */
            probability,        /* Name of function in C++ (must match Python name) */
            observation,      /* Argument(s) */
            next_state,
            action
        );
    }

    Observation* sample(State* next_state,
        Action* action) override {
        PYBIND11_OVERLOAD_PURE(
            Observation*,  /*Return type */
            ObservationModel,      /* Parent class */
            sample,        /* Name of function in C++ (must match Python name) */
                           /* Argument(s) */
            next_state,
            action
        );
    }
  /*  State argmax(const State& next_state,
        const Action& action) override {
        PYBIND11_OVERLOAD_PURE(
            State, // Return type 
            ObservationModel,      // Parent class 
            argmax,        // Name of function in C++ (must match Python name) 
                           // Argument(s) 
            next_state,
            action
        );
    }*/
};

class PyTransitionModel : public TransitionModel
{
public:

    // inherit the constructors
    using TransitionModel::TransitionModel;

    // trampoline (one for each virtual function)
    double probability(State* next_state,
        State* state,
        Action* action) override {
        PYBIND11_OVERLOAD_PURE(
            double, /* Return type */
            TransitionModel,      /* Parent class */
            probability,        /* Name of function in C++ (must match Python name) */
            next_state,      /* Argument(s) */
            state,
            action
        );
    }

   /* State* sample(const State* state,
         const Action* action) override {
        PYBIND11_OVERLOAD_PURE(
            State*, // Return type 
            TransitionModel,      // Parent class 
            sample,        // Name of function in C++ (must match Python name) 
                           // Argument(s) 
            state,
            action
        );
    }
    */
    State* sample(State* state,
         Action* action) override {
        PYBIND11_OVERLOAD_PURE(
            State*, // Return type 
            TransitionModel,      // Parent class 
            sample,       // Name of function in C++ (must match Python name) 
            state,
            action
        );
    }
    /*State argmax(const State& state,
        const Action& action) override {
        PYBIND11_OVERLOAD_PURE(
            State, // Return type 
            TransitionModel,      // Parent class 
            argmax,        // Name of function in C++ (must match Python name) 
                           // Argument(s) 
            state,
            action
        );
    }*/
};

class PyRewardModel : public RewardModel
{
public:

    // inherit the constructors
    using RewardModel::RewardModel;

    // trampoline (one for each virtual function)
    /*double probability(const Observation& observation,
        const State& next_state,
        const Action& action) override {
        PYBIND11_OVERLOAD_PURE(
            double, 
            ObservationModel,      
            probability,        
            observation,      
            next_state,
            action
        );
    }*/

    double sample(State* state, Action* action,
        State* next_state) override {
        PYBIND11_OVERLOAD_PURE(
            double,  /*Return type */
            RewardModel,      /* Parent class */
            sample,        /* Name of function in C++ (must match Python name) */
            state, 
            action,           /* Argument(s) */
            next_state
            
        );
    }
    /*  State argmax(const State& next_state,
          const Action& action) override {
          PYBIND11_OVERLOAD_PURE(
              State, // Return type
              ObservationModel,      // Parent class
              argmax,        // Name of function in C++ (must match Python name)
                             // Argument(s)
              next_state,
              action
          );
      }*/
};

class PyPolicyModel : public PolicyModel
{
public:

    // inherit the constructors
    using PolicyModel::PolicyModel;

    /*virtual double probability(Action* action,
        State* state
    ) {};

    virtual Action sample(State& state) = 0;
    virtual double argmax(const State& state) {};
    //virtual list<Action> getAllActions(State& state) = 0;
    virtual vector<Action*> get_all_actions(State* state, History* history) = 0;*/

    // trampoline (one for each virtual function)
    double probability(Action* action,
        State* state
        ) override {
        PYBIND11_OVERLOAD(
            double, /* Return type */
            PolicyModel,      /* Parent class */
            probability,        /* Name of function in C++ (must match Python name) */
            action,      /* Argument(s) */
            state
            
        );
    }

    Action* sample(State* state
        ) override {
        PYBIND11_OVERLOAD(
            Action*,  /*Return type */
            PolicyModel,      /* Parent class */
            sample,        /* Name of function in C++ (must match Python name) */
                           /* Argument(s) */
            state
            
        );
    }
    /*double argmax(const State* state
          ) override {
          PYBIND11_OVERLOAD(
              double, // Return type
              PolicyModel,      // Parent class
              argmax,        // Name of function in C++ (must match Python name)
                             // Argument(s)
              state
              
          );
      }*/

    vector<Action*>* get_all_actions(State* state, History* history
    ) override {
        PYBIND11_OVERLOAD(
            vector<Action*>*,  /*Return type */
            PolicyModel,      /* Parent class */
            get_all_actions,        /* Name of function in C++ (must match Python name) */
                           /* Argument(s) */
            state,
            history

        );
    }
};
class PyEnvironment : public Environment

{
public:

    // inherit the constructors
    using Environment::Environment;
};

class PyAgent : public Agent

{
public:

    // inherit the constructors
    using Agent::Agent;
};

tuple<State*, Observation*, double, int> sample_generative_model(Agent* agent, State* state, Action* action, float discount_factor=1);
tuple<State*, Observation*, double, int> sample_explict_models1(TransitionModel* T, ObservationModel* O, RewardModel* R, State* state, Action* a, float discount_factor=1);
tuple<State*, double, int> sample_explict_models(TransitionModel* T, RewardModel* R, State* state, Action* a, float discount_factor=1);
