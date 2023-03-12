#include "oopomdp.h"
#include <chrono>
#include<cmath>
#include <iostream>
#include <map>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/embed.h>
using namespace std;

double OOTransitionModel::probability(State* next_state, State* state, Action* action)
{
	double trans_prob = 1.0;
	for (auto const& pair : transition_models)
	{
		std::shared_ptr<ObjectState> ostate = state->object_states[pair.first];
		std::shared_ptr<ObjectState> nextostate = next_state->object_states[pair.first];
		trans_prob = trans_prob * transition_models[pair.first]->probability(next_object_state.get(), state.get(), action.get());

	}
	return trans_prob;
}