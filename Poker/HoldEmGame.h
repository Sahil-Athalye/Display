// HoldEmGame.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares HoldEmGame class for Texas Hold'em

#include <vector>
#include <string>

#include "Poker.h"
#include "Game.h"
#include "Suits.h"
#include "CardSet.h"

enum class HoldEmState {
    preflop = 0,
    flop = 1,
    turn = 2,
    river = 3,
    undefined = 4
};

enum class HoldEmHandRank{
    xhigh = 0,
    pair = 1,
    twopair = 2,
    threeofakind = 3,
    straight = 4,
    flush = 5,
    fullhouse = 6, 
    fourofakind = 7, 
    straightflush = 8, 
    undefined = 9, 
};

class HoldEmGame : public Game {
    public:
        HoldEmGame(int argc, const char * argv[]);
        virtual int play();
        
        class Player {
            public:
                CardSet<PokerRank, Suit> hand;
                std::string playerName;
                HoldEmHandRank handRank;

                Player(const CardSet<PokerRank, Suit> & oHand, const std::string & oName, const HoldEmHandRank & oRank);

        };

    protected:
        HoldEmState currstate;
        PokerDeck currdeck;
        std::vector<CardSet<PokerRank, Suit>> hands;
        CardSet<PokerRank, Suit> board;
        virtual void deal();

    private:
        void printHands();
        void collectCards();
        std::string takeUserResponse();
        HoldEmHandRank holdem_hand_evaluation(const CardSet<PokerRank, Suit>& cardset);

};

std::ostream& operator<<(std::ostream &stream, const HoldEmHandRank &hand_rank);

bool operator<(const HoldEmGame::Player &player1, const HoldEmGame::Player &player2);