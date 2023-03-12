"""Policy model for 2D Multi-Object Search domain. 
It is optional for the agent to be equipped with an occupancy
grid map of the environment.
"""

import example
from example import *
import random
from objectsearch.domain.action import *

class PolicyModel(RolloutPolicy):
    """Simple policy model. All actions are possible at any state."""

    def __init__(self, robot_id, grid_map=None):
        """FindAction can only be taken after LookAction"""
        self.robot_id = robot_id
        self._grid_map = grid_map
        RolloutPolicy.__init__(self)

    def sample(self, state, **kwargs):
        return random.sample(self._get_all_actions(**kwargs), 1)[0]
    
    def probability(self, action, state, **kwargs):
        raise NotImplementedError

    def argmax(self, state, **kwargs):
        """Returns the most likely action"""
        raise NotImplementedError

    def get_all_actions(self, state=None, history=None):
        """note: find can only happen after look."""
        can_find = False
        #print("history",history.getHist())
        if history is not None and len(history.getHist()) > 1:
            # last action
            last_action = history.getHist()[-1][0]
            if isinstance(last_action, LookAction):
                can_find = True
        find_action = set({Find}) if can_find else set({})
        if state is None:
            return ALL_MOTION_ACTIONS | {Look} | find_action
        else:
            if self._grid_map is not None:
                valid_motions =\
                    self._grid_map.valid_motions(self.robot_id,
                                                 state.pose(self.robot_id),
                                                 ALL_MOTION_ACTIONS)
                return list(valid_motions | {Look} | find_action)
            else:
                return list(ALL_MOTION_ACTIONS | {Look} | find_action)

    def rollout(self, state, history=None):
        return random.sample(self.get_all_actions(state=state, history=history), 1)[0]
