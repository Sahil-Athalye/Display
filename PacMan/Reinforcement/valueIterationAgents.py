# valueIterationAgents.py
# -----------------------
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


# valueIterationAgents.py
# -----------------------
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


import mdp, util

from learningAgents import ValueEstimationAgent
import collections

class ValueIterationAgent(ValueEstimationAgent):
    """
        * Please read learningAgents.py before reading this.*

        A ValueIterationAgent takes a Markov decision process
        (see mdp.py) on initialization and runs value iteration
        for a given number of iterations using the supplied
        discount factor.
    """
    def __init__(self, mdp, discount = 0.9, iterations = 100):
        """
          Your value iteration agent should take an mdp on
          construction, run the indicated number of iterations
          and then act according to the resulting policy.

          Some useful mdp methods you will use:
              mdp.getStates()
              mdp.getPossibleActions(state)
              mdp.getTransitionStatesAndProbs(state, action)
              mdp.getReward(state, action, nextState)
              mdp.isTerminal(state)
        """
        self.mdp = mdp
        self.discount = discount
        self.iterations = iterations
        self.values = util.Counter() # A Counter is a dict with default 0
        self.runValueIteration()

    def runValueIteration(self):
        # Write value iteration code here
        "*** YOUR CODE HERE ***"
        neg_inf = float("-inf")
        
        the_range = []
        i = 0
        #populate range
        while i < self.iterations:
            the_range.append(i)
            i = i + 1

        #iterate through range of iterations
        for num in the_range:
          ctr = util.Counter()

          #took Spanish in HS
          for estado in self.mdp.getStates():
            #initialize v such that it is overwritten in first iteration
            v = neg_inf

            possibleMoves = self.mdp.getPossibleActions(estado)

            #find v for state by iterating through q vals for each pi and grabbing maximum
            for move in possibleMoves:
              curr_q = self.computeQValueFromValues(estado, move)
              if curr_q > v:
                v = curr_q
                #set to max q
                ctr[estado] = v

          #Now self.values will contain max values
          self.values = ctr


    def getValue(self, state):
        """
          Return the value of the state (computed in __init__).
        """
        return self.values[state]


    def computeQValueFromValues(self, state, action):
        """
          Compute the Q-value of action in state from the
          value function stored in self.values.
        """
        "*** YOUR CODE HERE ***"
        ret_sum = 0

        gamma = self.discount

        #fetch set of states and probabilities
        S = self.mdp.getTransitionStatesAndProbs(state, action)
        
        #summation of T*(R+gamma*V(s'))
        for s_prime, likelihood in S:
            R = self.mdp.getReward(state, action, s_prime)

            #add to total
            ret_sum += likelihood * (R + gamma * self.values[s_prime])

        #return total
        return ret_sum
        util.raiseNotDefined()

    def computeActionFromValues(self, state):
        """
          The policy is the best action in the given state
          according to the values currently stored in self.values.

          You may break ties any way you see fit.  Note that if
          there are no legal actions, which is the case at the
          terminal state, you should return None.
        """
        "*** YOUR CODE HERE ***"
        pi_star = None
        neg_inf = float("-inf")

        #initialize V to be automatically overwritten in first iteration
        V = neg_inf

        #iterate through all possible moves
        possibleMoves = self.mdp.getPossibleActions(state)

        #no move to be returned if no possible moves
        if not possibleMoves:
            return None

        for pi in possibleMoves:
          Q = self.computeQValueFromValues(state, pi)

          #V = max(Q)
          if Q > V:
            V = Q
            pi_star = pi

        #pi* is the move that yields utility V    
        return pi_star
        util.raiseNotDefined()

    def getPolicy(self, state):
        return self.computeActionFromValues(state)

    def getAction(self, state):
        "Returns the policy at the state (no exploration)."
        return self.computeActionFromValues(state)

    def getQValue(self, state, action):
        return self.computeQValueFromValues(state, action)

class AsynchronousValueIterationAgent(ValueIterationAgent):
    """
        * Please read learningAgents.py before reading this.*

        An AsynchronousValueIterationAgent takes a Markov decision process
        (see mdp.py) on initialization and runs cyclic value iteration
        for a given number of iterations using the supplied
        discount factor.
    """
    def __init__(self, mdp, discount = 0.9, iterations = 1000):
        """
          Your cyclic value iteration agent should take an mdp on
          construction, run the indicated number of iterations,
          and then act according to the resulting policy. Each iteration
          updates the value of only one state, which cycles through
          the states list. If the chosen state is terminal, nothing
          happens in that iteration.

          Some useful mdp methods you will use:
              mdp.getStates()
              mdp.getPossibleActions(state)
              mdp.getTransitionStatesAndProbs(state, action)
              mdp.getReward(state)
              mdp.isTerminal(state)
        """
        ValueIterationAgent.__init__(self, mdp, discount, iterations)

    def runValueIteration(self):
        "*** YOUR CODE HERE ***"
        S = self.mdp.getStates()

        #get size of states array
        len_S = 0
        for s in S:
            len_S = len_S+1

        the_range = []
        i = 0
        while i < self.iterations:
            the_range.append(i)
            i = i +1
            
        #iterate through range of iterations    
        for num in the_range:
          little_s = S[num % len_S]

          final = self.mdp.isTerminal(little_s)

          #check if current state is final state
          if final:
            continue
          #if not then we can evaluate state
          else:
            Q_vals = []

            possibleMoves = self.mdp.getPossibleActions(little_s)
            for move in possibleMoves:
              #find all Q values
              q = self.computeQValueFromValues(little_s, move)
              Q_vals.insert(-1,q)

            #set V to max(Q)
            self.values[little_s] = max(Q_vals)

class PrioritizedSweepingValueIterationAgent(AsynchronousValueIterationAgent):
    """
        * Please read learningAgents.py before reading this.*

        A PrioritizedSweepingValueIterationAgent takes a Markov decision process
        (see mdp.py) on initialization and runs prioritized sweeping value iteration
        for a given number of iterations using the supplied parameters.
    """
    def __init__(self, mdp, discount = 0.9, iterations = 100, theta = 1e-5):
        """
          Your prioritized sweeping value iteration agent should take an mdp on
          construction, run the indicated number of iterations,
          and then act according to the resulting policy.
        """
        self.theta = theta
        ValueIterationAgent.__init__(self, mdp, discount, iterations)

    def runValueIteration(self):
        "*** YOUR CODE HERE ***"
        prior_states = {}

        the_queue = util.PriorityQueue()
        
        for estado in self.mdp.getStates():
            final = self.mdp.isTerminal(estado)
            #check if this state is the final state
            if final:
                continue
            #if not the final state
            else:
                possibleMoves = self.mdp.getPossibleActions(estado)
                #iterate through all moves
                for move in possibleMoves:
                    for followingEstado, prob in self.mdp.getTransitionStatesAndProbs(estado, move):
                        #either rewrite state or add state
                        if followingEstado not in prior_states:
                            prior_states[followingEstado] = {estado}
                        else:
                            prior_states[followingEstado].add(estado)
                  

        for estado in self.mdp.getStates():
            final = self.mdp.isTerminal(estado)
            #check if this state is the final state
            if final:
                continue
            #if not the final state
            else:
                q_vals = []
                possibleMoves = self.mdp.getPossibleActions(estado)
                #iterate through all moves
                for move in possibleMoves:
                    #get q for each move
                    q = self.computeQValueFromValues(estado, move)
                    q_vals.insert(-1,q)
                    #calculate difference using q*
                    subtraction_result = abs(max(q_vals) - self.values[estado])
                    the_queue.update(estado, - subtraction_result)

        the_range = []
        i = 0
        while i < self.iterations:
            the_range.append(i)
            i = i +1
        for num in the_range:
            if len(the_queue.heap) == 0:
                break
            currentState = the_queue.pop()

            final = self.mdp.isTerminal(currentState)

            #check if this state is the final state
            if final:
                continue

            #if not the final state
            else:
                q_vals = []

                possibleMoves = self.mdp.getPossibleActions(currentState)
                #iterate through all moves
                for move in possibleMoves:
                    #get q for each move   
                    q = self.computeQValueFromValues(currentState, move)
                    #append to array of q vals
                    q_vals.append(q)
            
                #assign v based on max(q)
                self.values[currentState] = max(q_vals)


            for currState in prior_states[currentState]:
                final = self.mdp.isTerminal(currState)
                #check if this state is the final state
                if final:
                    continue
                #if not the final state
                else:
                    q_vals = []

                    possibleMoves = self.mdp.getPossibleActions(currState)
                    #iterate through all moves
                    for move in possibleMoves:
                        q_vals.append(self.computeQValueFromValues(currState, move))

                        #use v = max(q) in difference calculation
                        sub_val = abs(max(q_vals) - self.values[currState])
                    if not sub_val <= self.theta:
                        #update queue if difference is greater than theta
                        the_queue.update(currState, -sub_val)


