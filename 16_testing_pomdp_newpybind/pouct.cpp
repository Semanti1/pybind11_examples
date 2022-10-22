#include "pouct.h"
#include <chrono>
#include<cmath>
#include <iostream>
#include <map>
using namespace std;

/*QNode::QNode(int nvisit, float val)
{
	num_visits = nvisit;
	value = val;
}
VNode::VNode(int nvisit)
{
	num_visits = nvisit;

}
RootVNode::RootVNode(int nvisit, History hist) :VNode(nvisit)
{
	history = hist;
	num_visits = nvisit;
}*/
std::shared_ptr<Action> VNode::argmax()
{
	std::shared_ptr<Action> action;
	std::shared_ptr<Action> best_action;
	float best_val = 0;// -INFINITY;
	//best_action = nullptr;
	bool it = children.empty();
	cout << "HELLOO1" << " size " << children.size() << endl;
	for (auto const& x : children)
	{
		cout << "HELLOO2" << endl;
		if (x.second->value > best_val)
		{
			cout << "HELLOO3" << endl;
			best_action = x.first;
			best_val = x.second->value;
			cout << "HELLOO4" << endl;
		}
	}
	return best_action;
}

RootVNode RootVNode::from_vnode(std::shared_ptr<VNode> vnode, std::shared_ptr<History> hist)
{
	//RootVNode rootnode = new RootVNode(vnode.num_visits, history);
	RootVNode rootnode(vnode->num_visits, hist);
	rootnode.children = vnode->children;
	return rootnode;
}

std::shared_ptr<Action> POUCT::plan(std::shared_ptr<Agent> agent)
{

	_agent = agent;
	tuple<std::shared_ptr<Action>, float, int> planned_act = _search();
	_last_num_sims = get<2>(planned_act);
	_last_planning_time = get<1>(planned_act);

	return get<0>(planned_act);


}

tuple<std::shared_ptr<Action>, double, int> POUCT::_search()
{
	std::chrono::time_point<std::chrono::system_clock> start_time;
	std::shared_ptr<State> state;
	std::shared_ptr<Action> best_action;
	
	int sims_count = 0;
	double time_taken = 0;
	bool stop_by_sims = _num_sims > 0 ? true : false;
	start_time = std::chrono::system_clock::now();
	while (true)
	{
		state = _agent->sample_belief();
		cout << state << "STATE" << endl;
		_simulate(state, _agent->gethistory(), tree, NULL, NULL, 0);
		sims_count += 1;
		std::chrono::duration<double> duration = std::chrono::system_clock::now() - start_time;
		time_taken = duration.count();
		if (stop_by_sims)
		{
			if (sims_count >= _num_sims)
				break;
		}
		else
		{
			if (time_taken > _planning_time)
				break;
		}
	}
	cout << "OUT OF WHILE LOOP" << endl;
	//cout << dynamic_cast<RootVNode*>(tree)->num_visits << endl;
	std::map<std::shared_ptr<Action>, std::shared_ptr<QNode>> a = tree->children;
	//cout << "Tree empty" << ((*tree).children) << endl;
	//best_action = tree->argmax();
	//tuple<Action*, double, int> result(best_action, time_taken, sims_count);
	//return make_tuple<best_action, time_taken, sims_count>;
	//return result;
	tuple<std::shared_ptr<Action>, double, int> result(nullptr, 0, 0);
	return result;
}

double POUCT::_simulate(std::shared_ptr<State> state, std::shared_ptr<History> history, std::shared_ptr<VNode> root, std::shared_ptr<QNode> parent, std::shared_ptr<Observation> observation, int depth)
{
	if (depth > _max_depth)
	{
		return 0;
	}
	cout << "HELLO0"<< endl;
	if (root == nullptr)
	{
		//cout << "HELLO1"<<endl;
		if (tree == nullptr)
		{

			root = _VNode(_agent, true);
			tree = root;
			cout << "Tree" << (tree)->num_visits << endl;
			/*if (dynamic_cast<RootVNode*>(tree)->history != _agent->gethistory())
			{
				cout << "Cannot plan" <<  endl;
				return -1;
			}*/
		}
		else
		{
			root = _VNode();
			cout << "root" << (root)->num_visits << endl;
		}
		if (parent != NULL)
		{
			parent->children[observation] = root;
		}
		_expand_vnode(root, history, state);
		double rollout_reward = _rollout(state, history, root, depth);
		return rollout_reward;
	}
	int nsteps;
	std::shared_ptr<Action> action = _ucb(root);
	tuple<std::shared_ptr<State>, std::shared_ptr<Observation>, double, int> generated = sample_generative_model(_agent, state, action);
	std::shared_ptr<State> next_state = get<0>(generated);
	std::shared_ptr<Observation> obs = get<1>(generated);
	double rd = get<2>(generated);
	nsteps = get<3>(generated);
	if (nsteps == 0)
		return rd;
	history->add(action, obs);
	double total_reward = rd + pow(_discount_factor,nsteps) * _simulate(next_state, history, root->children[action]->children[obs], root->children[action], obs, depth + nsteps);
	root->num_visits += 1;
	root->children[action]->num_visits += 1;
	root->children[action]->value = root->children[action]->value + (total_reward - (root->children[action]->value)) / (root->children[action]->num_visits);
	return total_reward;
	//return 0;
}

std::shared_ptr<VNode> POUCT::_VNode(std::shared_ptr<Agent> agent, bool root)
{
	if (root)
	{

		std::shared_ptr<RootVNode> trvnode(new RootVNode(_num_visits_init, agent->gethistory()));
		return trvnode;
	}
	else
	{
		std::shared_ptr<VNode> shared_vnode(new VNode(_num_visits_init));
		return shared_vnode;
	}
}
void POUCT::_expand_vnode(std::shared_ptr<VNode> vnode, std::shared_ptr<History> history, std::shared_ptr<State> state)
{
	 vector<std::shared_ptr<Action>>* actlist = _agent->validActions(state, history);
	cout << "size vect " << int((*(actlist)).size()) << endl;
	//cout << "Agent" << (*actlist.begin())->name << endl;
	for (auto it : *actlist) {
		if (!vnode->children.count(it))
		{
			cout << "act name"<<it->name << endl;

			std::shared_ptr<QNode> shared_qnode(new QNode(_num_visits_init, _value_init));

			// QNode* newqnode = new QNode(_num_visits_init, _value_init);
			//vnode->children[(*action)] = new QNode(_num_visits_init, _value_init);
			//vnode->children[action] = newqnode;
			// vnode->children[it] = newqnode;
			vnode->children[it] = shared_qnode;
		}

	}
	cout << "num child" << (vnode->children).size()<<endl;
}

double POUCT::_rollout(std::shared_ptr<State> state, std::shared_ptr<History> history, std::shared_ptr<VNode> root, int depth)
{
	std::shared_ptr<Action> action;
	float discount = 1.0;
	float total_discounted_reward = 0;
	std::shared_ptr<State> next_state;
	std::shared_ptr<Observation> observation;
	float reward;

	while (depth < _max_depth)
	{
		action = _rollout_policy->rollout(state, history);
		tuple<std::shared_ptr<State>, std::shared_ptr<Observation>, double, int> generated = sample_generative_model(_agent, state, action);
		next_state = get<0>(generated);
		observation = get<1>(generated);
		reward = get<2>(generated);
		int nsteps = get<3>(generated);
		history->add(action, observation);
		depth += nsteps;
		total_discounted_reward += reward * discount;
		discount *= pow(_discount_factor , nsteps);
		state = next_state;


	}
	return total_discounted_reward;
}

std::shared_ptr<Action> POUCT::_ucb(std::shared_ptr<VNode> root)
{
	std::shared_ptr<Action> best_action;
	float best_value = -INFINITY;
	double val;
	std::map<std::shared_ptr<Action>, std::shared_ptr<QNode>> rtchld(root->children);
	for (auto const& i : root->children)
	{
		std::shared_ptr<Action> act = i.first;
		if (root->children[act]->num_visits == 0)
		{
			val = INFINITY;
		}
		else
		{
			val = (root->children[act]->value) + _exploration_const * sqrt(log(root->num_visits + 1) / root->children[act]->num_visits);
		}
		if (val > best_value)
		{
			best_action = act;
			best_value = val;
		}
		best_action = act;
	}
	return best_action;
	//return nullptr;

}

/*void POUCT::_expand_vnode(VNode* vnode, History* history, State* state)
{

	for (auto const& action : _agent->validActions(state, history)) {
		if (!vnode->children.count(action))
		{
			//QNode newqnode = new QNode(_num_visits_init, _value_init);
			vnode->children[action] = new QNode(_num_visits_init, _value_init);
		}

	}
}*/