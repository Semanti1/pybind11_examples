#include "pouct.h"
#include <chrono>
#include<cmath>
#include <iostream>
#include <map>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/embed.h>
using namespace std;
//PYBIND11_MAKE_OPAQUE(std::map<std::shared_ptr<State>, float>);
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
	//std::shared_ptr<Action> best_action;
	float best_val =  -INFINITY;
	//best_action = nullptr;
	bool it = children.empty();
	string ba;
	//cout << "HELLOO1" << " size " << children.size() << endl;
	for (auto const& x : children)
	{
		//cout << "HELLOO2" << endl;
		if (x.second->value > best_val)
		{
			//cout << "HELLOO3" << endl;
			ba = x.first;
			best_val = x.second->value;
			//cout << "" << endl;
		}
	}
	auto best_action = std::make_shared<Action>(ba);
	return best_action;
}

std::shared_ptr<RootVNode> RootVNode::from_vnode(std::shared_ptr<VNode> vnode, History hist)
{
	//RootVNode rootnode = new RootVNode(vnode.num_visits, history);
	//std::shared_ptr<RootVNode> rootnode(vnode->num_visits, hist);
	std::shared_ptr<RootVNode> rootnode = make_shared<RootVNode>(vnode->num_visits, hist);
	rootnode->children = vnode->children;
	return rootnode;
}

//std::shared_ptr<Action> POUCT::plan(std::shared_ptr<Agent> agent)
std::shared_ptr<Action> POUCT::plan()
{

	//_agent = agent;
	//cout << "hello inside plan " << _agent.sample_belief()->getname() << endl;
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
		//cout << "INSIDE WHILE";
		state = _agent.sample_belief();
		//cout << state << "STATE" << endl;
		_simulate(state, _agent.gethistory(), tree, NULL, NULL, 0);
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
	//cout << "OUT OF WHILE LOOP" << endl;
	//cout << dynamic_cast<RootVNode*>(tree)->num_visits << endl;
	//std::unordered_map<std::shared_ptr<Action>, std::shared_ptr<QNode>> a = tree->children;
	std::unordered_map<string, std::shared_ptr<QNode>> a = tree->children;
	//cout << "Tree empty" << ((*tree).children) << endl;
	best_action = tree->argmax();
	tuple<std::shared_ptr<Action>, double, int> result(best_action, time_taken, sims_count);
	//return make_tuple<best_action, time_taken, sims_count>;
	//return result;
	//tuple<std::shared_ptr<Action>, double, int> result(nullptr, 0, 0);
	return result;
}

double POUCT::_simulate(std::shared_ptr<State> state, History history, std::shared_ptr<VNode> root, std::shared_ptr<QNode> parent, std::shared_ptr<Observation> observation, int depth)
{
	if (depth > _max_depth)
	{
		return 0;
	}
	//cout << "HELLO0"<< endl;
	if (root == nullptr)
	{
		//cout << "HELLO1"<<endl;
		if (tree == nullptr)
		{

			root = _VNode(true);
			tree = root;
			//cout << "Tree" << (tree)->num_visits << endl;
			/*if (dynamic_cast<RootVNode*>(tree)->history != _agent->gethistory())
			{
				cout << "Cannot plan" <<  endl;
				return -1;
			}*/
		}
		else
		{
			root = _VNode(false);
			//cout << "root" << (root)->num_visits << endl;
		}
		if (parent != NULL)
		{
			//cout << "parent not null" << endl;
			//parent->children.insert(std::pair<std::shared_ptr<Observation>, std::shared_ptr<VNode>>(std::move(observation), root));
			//if (!parent->children.count(observation))
			{
				auto it = parent->children.find(std::move(observation->name));
					if (it != parent->children.end())
						it->second = root;
				
			}
		}
		//cout << "CALLING EXPAND VNODE" << endl;
		_expand_vnode(root, history, state);
		//cout << "AFTER CALLING EXPAND VNODE" << endl;
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
	history.add(action, obs);
	double total_reward = rd + pow(_discount_factor,nsteps) * _simulate(next_state, history, root->children[action->name]->children[obs->name], root->children[action->name], obs, depth + nsteps);
	root->num_visits += 1;
	root->children[action->name]->num_visits += 1;
	root->children[action->name]->value = root->children[action->name]->value + (total_reward - (root->children[action->name]->value)) / (root->children[action->name]->num_visits);
	return total_reward;
	//return 0;
}

//std::shared_ptr<VNode> POUCT::_VNode(std::shared_ptr<Agent> agent, bool root)
std::shared_ptr<VNode> POUCT::_VNode(bool root)
{
	if (root)
	{

		//std::shared_ptr<RootVNode> trvnode(new RootVNode(_num_visits_init, _agent.gethistory()));
		return std::move(std::make_shared<RootVNode>(_num_visits_init, _agent.gethistory()));
		//return trvnode;
	}
	else
	{
		//std::shared_ptr<VNode> shared_vnode(new VNode(_num_visits_init));
		//return shared_vnode;
		return std::move(std::make_shared<VNode>(_num_visits_init));
	}
}
void POUCT::_expand_vnode(std::shared_ptr<VNode> vnode, History history, std::shared_ptr<State> state)
{
	
	
	//vector<std::shared_ptr<Action>>* actlist = (_agent->validActions(state, history));
	auto actlist = _agent.getTransModel();
	//cout << "hello13254654" << endl;
	int ct = 0;
	//cout << "hello there" << endl;
	//cout <<  _agent.validActions(state, history).empty() << endl;//((_agent->validActions(state, history)).empty()) << endl;
	//cout << "after hello there " << endl;
	
	for (auto const& ptr : (_agent.validActions(state, history)))
		//for (vector<std::shared_ptr<Action>>::iterator ptr = actlist.begin(), itr_end = actlist.end(); ptr != itr_end; ++ptr)
	{
		//cout << "name act " << ptr->name << endl;
		ct = ct + 1;
		if (!vnode->children.count(ptr->name))
			//if ((*iter)->bar() == true)
			//actlist(ptr);
			//actlist.push_back(std::move(ptr));
		{
			//cout << "adding qnode " << endl;
			//cout << "g" << endl << typeid(ptr).name()<<endl;
			 //std::shared_ptr<QNode> shared_qnode = std::make_shared<QNode>(_num_visits_init, _value_init);
			//std::map<std::shared_ptr<Action>, std::shared_ptr<QNode>>::iterator it = vnode->children.begin();
			
			//vnode->children.insert(std::pair<std::shared_ptr<Action>, std::shared_ptr<QNode>>(std::move(ptr), std::make_shared<QNode>(_num_visits_init, _value_init)));
			//vnode->children[*&ptr] = std::make_shared<QNode>(_num_visits_init, _value_init);
			 vnode->children[ptr->name] = std::move(std::make_shared<QNode>(_num_visits_init, _value_init));
			//cout << "hello " <<  endl;
			//cout << "count " << vnode->children.count(ptr) << endl;
		//	vnode->children[std::move(ptr)] = std::move(std::make_shared<QNode>(_num_visits_init, _value_init));
		}
		else
		{
			continue;
		}
	}
	
	
	/*actlist.resize((_agent->validActions(state, history)).size());
	actlist = _agent->validActions(state, history);*/
	//cout << "type" << (_agent->validActions(state, history))->size();*/
	//cout << _agent->validActions(state, history) << endl;
	//cout << "size vect " << (*(actlist.begin())).get() << endl;
	//cout << "Agent" << (actlist[0]->name) << endl;
	/*int ct = 0;
	for (auto  it : _agent->validActions(state, history)) {
		cout << "ct" << ct;
		cout << "act name " << (*it).getname() << endl;
		ct = ct + 1;
	//for (vector<std::shared_ptr<Action>>::iterator itr = actlist.begin(), itr_end = actlist.end(); itr != itr_end; ++itr)
		/*if (!vnode->children.count(it))
		{
			//cout << "act name "<<(*it).getname()<< endl;

			//std::shared_ptr<QNode> shared_qnode(new QNode(_num_visits_init, _value_init));

			// QNode* newqnode = new QNode(_num_visits_init, _value_init);
			//vnode->children[(*action)] = new QNode(_num_visits_init, _value_init);
			//vnode->children[action] = newqnode;
			// vnode->children[it] = newqnode;
			vnode->children[std::move(it)] = std::move(std::make_shared<QNode>(_num_visits_init, _value_init));
		}*/
		//cout << "act name" << it.get()->getname() << endl;
	//}
	//cout << "ct " << ct;
	//cout << "num child" << (vnode->children).size()<<endl;
}

double POUCT::_rollout(std::shared_ptr<State> state, History history, std::shared_ptr<VNode> root, int depth)
{
	std::shared_ptr<Action> action;
	float discount = 1.0;
	float total_discounted_reward = 0;
	std::shared_ptr<State> next_state;
	std::shared_ptr<Observation> observation;
	float reward;

	while (depth < _max_depth)
	{
		action = _rollout_policy->rollout(state.get(), history);
		tuple<std::shared_ptr<State>, std::shared_ptr<Observation>, double, int> generated = sample_generative_model(_agent, state, action);
		next_state = get<0>(generated);
		observation = get<1>(generated);
		reward = get<2>(generated);
		int nsteps = get<3>(generated);
		history.add(action, observation);
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
	float best_value = -1000000000;
	double val;
	//std::unordered_map<std::shared_ptr<Action>, std::shared_ptr<QNode>> rtchld(root->children);
	std::unordered_map<string, std::shared_ptr<QNode>> rtchld(root->children);
	for (const auto& i : root->children)
	{
		//std::shared_ptr<Action> act = i.first;
		//std::shared_ptr<Action> act;
		//act->name = i.first;
		auto act = std::make_shared<Action>(i.first);
		if (root->children[act->name]->num_visits == 0)
		{
			val = 1000000000;
		}
		else
		{
			val = (root->children[act->name]->value) + _exploration_const * sqrt(log(root->num_visits + 1) / root->children[act->name]->num_visits);
		}
		if (val > best_value)
		{
			best_action = act;
			best_value = val;
		}
		//best_action = act;
	}
	//cout << "best action " << best_action->name << endl;
	return best_action;
	//return nullptr;

}

void POUCT::update(std::shared_ptr<Action> real_action, std::shared_ptr<Observation> real_observation)
{
	if (tree == nullptr)
	{
		cout << "No tree. Have you planned yet?" << endl;

	}
	else

	{
		int ct = 0;
		for (const auto& elem : tree->children)
		{
			//std::cout << "first " << elem.first << " " << "\n";
			//std::cout << "real act " << real_action->name << " " << (elem.first==real_action->name) << "\n";
			//cout << "first clause " << (tree->children.find(real_action->name) == tree->children.end()) << endl;
			//cout << "real obs " << real_observation->name << endl;
			//cout << "clause 2 " << (elem.second->children) << endl;
			//cout << "entering nested" << endl;
			
			/*for (auto it = (elem.second->children).begin(); it != (elem.second->children).end(); it++) {
				//cout << it->first->name << endl;
				
			}*/
			//cout << "out of nested nested " << endl;
			/*for (const auto& elem2 : elem.second->children)
			{
				ct = ct + 1;
				//std::cout << "second " << elem2.first->name << " " << "\n";
			}*/

		}
		/*if ((tree->children.find(real_action) == tree->children.end()) || tree->children[real_action]->children.find(real_observation) == tree->children[real_action]->children.end())
		{
			tree = nullptr;
			cout << "null ptr " << endl;
		}*/
		auto test = tree->children;
		
		//for (auto& elem : test[real_action]->children)
		{
			//ct = ct + 1;
			//std::cout << "second " << elem.first->name << " " << "\n";
		}
		//cout << "out of nested nested ct" << ct << endl;
		if (test[real_action->name]->children[real_observation->name])
		{
			tree = RootVNode::from_vnode(
				tree->children[real_action->name]->children[real_observation->name],
				_agent.gethistory());

			cout << "pruned " << endl;
		}
		else
		{
			cout << "Unexpected state; child should not be None" << endl;
		}
	}
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