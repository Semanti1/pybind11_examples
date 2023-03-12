from example import *
import example

import random

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
        self.name = name
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
        Observation.__init__(self,name)
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

    def probability(self, observation, next_state, action, next_robot_state):
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
        #self.stashed_st = None
        
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
        #print("WE ARE IN")
        if action.name.startswith("open"):
            #print("open")
            return random.choice(self.get_all_states())
        else:
            #print("close")
            #self.stashed_st = TigerState(state.name)
            #print("State name", str(state))
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
        #print("rewardmodel sample")
        return self._reward_func(state, action)

class PPolicyModel(RolloutPolicy):
#class PolicyModel(PolicyModel):
    """A simple policy model with uniform prior over a
       small, finite action space"""
    #ACTIONS = [TigerAction(s)
     #         for s in {"open-left", "open-right", "listen"}]
    def __init__(self):
        #pass
        RolloutPolicy.__init__(self)
        #PolicyModel.__init__(self)

    def sample(self, state):
        return random.sample(self.get_all_actions(), 1)[0]

    def rollout(self, state, history=None):
        """Treating this PolicyModel as a rollout policy"""
        return self.sample(state)

    def get_all_actions(self, state=None, history=None):
        #print("INSIDE PYTHON POLICYMODEL")
        #print(list(ACTIONS))
        a =  [TigerAction(s)
              for s in {"open-left", "open-right", "listen"}]
        #print(a)
        return a
        #return PPolicyModel.ACTIONS
        #return list(ACTIONS)
