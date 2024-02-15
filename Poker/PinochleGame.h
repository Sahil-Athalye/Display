// PinochleGame.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares PinochleGame
#include <vector>
#include <string>

#include "Pinochle.h"
#include "Game.h"
#include "Suits.h"
#include "CardSet.h"
#include "Card.h"

enum class PinochleMelds{
    dix = 0,
    offsuitmarriage = 1,
    fortyjacks = 2,
    pinochle = 3,
    insuitmarriage = 4,
    sixtyqueens = 5,
    eightykings = 6, 
    hundredaces = 7, 
    insuitrun = 8, 
    doublepinochle = 9, 
    fourhundredjacks = 10, 
    sixhundredqueens = 11, 
    eighthundredkings = 12, 
    thousandaces = 13, 
    insuitdoublerun = 14,
};

class PinochleGame : public Game {
    public:
        static constexpr unsigned int pinochlePointVals[15] = {10, 20, 40, 40, 40, 60, 80, 100, 150, 300, 400, 600, 800, 1000, 1500};
        PinochleGame(int argc, const char * argv[]);
        virtual int play();

    protected:
        PinochleDeck deck;
        std::vector<CardSet<PinochleRank, Suit>> hands;
        virtual void deal();
    
    private:
        void printHands();
        void printMelds(std::vector<PinochleMelds> &points);
        void collectCards();
        std::string takeUserResponse();
        void suit_independent_evaluation(const CardSet<PinochleRank, Suit> &cards, std::vector<PinochleMelds> &melds);

};


std::ostream& operator<<(std::ostream &stream, const PinochleMelds &pinochle_melds);
