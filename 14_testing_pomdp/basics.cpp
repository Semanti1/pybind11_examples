#include "basics.h"
#include <iostream>>
#include<typeinfo>>
#include <set>
#include <algorithm>
#include <map>
#include <pybind11/stl.h>
using namespace std;

/*Histogram::Histogram(std::map<State*, float> histogram)
{
	_histogram = histogram;
}*/

std::map<State*, float> Histogram::getHist()
{
	return _histogram;
}
int Histogram::lenHist()
{
	return _histogram.size();
}
float Histogram::getitem(State* st)
{
	return _histogram[st];
}
void Histogram::setitem(State* st, float prob)
{
	_histogram[st] = prob;
}
bool Histogram::isEq(Histogram* b)
{
	return (_histogram == ( b->getHist()));
}
State* Histogram::mpe()
{
	//From here: https://stackoverflow.com/questions/30611709/find-element-with-max-value-from-stdmap

	auto x = std::max_element((_histogram).begin(), (_histogram).end(),
		[](const pair<State*, float>& p1, const pair<State*, float>& p2) {
			return p1.second < p2.second; });
	return x->first;
}
State* Histogram::random()
{
	auto sel = _histogram.begin();
	std::advance(sel, rand() % _histogram.size());
	State* random_key = sel->first;
	return random_key;
}
bool Histogram::isNormalized(double eps = 1e-9)
{
	float prob_sum = 0;
	for (auto const& x : _histogram)
	{
		prob_sum += x.second;
	}
	if (abs(1 - prob_sum) < eps)
		return true;
	else
		return false;
}

Histogram* Histogram::update_hist_belief(Action* real_act, Observation* real_obs, ObservationModel* O, TransitionModel* T, bool normalize = true, bool static_transition = false)
{
	std::map<State*, float> new_histogram;
	double total_prob = 0;
	for (auto const& next_state : _histogram)
	{
		double obs_prob = O->probability(real_obs, next_state.first, real_act);
		double trans_prob = 0;
		if (!static_transition)
		{

			for (auto const& state : _histogram)
			{
				trans_prob += T->probability(next_state.first, state.first, real_act) * getitem(state.first);
			}
		}
		else
		{
			trans_prob = getitem(next_state.first);
		}
		new_histogram[next_state.first] = obs_prob * trans_prob;
		total_prob += new_histogram[next_state.first];
	}
	if (normalize)
	{
		for (auto const& state : new_histogram)
		{
			if (total_prob > 0)
			{
				new_histogram[state.first] /= total_prob;
			}
		}
	}
	Histogram* updatedhist = new  Histogram(new_histogram);
	return updatedhist;
	//return Histogram(new_histogram);
}


History* Agent::gethistory()
{
	//cout << hist->history << endl;
	return hist;
}
void History::add(Action* act, Observation* obs)
{
	tuple<Action*, Observation*> newpair;
	newpair = make_tuple(act, obs);
	history.push_back(newpair);
	//cout << get<0>(history.front()) << endl;
}
void Agent::update_hist(Action* act, Observation* obs)
{
	hist->add(act, obs);
}

Belief* Agent::init_belief()
{
	return belief_;
}

Belief* Agent::belief()
{
	return _cur_belief;
}

Belief* Agent::cur_belief()
{
	return 	_cur_belief;
	
}

void Agent::setbelief(Belief* bel, bool prior)
{
	_cur_belief = bel;
	if (prior)
	{
		belief_ = bel;
	}
}

State* Agent::sample_belief()
{
	//return State();
	return _cur_belief->random();
}

ObservationModel* Agent::getObsModel()
{
	return O_;
}

TransitionModel* Agent::getTransModel()
{
	return T_;
}

RewardModel* Agent::getRewardModel()
{
	return R_;
}

PolicyModel* Agent::getPolicyModel()
{
	return pi_;
}

/*void Agent::update(Action* act, Observation* obs)
{
}*/

vector<Action*>* Agent::validActions(State* state, History* history)
{
	return pi_->get_all_actions(state, history);
}

/*State& Environment::state()
{
	return state_;
}
*/
State* Environment::getstate() {
	//cout << "HELLOOOOOO " <<endl;
	return state_;
}
TransitionModel* Environment::transitionmodel()
{
	return T_;
}

RewardModel* Environment::reward_model()
{
	return R_;
}

double Environment::state_transition(Action* action, float discount_factor)
{
	tuple<State*, double, int> result;
	TransitionModel* Tr = transitionmodel();
	RewardModel* Re = reward_model();
	State* st = getstate();

	//State* sampledst = Tr->sample(st, action);
	//State& st = state_;
	//result = sample_explict_models(transitionmodel(), reward_model(), getstate(), action, discount_factor);
	result = sample_explict_models(Tr,Re, st, action, discount_factor);
	apply_transition(get<0>(result));
	return get<1>(result);
	//return sampledst;
}

tuple<State*, double> Environment::state_transition_sim(Action* action, float discount_factor)
{
	tuple<State*, double, int> result;
	result = sample_explict_models(transitionmodel(), reward_model(), state_, action, discount_factor);
	tuple <State*, double> retRes(get<0>(result), get<1>(result));
	return retRes;
}

void Environment::apply_transition(State* next_st)
{
	//cout << "State now " << state_ << "next state : " << next_st << endl;
	//State* temp = next_st;
	//state_= next_st;
	//state_ = *temp;
	state_ = next_st;
	
}

tuple<Observation*, double> Environment::execute(Action* act, ObservationModel* Omodel)
{
	double reward = state_transition(act);
	Observation* obs = provide_observation(Omodel, act);
	tuple<Observation*, double> result(obs, reward);
	return result;
}

Observation* Environment::provide_observation(ObservationModel* Omodel, Action* act)
{
	return Omodel->sample(getstate(), act);
}

tuple<State*, Observation*, double, int> sample_generative_model(Agent* agent, State* state, Action* action, float discount_factor)
{

	tuple<State*, Observation*, double, int> result;
	result = sample_explict_models1(agent->getTransModel(), agent->getObsModel(), agent->getRewardModel(), state, action, discount_factor);
	return result;
}

tuple<State*, Observation*, double, int> sample_explict_models1(TransitionModel* T, ObservationModel* O, RewardModel* R, State* state, Action* a, float discount_factor)
{
	int nsteps = 0;
	State* next_st = T->sample(state, a);
	cout << "Done transitioning to ns" << endl;
	double reward = R->sample(state, a, next_st);
	nsteps += 1;
	Observation* obs = O->sample(next_st, a);
	tuple<State*, Observation*, double, int> res(next_st, obs, reward, nsteps);
	return res;


}

tuple<State*, double, int> sample_explict_models(TransitionModel* T, RewardModel* R, State* state, Action* a, float discount_factor)
{
	int nsteps = 0;
	//State next_st = T.sample(&(*state), a);
	State* next_st = T->sample(state, a);
	cout << "next st sampled" << endl;
	double reward = R->sample(state, a, next_st);
	cout << "Reward " << reward << endl;
	nsteps += 1;
	tuple<State*, double, int> res(next_st, reward, nsteps);
	return res;
	//tuple<State*, double, int> res(next_st, 0, nsteps);
	//return res;
}
/*
State ObservationModel::argmax(const State& next_state, const Action& action)
{
	//return State();
}

State TransitionModel::argmax(const State& state, const Action& action)
{
	//return State();
}*/
