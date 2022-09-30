//#include "basics.h"
#include "planner.h"
#include <map>
#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
#include <list>

#include <pybind11/stl.h>
using namespace std;

class QNode {
public:
    std::map<Observation*, VNode*> children;
    int num_visits;
    float value;
    QNode(int nvisit, float val) :num_visits(nvisit), value(val) {}
    //friend class VNode;
};
class VNode {
public:
    
    VNode(int nvisit) :num_visits(nvisit) {}
    std::map<Action*, QNode*> children;
    int num_visits;
    float value;
    Action* argmax();
    int getnumvis() { return num_visits; };
    //virtual ~VNode() {}
    //friend class QNode;
};

class RootVNode :public VNode {
public:
    //tuple<Action, Observation> history;
    RootVNode(int nvis, History* hist) :VNode(nvis),history(hist)
    {

    }
    History* history;
    RootVNode from_vnode(VNode* vnode, History* hist);
    //static RootVNode from_vnode(VNode vnode);
    
};

class RolloutPolicy : public PolicyModel {
public:
    RolloutPolicy() {}
    Action* rollout(State* state, History* history) {};
};

class RandomRollout : public RolloutPolicy {
    Action* rollout(State* state, History* history)
    {
        vector<Action*>* all_actions = PolicyModel::get_all_actions(state, history);
        srand(time(NULL));
        int randnum = rand() % (*all_actions).size();
        auto ind = std::next(all_actions->begin(), randnum);
        return *ind;
    };
}; 

class ActionPrior {
public:
    //ActionPrior() {}
    virtual set<tuple<Action*, int, float>> get_preferred_actions(State* state, History* history) =0;
};


/*class PyRolloutPolicy : public RolloutPolicy
{
public:

    // inherit the constructors
    using RolloutPolicy::RolloutPolicy;

    // trampoline (one for each virtual function)
    /*set<tuple<Action*, int, float>> get_preferred_actions(State* state, History* history
        ) override {
        PYBIND11_OVERLOAD_PURE(
            set<tuple<Action*, int, float>>, 
            ActionPrior,      
            get_preferred_actions,        
            state,
            history           

        );
    }

    Action* rollout(State* state, History* history
    ) override {
        PYBIND11_OVERLOAD(
            Action*,
            RolloutPolicy,
            rollout,
            state,
            history

        );
    }

};*/
class POUCT : public Planner {
public:

    int _max_depth;
    float _planning_time;
    int _num_sims;
    int _num_visits_init;
    float _value_init;
    float _discount_factor;
    float _exploration_const;
    ActionPrior* _action_prior;
    RolloutPolicy* _rollout_policy;
    Agent* _agent;
    int _last_num_sims;
    float _last_planning_time;
    VNode* tree;
    bool _show_progress;
    int _pbar_update_interval;
    POUCT(int max_depth, float plan_time, int num_sims, float discount_factor, float exp_const, int num_visits_init, float val_init, RolloutPolicy* rollout_pol,bool show_prog, int pbar_upd_int) :  _max_depth(max_depth), _planning_time(plan_time), _num_sims(num_sims), _discount_factor(discount_factor), _exploration_const(exp_const), _num_visits_init(num_visits_init), _rollout_policy(rollout_pol), _show_progress(show_prog), _pbar_update_interval(pbar_upd_int) 
    {
        tree = nullptr;
    }


    Action* plan(Agent* agent) ;
    tuple<Action*, double, int> _search();
    double _simulate(State* state, History* history, VNode* root, QNode* parent, Observation* observation, int depth);
    //void clear_agent();
    //void _expand_vnode(VNode vnode, tuple<Action, Observation> history, State* state);
    void _expand_vnode(VNode* vnode, History* history, State* state);
    double _rollout(State* state, History* history, VNode* root, int depth);
    Action* _ucb(VNode* root);
    /*tuple<State, Observation, double> _sample_generative_model(State state, Action action); */
    VNode* _VNode(Agent* agent = NULL, bool root = false);
};


