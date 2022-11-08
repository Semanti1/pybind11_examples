from example import *
import example

import random
#print(example.talk(example.Dog(),2))
#print(example.talk(example.Cat(),3))

'''class Deer(Animal):
    def talk(self, n_times):
        //return "bark! " * n_times'''
class TigerState(State):
    def __init__(self, name):
        State.__init__(self,name)
        #self.name = name
        #super().__init__(name)
    def __hash__(self):
        return hash(self.name)
    def __eq__(self, other):
        if isinstance(other, TigerState):
            return self.name == other.name
        return False
    def __str__(self):
        return self.name
    def __repr__(self):
        return "TigerState(%s)" % self.name
    def other(self):
        if self.name.endswith("left"):
            return TigerState("tiger-right")
        else:
            return TigerState("tiger-left")

class TigerAction(Action):
    def __init__(self, name):
        Action.__init__(self,name)
        #self.name = name
    def __hash__(self):
        return hash(self.name)
    def __eq__(self, other):
        if isinstance(other, TigerAction):
            return self.name == other.name
        return False
    def __str__(self):
        return self.name
    def __repr__(self):
        return "TigerAction(%s)" % self.name
class TigerObservation(Observation):
    def __init__(self, name):
        Observation.__init__(self)
        self.name = name
    def __hash__(self):
        return hash(self.name)
    def __eq__(self, other):
        if isinstance(other, TigerObservation):
            return self.name == other.name
        return False
    def __str__(self):
        return self.name
    def __repr__(self):
        return "TigerObservation(%s)" % self.name

class TObservationModel(ObservationModel):
    def __init__(self, noise=0.15):
        ObservationModel.__init__(self)
        self.noise = noise

    def probability(self, observation, next_state, action):
        if action.name == "listen":
            # heard the correct growl
            if observation.name == next_state.name:
                return 1.0 - self.noise
            else:
                return self.noise
        else:
            return 0.5

    def sample(self, next_state, action):
        if action.name == "listen":
            thresh = 1.0 - self.noise
        else:
            thresh = 0.5
        if random.uniform(0,1) < thresh:
            return TigerObservation(next_state.name)
        else:
            return TigerObservation(next_state.other().name)

    def get_all_observations(self):
        """Only need to implement this if you're using
        a solver that needs to enumerate over the observation space
        (e.g. value iteration)"""
        return [TigerObservation(s)
                for s in {"tiger-left", "tiger-right"}]
class TTransitionModel(TransitionModel):
    
    def __init__(self):
        #pass
        TransitionModel.__init__(self)
        self.stashed_st = None
        
    def probability(self, next_state, state, action):
        """According to problem spec, the world resets once
        action is open-left/open-right. Otherwise, stays the same"""
        if action.name.startswith("open"):
            return 0.5
        else:
            if next_state.name == state.name:
                return 1.0 - 1e-9
            else:
                return 1e-9

    def sample(self, state, action):
        print("WE ARE IN")
        if action.name.startswith("open"):
            print("open")
            return random.choice(self.get_all_states())
        else:
            print("close")
            #self.stashed_st = TigerState(state.name)
            print("State name", str(state))
            return TigerState(str(state))
            #return self.stashed_st

    def get_all_states(self):
        """Only need to implement this if you're using
        a solver that needs to enumerate over the observation space (e.g. value iteration)"""
        return [TigerState(s) for s in {"tiger-left", "tiger-right"}]

class TRewardModel(RewardModel):
    def __init__(self):
        #pass
        RewardModel.__init__(self)
    def _reward_func(self, state, action):
        if action.name == "open-left":
            if state.name == "tiger-right":
                return 10
            else:
                return -100
        elif action.name == "open-right":
            if state.name == "tiger-left":
                return 10
            else:
                return -100
        else: # listen
            return -1

    def sample(self, state, action, next_state):
        # deterministic
        print("rewardmodel sample")
        return self._reward_func(state, action)

class PolicyModel(RolloutPolicy):
    """A simple policy model with uniform prior over a
       small, finite action space"""
    ACTIONS = {TigerAction(s)
              for s in {"open-left", "open-right", "listen"}}
    def __init__(self):
        #pass
        RolloutPolicy.__init__(self)

    def sample(self, state):
        return random.sample(self.get_all_actions(), 1)[0]

    def rollout(self, state, history=None):
        """Treating this PolicyModel as a rollout policy"""
        return self.sample(state)

    def get_all_actions(self, state=None, history=None):
        return PolicyModel.ACTIONS

'''class Environment(Environment):
    def __init__(self,init_true_state,T, R):
        //super().__init__(init_true_state,T, R)'''

if __name__ == "__main__":
    init_true_state = random.choice([TigerState("tiger-left"),
                                     TigerState("tiger-right")])
    print(init_true_state)
    ACTIONS = {TigerAction(s)
              for s in {"open-left", "open-right", "listen"}}
    print(ACTIONS)
    T = TTransitionModel()
    R=TRewardModel()
    OBSERVATIONS = [TigerObservation(s)
                for s in {"tiger-left", "tiger-right"}]
    print(OBSERVATIONS)

    O = TObservationModel(0.15)
    T = TTransitionModel()
    print("obs model")
    print(O.probability(TigerObservation("tiger-left"),TigerState("tiger-left"),TigerAction("listen")))
    print("sample obs")
    print(repr(O.sample(TigerState("tiger-left"),TigerAction("listen"))))
    print("transition")
    print(T.sample(TigerState("tiger-left"),TigerAction("listen")))
    R=TRewardModel()
    print("reward eaten by tiger",R.sample(TigerState("tiger-left"),TigerAction("open-left"),TigerState("tiger-left")))
    P = PolicyModel()
    print("rollout ",P.rollout(TigerState("tiger-left")))
    a = TigerAction("open-left")
    st = TigerState("tiger-left")
    env = Environment(st,T,R)
    #rew = env.state_transition(a,1)
    print(isinstance(a,Action))
    #act = Action()
    nst= TigerState("tiger-right")
    print("before transition",env.getstate())
    env.apply_transition(nst)
    tst = env.getstate()
    print(isinstance(tst,TigerState),"after transition ", tst)
    print("checking state transition fn")
    #rew = env.state_transition(TigerAction("listen"),1.0)
    print("before transition",env.getstate())
    rew = env.state_transition(a,1.0)
    print("after transition",env.getstate())
    #print("reward", rew)
    #act.name_ = "p"
    real_observation = TigerObservation(str(tst))
    print(repr(real_observation))
    init_true_state = TigerState("tiger-left")
    init_belief = Histogram({
            TigerState("tiger-left"): 0.5,
            TigerState("tiger-right"): 0.5
        })
    print('Testing histogram')
    print(init_belief.lenHist())
    print(init_belief.getitem(TigerState("tiger-left")))

    agent = Agent(init_belief,P,T,O,R)
    newenv = Environment(st,T,R)
    agent.update_hist(TigerAction("listen"),real_observation)

    print("\n** Testing POUCT **")
    act = ActionPrior
    pouct = POUCT(3,1,4096,0.9,1.4,0,0,agent.getPolicyModel(),True,5)
    #actplan = pouct.plan(agent)
    '''agent.gethistory()
    sim_obs = env.provide_observation(O,TigerAction("listen"))
    print("sim obs",repr(sim_obs))
    print("env test ",env.state_transition(act,1.0))'''
    



