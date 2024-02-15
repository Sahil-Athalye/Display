// Poker.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares PokerRank and PokerDeck

#pragma once
#include <vector>
#include "Card.h"
#include "Suits.h"
#include "Deck.h"

enum class PokerRank{
    Two = 0,
    Three = 1,
    Four = 2,
    Five = 3,
    Six = 4,
    Seven = 5,
    Eight = 6,
    Nine = 7,
    Ten = 8,
    Jack = 9,
    Queen = 10,
    King = 11,
    Ace = 12,
    undefined = 13,
};

std::ostream& operator<<(std::ostream &stream, const PokerRank &poker_rank);

PokerRank& operator++ (PokerRank &poker_rank);

class PokerDeck:public Deck<PokerRank,Suit>{
    public:
        PokerDeck();
        // void print(std::ostream &stream);
    private:
        // std::vector<Card<PokerRank,Suit>> PokerCards;
};
