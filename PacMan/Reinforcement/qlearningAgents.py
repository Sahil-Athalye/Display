# qlearningAgents.py
# ------------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


from game import *
from learningAgents import ReinforcementAgent
from featureExtractors import *

import random,util,math

class QLearningAgent(ReinforcementAgent):
    """
      Q-Learning Agent

      Functions you should fill in:
        - computeValueFromQValues
        - computeActionFromQValues
        - getQValue
        - getAction
        - update

      Instance variables you have access to
        - self.epsilon (exploration prob)
        - self.alpha (learning rate)
        - self.discount (discount rate)

      Functions you should use
        - self.getLegalActions(state)
          which returns legal actions for a state
    """
    def __init__(self, **args):
        "You can initialize Q-values here..."
        ReinforcementAgent.__init__(self, **args)

        "*** YOUR CODE HERE ***"
        counter = util.Counter()
        self.q_values = counter

    def getQValue(self, state, action):
        """
          Returns Q(state,action)
          Should return 0.0 if we have never seen a state
          or the Q node value otherwise
        """
        "*** YOUR CODE HERE ***"
        q = self.q_values[(state, action)]
        return q
        util.raiseNotDefined()
        


    def computeValueFromQValues(self, state):
        """
          Returns max_action Q(state,action)
          where the max is over legal actions.  Note that if
          there are no legal actions, which is the case at the
          terminal state, you should return a value of 0.0.
        """
        "*** YOUR CODE HERE ***"
        possibleMoves = self.getLegalActions(state)

        counter = util.Counter()

        ret = 0.0

        #nothing to do if no possible moves
        if not possibleMoves:
          return ret
        else:
          #populate counter w/all q values    
          for move in possibleMoves:
            q = self.getQValue(state, move)
            counter[move] = q

        #return action with max q value
        ret = counter[counter.argMax()] 
        return ret
        util.raiseNotDefined()


    def computeActionFromQValues(self, state):
        """
          Compute the best action to take in a state.  Note that if there
          are no legal actions, which is the case at the terminal state,
          you should return None.
        """
        "*** YOUR CODE HERE ***"
        #get all possible moves for evaluation
        possible_moves = self.getLegalActions(state)

        ret = None

        neg_inf = float('-inf')

        #intialize maximum to lowest possible value
        v_star = neg_inf

        for move in possible_moves:
          amount_q = self.q_values[(state, move)]

          #check if curr value is greater than maximum
          if v_star < amount_q:
            #then overwrite maximum and set max move
            v_star = amount_q
            ret = move
        return ret
        util.raiseNotDefined()

    def getAction(self, state):
        """
          Compute the action to take in the current state.  With
          probability self.epsilon, we should take a random action and
          take the best policy action otherwise.  Note that if there are
          no legal actions, which is the case at the terminal state, you
          should choose None as the action.

          HINT: You might want to use util.flipCoin(prob)
          HINT: To pick randomly from a list, use random.choice(list)
        """
        # Pick Action
        possibleMoves = self.getLegalActions(state)
        move = None
        "*** YOUR CODE HERE ***"

        #what is probability result after epsilon is provided
        probability_result = util.flipCoin(self.epsilon)

        #exploit
        if not probability_result:
          move = self.getPolicy(state)
          return move

        #explore
        else:
          move = random.choice(possibleMoves)
          return move
            
        util.raiseNotDefined()

        return action

    def update(self, state, action, nextState, reward):
        """
          The parent class calls this to observe a
          state = action => nextState and reward transition.
          You should do your Q-Value update here

          NOTE: You should never call this function,
          it will be called on your behalf
        """
        "*** YOUR CODE HERE ***"
        #Q value before factoring next state
        priorQ = self.getQValue(state, action)

        alpha_reward = self.alpha * reward

        prior = (1 - self.alpha) * priorQ
        
        #as long as another state exists
        if nextState is not None:

          nextState_part = self.getValue(nextState) * self.alpha * self.discount

          self.q_values[(state, action)] =  nextState_part + prior + alpha_reward

        #otherwise only factor alpha_reward
        else:

          self.q_values[(state, action)] =  alpha_reward + prior
          
        # util.raiseNotDefined()

    def getPolicy(self, state):
        return self.computeActionFromQValues(state)

    def getValue(self, state):
        return self.computeValueFromQValues(state)


class PacmanQAgent(QLearningAgent):
    "Exactly the same as QLearningAgent, but with different default parameters"

    def __init__(self, epsilon=0.05,gamma=0.8,alpha=0.2, numTraining=0, **args):
        """
        These default parameters can be changed from the pacman.py command line.
        For example, to change the exploration rate, try:
            python pacman.py -p PacmanQLearningAgent -a epsilon=0.1

        alpha    - learning rate
        epsilon  - exploration rate
        gamma    - discount factor
        numTraining - number of training episodes, i.e. no learning after these many episodes
        """
        args['epsilon'] = epsilon
        args['gamma'] = gamma
        args['alpha'] = alpha
        args['numTraining'] = numTraining
        self.index = 0  # This is always Pacman
        QLearningAgent.__init__(self, **args)

    def getAction(self, state):
        """
        Simply calls the getAction method of QLearningAgent and then
        informs parent of action for Pacman.  Do not change or remove this
        method.
        """
        action = QLearningAgent.getAction(self,state)
        self.doAction(state,action)
        return action


class ApproximateQAgent(PacmanQAgent):
    """
       ApproximateQLearningAgent

       You should only have to overwrite getQValue
       and update.  All other QLearningAgent functions
       should work as is.
    """
    def __init__(self, extractor='IdentityExtractor', **args):
        self.featExtractor = util.lookup(extractor, globals())()
        PacmanQAgent.__init__(self, **args)
        self.weights = util.Counter()

    def getWeights(self):
        return self.weights

    def getQValue(self, state, action):
        """
          Should return Q(state,action) = w * featureVector
          where * is the dotProduct operator
        """
        "*** YOUR CODE HERE ***"
        ret = 0

        #get all features
        extracted = self.featExtractor.getFeatures(state, action)

        #Calculate q using weights
        for idx in extracted: ret = ret + self.weights[idx] * extracted[idx]
        return ret
        util.raiseNotDefined()

    def update(self, state, action, nextState, reward):
        """
           Should update your weights based on transition
        """
        "*** YOUR CODE HERE ***"
        #left and right side used to calculate difference
        left_side = (reward + self.getValue(nextState) * self.discount)
        right_side = self.getQValue(state, action)

        #get all possible features
        extracted = self.featExtractor.getFeatures(state, action)

        #now update features
        for index in extracted:
          curr_weight = self.weights[index] + ((left_side-right_side) * extracted[index] * self.alpha) 
          self.weights[index] = curr_weight
        # util.raiseNotDefined()

    def final(self, state):
        "Called at the end of each game."
        # call the super-class final method
        PacmanQAgent.final(self, state)

        # did we finish training?
        if self.episodesSoFar == self.numTraining:
            # you might want to print your weights here for debugging
            "*** YOUR CODE HERE ***"
            pass
