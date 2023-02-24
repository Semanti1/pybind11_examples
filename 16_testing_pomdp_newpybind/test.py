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



if __name__ == "__main__":

    

    
    print("\n** Testing POUCT **")
    st1 = TigerState("tiger-left")
    st2 = TigerState("tiger-right")
    print ("st1", st1)
    init_belief = Histogram({st1: 0.5, st2: 0.5})
    P = PPolicyModel()
    O = TObservationModel(0.15)
    T = TTransitionModel()
    R = TRewardModel()
    #agent = Agent(init_belief,P,T,O,R)
    newenv = Environment(st2,T,R)
    print("len bel ", init_belief.lenHist())
    print("init bel ", init_belief.getitem(st1))
    agent = Agent(init_belief,P,T,O,R)
    print("agents belief" , agent.belief().getitem(st1))
    #agent.update_hist(TigerAction("listen"),real_observation)
    print(init_belief.getitem(st1))
    
    #act = ActionPrior
    pouct = POUCT(3,1,2000,0.9,1.4,0,0,P,True,5,agent)
    #print(init_belief.getitem(TigerState("tiger-left")))

    #h = agent.gethistory()
    #print("hist", agent.gethistory())
    #a = agent.validActions(st,h)
    #print ("valid actions ", a)
    #p = pouct.plan()
    #print("plan ", p)
    # sim_obs = env.provide_observation(O,TigerAction("listen"))
    # print("sim obs",repr(sim_obs))
    # print("env test ",env.state_transition(act,1.0))

    for i in range(10):

        action = pouct.plan()
        
        print("==== Step %d ====" % (i+1))
        #print("True state:", newenv.state)
        #print("Belief:", agent.cur_belief)
        print("Action:", action.name)
        # There is no state transition for the tiger domain.
        # In general, the ennvironment state can be transitioned
        # using
        #
        #   reward = tiger_problem.env.state_transition(action, execute=True)
        #
        # Or, it is possible that you don't have control
        # over the environment change (e.g. robot acting
        # in real world); In that case, you could skip
        # the state transition and re-estimate the state
        # (e.g. through the perception stack on the robot).
        reward = newenv.reward_model().sample(newenv.getstate(), action, None)
        print("Reward:", reward)

        # Let's create some simulated real observation;
        # Here, we use observation based on true state for sanity
        # checking solver behavior. In general, this observation
        # should be sampled from agent's observation model, as
        #
        #    real_observation = tiger_problem.agent.observation_model.sample(tiger_problem.env.state, action)
        #
        # or coming from an external source (e.g. robot sensor
        # reading). Note that tiger_problem.env.state stores the
        # environment state after action execution.
        real_observation = TigerObservation(newenv.getstate().name)
        print(">> Observation:",  real_observation)
        pouct.getAgent().update_hist(action, real_observation)
        #print(len(pouct.getAgent().gethistory().history))
        # Update the belief. If the planner is POMCP, planner.update
        # also automatically updates agent belief.
        pouct.update(action, real_observation)
        #print(len(pouct.getAgent().gethistory().history))
        new_belief = pouct.getAgent().cur_belief().update_hist_belief(
                
                action, real_observation,
                pouct.getAgent().getObsModel(),
                pouct.getAgent().getTransModel(),True,False)
        #print("belief st ", new_belief.getHist())
        #pouct.getAgent().setbelief(new_belief, True)
        agent.setbelief(new_belief, True)
        print("after bel ")
        print(new_belief.getitem(st1), new_belief.getitem(st2))
        #print("agent bel ")
        pouct.setAgent(agent)
        #pouct = POUCT(3,1,10,0.9,1.4,0,0,P,True,5,agent)
        #print(agent.belief().getitem(st1), pouct.getAgent().cur_belief().getitem(st1) )
        #print(agent.belief().getitem(st1), agent.belief().getitem(st2))
        #print("T left", pouct.getAgent().cur_belief().getitem(TigerState("tiger-left")), "T right", pouct.getAgent().cur_belief().getitem(TigerState("tiger-right")))
        if action.name.startswith("open"):
            # Make it clearer to see what actions are taken
            # until every time door is opened.
            print("\n")

    



