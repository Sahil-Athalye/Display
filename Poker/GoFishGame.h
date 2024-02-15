// GoFishGame.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
//This file declares class GoFishGame which inherits from base class Game

#pragma once

#include "Deck.h"
#include "CardSet.h"
#include "Uno.h"
#include "Poker.h"
#include "Suits.h"

#include <vector>
#include <set>
#include <type_traits>

template <typename S = Suit, typename R = PokerRank, typename D = PokerDeck>
class GoFishGame : public Game {
    public:
        GoFishGame(int num_players, const char * player_names[]);
        virtual ~GoFishGame();
        virtual int play();
        bool collect_books(int player_num);
    protected:
        static_assert(std::is_base_of<CardSet<R, S>, D>::value);
        D deck;
        std::vector<CardSet<R, S>> hands;
        std::vector<CardSet<R, S>> books;
        std::vector<int> book_nums;
        std::vector<std::string> player_names;
        virtual void deal();
        bool turn(int player_number);
    private:
        std::set<int> finished_players;
};


#ifdef TEMPLATE_HEADERS_INCLUDE_SOURCE
#include "GoFishGame.cpp"
#endif
