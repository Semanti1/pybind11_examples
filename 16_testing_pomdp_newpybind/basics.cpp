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

std::map<std::shared_ptr<State>, float> Histogram::getHist()
{
	return _histogram;
}
int Histogram::lenHist()
{
	return _histogram.size();
}
float Histogram::getitem(std::shared_ptr<State> st)
{
	return _histogram[st];
}
void Histogram::setitem(std::shared_ptr<State> st, float prob)
{
	_histogram[st] = prob;
}
bool Histogram::isEq(std::shared_ptr<Histogram> b)
{
	return (_histogram == ( b->getHist()));
}
std::shared_ptr<State> Histogram::mpe()
{
	//From here: https://stackoverflow.com/questions/30611709/find-element-with-max-value-from-stdmap

	auto x = std::max_element((_histogram).begin(), (_histogram).end(),
		[](const pair<std::shared_ptr<State>, float>& p1, const pair<std::shared_ptr<State>, float>& p2) {
			return p1.second < p2.second; });
	return x->first;
}
std::shared_ptr<State> Histogram::random()
{
	auto sel = _histogram.begin();
	std::advance(sel, rand() % _histogram.size());
	std::shared_ptr<State> random_key = sel->first;
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

std::shared_ptr<Histogram> Histogram::update_hist_belief(std::shared_ptr<Action> real_act, std::shared_ptr<Observation> real_obs, std::shared_ptr<ObservationModel> O, std::shared_ptr<TransitionModel> T, bool normalize = true, bool static_transition = false)
{
	std::map<std::shared_ptr<State>, float> new_histogram;
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
	std::shared_ptr<Histogram> updatedhist(new Histogram(new_histogram));
	// std::shared_ptr<Histogram> updatedhist = std::shared_ptr<Histogram> historgam(new Histogram(new_histogram));
	return updatedhist;
	// return Histogram(new_histogram);
}


std::shared_ptr<History> Agent::gethistory()
{
	//cout << hist->history << endl;
	std::shared_ptr<History> sharedHist(hist);
	return sharedHist;
}
void History::add(std::shared_ptr<Action> act, std::shared_ptr<Observation> obs)
{
	tuple<std::shared_ptr<Action>, std::shared_ptr<Observation>> newpair;
	newpair = make_tuple(act, obs);
	history.push_back(newpair);
	//cout << get<0>(history.front()) << endl;
}
void Agent::update_hist(std::shared_ptr<Action> act, std::shared_ptr<Observation> obs)
{
	hist->add(act, obs);
}

std::shared_ptr<Belief> Agent::init_belief()
{
	return belief_;
}

std::shared_ptr<Belief> Agent::belief()
{
	return _cur_belief;
}

std::shared_ptr<Belief> Agent::cur_belief()
{
	return 	_cur_belief;
	
}

void Agent::setbelief(std::shared_ptr<Belief> bel, bool prior)
{
	_cur_belief = bel;
	if (prior)
	{
		belief_ = bel;
	}
}

std::shared_ptr<State> Agent::sample_belief()
{
	//return State();
	return _cur_belief->random();
}

std::shared_ptr<ObservationModel> Agent::getObsModel()
{
	return O_;
}

std::shared_ptr<TransitionModel> Agent::getTransModel()
{
	return T_;
}

std::shared_ptr<RewardModel> Agent::getRewardModel()
{
	return R_;
}

std::shared_ptr<PolicyModel> Agent::getPolicyModel()
{
	return pi_;
}

/*void Agent::update(Action* act, Observation* obs)
{
}*/

vector<std::shared_ptr<Action>>* Agent::validActions(std::shared_ptr<State> state, std::shared_ptr<History> history)
{
	return pi_->get_all_actions(state, history);
}

/*State& Environment::state()
{
	return state_;
}
*/
std::shared_ptr<State> Environment::getstate() {
	//cout << "HELLOOOOOO " <<endl;
	return state_;
}
std::shared_ptr<TransitionModel> Environment::transitionmodel()
{
	return T_;
}

std::shared_ptr<RewardModel> Environment::reward_model()
{
	return R_;
}

double Environment::state_transition(std::shared_ptr<Action> action, float discount_factor)
{
	tuple<std::shared_ptr<State>, double, int> result;
	std::shared_ptr<TransitionModel> Tr = transitionmodel();
	std::shared_ptr<RewardModel> Re = reward_model();
	std::shared_ptr<State> st = getstate();

	//State* sampledst = Tr->sample(st, action);
	//State& st = state_;
	//result = sample_explict_models(transitionmodel(), reward_model(), getstate(), action, discount_factor);
	
	cout << "B4 SAMPLE explicit" << endl;
	result = sample_explict_models(Tr,Re, st, action, discount_factor);
	cout << "After SAMPLE explicit" << endl;
	apply_transition(get<0>(result));
	return get<1>(result);
	//return sampledst;
}

tuple<std::shared_ptr<State>, double> Environment::state_transition_sim(std::shared_ptr<Action> action, float discount_factor)
{
	tuple<std::shared_ptr<State>, double, int> result;
	result = sample_explict_models(transitionmodel(), reward_model(), state_, action, discount_factor);
	tuple <std::shared_ptr<State>, double> retRes(get<0>(result), get<1>(result));
	return retRes;
}

void Environment::apply_transition(std::shared_ptr<State> next_st)
{
	//cout << "State now " << state_ << "next state : " << next_st << endl;
	//State* temp = next_st;
	//state_= next_st;
	//state_ = *temp;
	state_ = next_st;
	
}

tuple<std::shared_ptr<Observation>, double> Environment::execute(std::shared_ptr<Action> act, std::shared_ptr<ObservationModel> Omodel)
{
	double reward = state_transition(act);
	std::shared_ptr<Observation> obs = provide_observation(Omodel, act);
	tuple<std::shared_ptr<Observation>, double> result(obs, reward);
	return result;
}

std::shared_ptr<Observation> Environment::provide_observation(std::shared_ptr<ObservationModel> Omodel, std::shared_ptr<Action> act)
{
	//return Omodel->sample(getstate(), act);
}

tuple<std::shared_ptr<State>, std::shared_ptr<Observation>, double, int> sample_generative_model(std::shared_ptr<Agent> agent, std::shared_ptr<State> state, std::shared_ptr<Action> action, float discount_factor)
{

	tuple<std::shared_ptr<State>, std::shared_ptr<Observation>, double, int> result;
	//result = sample_explict_models1(agent->getTransModel(), agent->getObsModel(), agent->getRewardModel(), state, action, discount_factor);
	return result;
}

/*tuple<State*, Observation*, double, int> sample_explict_models1(TransitionModel* T, ObservationModel* O, RewardModel* R, State* state, Action* a, float discount_factor)
{
	int nsteps = 0;
	std::shared_ptr<State> next_st = T->sample(state, a);
	cout << "Done transitioning to ns" << endl;
	double reward = R->sample(state, a, next_st);
	nsteps += 1;
	Observation* obs = O->sample(next_st, a);
	tuple<State*, Observation*, double, int> res(next_st, obs, reward, nsteps);
	return res;


}*/

tuple<std::shared_ptr<State>, double, int> sample_explict_models(std::shared_ptr<TransitionModel> T, std::shared_ptr<RewardModel> R, std::shared_ptr<State> state, std::shared_ptr<Action> a, float discount_factor)
{
	int nsteps = 0;
	//State next_st = T.sample(&(*state), a);
	//std::shared_ptr<State> next_st = T->sample(state, a);
	std::shared_ptr<State> next_st(T->sample(state, a));
	cout << "next st sampled 1233333 " << typeid(next_st).name() << " st " << typeid(state).name()<< endl;
	double reward = R->sample(state.get(), a.get(), next_st.get());
	cout << "Reward " << reward << endl;
	nsteps += 1;
	tuple<std::shared_ptr<State>, double, int> res(next_st, reward, nsteps);
	return res;
	//return null;
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
