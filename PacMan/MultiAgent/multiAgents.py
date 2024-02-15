# multiAgents.py
# --------------
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


from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
    """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
    """


    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {NORTH, SOUTH, WEST, EAST, STOP}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices) # Pick randomly among the best

        "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action)
        newPos = successorGameState.getPacmanPosition()
        newFood = successorGameState.getFood()
        newGhostStates = successorGameState.getGhostStates()
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

        "*** YOUR CODE HERE ***"
        separationFromGhost = 100000
        for newloc in newGhostStates:
            if manhattanDistance(newPos, newloc.getPosition()) < separationFromGhost:
                separationFromGhost = manhattanDistance(newPos, newloc.getPosition())

        increasesPoints = successorGameState.getScore() - currentGameState.getScore()
        distToNearestPellet = 10000000
        for pelletLoc in currentGameState.getFood().asList():
            if manhattanDistance(pelletLoc, currentGameState.getPacmanPosition()) < distToNearestPellet:
                distToNearestPellet = manhattanDistance(pelletLoc, currentGameState.getPacmanPosition())
        distToFuturePellets = [manhattanDistance(newPos, pellets) for pellets in newFood.asList()]
        bestPelletDist = 0 if not distToFuturePellets else min(distToFuturePellets)
        pelletCloser = distToNearestPellet - bestPelletDist


        #Death condition
        if action == Directions.STOP or separationFromGhost < 1:
            return 0
    
        ret = 0
        #Eat pellet if within options for successor state otherwise get closer to the nearest pellet
        #Try not to change directions in general
        if increasesPoints > 0:
            ret = 6969
        elif pelletCloser > 0:
            ret = 69
        elif action == currentGameState.getPacmanState().getDirection():
            ret =  9
        else:
            ret = 6

    
        return ret

def scoreEvaluationFunction(currentGameState):
    """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
    """
    return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
    """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
    """
    Your minimax agent (question 2)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action from the current gameState using self.depth
        and self.evaluationFunction.

        Here are some method calls that might be useful when implementing minimax.

        gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

        gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

        gameState.getNumAgents():
        Returns the total number of agents in the game

        gameState.isWin():
        Returns whether or not the game state is a winning state

        gameState.isLose():
        Returns whether or not the game state is a losing state
        """
        "*** YOUR CODE HERE ***"
        def Mini(state, currAgent, howDeep):
            possibleMoves = state.getLegalActions(currAgent)
            #If there are no more possible moves then the game must be finished
            if len(possibleMoves)==0:
                return self.evaluationFunction(state)

            # Ghost will take the minimum of the minimum possible values
            # PacMan will take the minimum of the maximum possible values
            if currAgent != state.getNumAgents() - 1:
                ret = 10000000
                for move in possibleMoves:
                    if Mini(state.generateSuccessor(currAgent, move), currAgent+1, howDeep) < ret:
                        ret = Mini(state.generateSuccessor(currAgent, move), currAgent+1, howDeep)
                return ret
            else:
                ret = 10000000
                for move in possibleMoves:
                    if Max(state.generateSuccessor(currAgent, move), howDeep) < ret:
                        ret = Max(state.generateSuccessor(currAgent, move), howDeep)
                return ret
                

        def Max(state, howDeep):
            possibleMoves = state.getLegalActions(0)
            #If there are no more possible moves then the game must be finished
            if len(possibleMoves)==0:
                return self.evaluationFunction(state)

            if howDeep == self.depth:
                return self.evaluationFunction(state)

            ret = -1000000
            for move in possibleMoves:
                if Mini(state.generateSuccessor(0, move), 1, howDeep+1) > ret:
                    ret = Mini(state.generateSuccessor(0, move), 1, howDeep+1)
            return ret

        possibleMoves = gameState.getLegalActions(0)

        ret = -1000000000
        retMove = 0
        for move in possibleMoves:
            if Mini(gameState.generateSuccessor(0, move), 1, 1) > ret:
                ret = Mini(gameState.generateSuccessor(0, move), 1, 1)
                retMove = move
        return retMove


class AlphaBetaAgent(MultiAgentSearchAgent):
    """
    Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"
        def minPrune(state, currAgent, howDeep, alpha, beta):
            possibleMoves = state.getLegalActions(currAgent)
            #If there are no more possible moves then the game must be finished
            if len(possibleMoves)==0:
                return self.evaluationFunction(state)

            currVal = float('inf')
            for move in possibleMoves:
                nextState = state.generateSuccessor(currAgent, move)

                if currAgent != state.getNumAgents() - 1:
                    nextVal = minPrune(nextState, currAgent + 1, howDeep, alpha, beta)
                else:
                    nextVal = maxPrune(nextState, howDeep, alpha, beta)
                    
                currVal = min(currVal, nextVal)
                if alpha > currVal and alpha!=currVal:
                    return currVal
                beta = min(beta, currVal)
            return currVal

        def maxPrune(state, howDeep, alpha, beta):
            possibleMoves = state.getLegalActions(0)
            #If there are no more possible moves then the game must be finished
            if len(possibleMoves)==0:
                return self.evaluationFunction(state)

            if howDeep == self.depth:
                return self.evaluationFunction(state)

            currVal = -float('inf')

            if howDeep == 0:
                ret = possibleMoves[0]


            for move in possibleMoves:
                newState = state.generateSuccessor(0, move)
                nextVal = minPrune(newState, 0 + 1, howDeep + 1, alpha, beta)
                if currVal < nextVal and nextVal!=currVal:
                    currVal = nextVal
                    if howDeep == 0:
                        ret = move
                if currVal > beta:
                    return currVal
                alpha = max(alpha, currVal)
            if howDeep == 0:
                return ret
            return currVal

        ret = maxPrune(gameState, 0, -float('inf'), float('inf'))
        return ret


class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """

    def getAction(self, gameState):
        """
        Returns the expectimax action using self.depth and self.evaluationFunction

        All ghosts should be modeled as choosing uniformly at random from their
        legal moves.
        """
        "*** YOUR CODE HERE ***"
        def Max(state, howDeep):
            value = -1000000000

            possibleMoves = state.getLegalActions(0)
            #If there are no more possible moves then the game must be finished
            if len(possibleMoves)==0:
                return self.evaluationFunction(state)

            if howDeep == self.depth:
                return self.evaluationFunction(state)

            
            for move in possibleMoves:
                if Expecti(state.generateSuccessor(0, move), 0 + 1, howDeep + 1) > value:
                    value = Expecti(state.generateSuccessor(0, move), 0 + 1, howDeep + 1)
            return value

        def Expecti(state, currAgent, howDeep):
            

            possibleMoves = state.getLegalActions(currAgent)
            
            #If there are no more possible moves then the game must be finished
            if len(possibleMoves)==0:
                return self.evaluationFunction(state)

            ret = 0
            chance = 1.0 / len(possibleMoves)
            
            for action in possibleMoves:
                next = state.generateSuccessor(currAgent, action)
                
                #do a check to see if we the agent is pacman or not
                #pac man acts optimally, the ghosts do not
                if currAgent != state.getNumAgents() - 1:
                    ret += Expecti(next, currAgent + 1, howDeep) * chance
                else:
                    ret += Max(next, howDeep) * chance

            return ret



        possibleMoves = gameState.getLegalActions(0)

        ret = -1000000000
        retMove = 0
        for move in possibleMoves:
            if Expecti(gameState.generateSuccessor(0, move), 1, 1) > ret:
                ret = Expecti(gameState.generateSuccessor(0, move), 1, 1)
                retMove = move
        return retMove


def betterEvaluationFunction(currentGameState):
    """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: Improved upon the prior eval function
    """
    "*** YOUR CODE HERE ***"
    pellets = currentGameState.getFood().asList()
    points = currentGameState.getScore()
    loci = currentGameState.getPacmanPosition()
    localPelletDistance = 10000000
    if(len(pellets)==0):
        localPelletDistance = 0.5
    else:
        for pellet in pellets:
            if manhattanDistance(loci,pellet) < localPelletDistance:
                localPelletDistance = manhattanDistance(loci,pellet)

    ret = 2.0 / localPelletDistance + points
    return ret

# Abbreviation
better = betterEvaluationFunction
