// Pinochle.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares emum PinochleRank and class PinochleDeck

#pragma once
#include <vector>
#include "Card.h"
#include "Suits.h"
#include "Deck.h"

enum class PinochleRank{
    Nine = 0,
    Jack = 1,
    Queen = 2,
    King = 3,
    Ten = 4,
    Ace = 5,
    undefined = 6,
};

std::ostream& operator<<(std::ostream &stream, const PinochleRank &pinochle_rank);

PinochleRank& operator++ (PinochleRank &pinochle_rank);

class PinochleDeck:public Deck<PinochleRank , Suit>{
    public:
        PinochleDeck();
        // void print(std::ostream &stream);
    private:
        // std::vector<Card<PinochleRank,Suit>> PinochleCards;
};
