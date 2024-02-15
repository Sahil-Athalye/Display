// Deck.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares Deck class with one member function shuffle()

#pragma once
#include <iostream>
#include "CardSet.h"
#include <random>
#include <algorithm>

template <typename R, typename S> class Deck:public CardSet<R,S>{
    public:
        void shuffle();
};



#ifdef TEMPLATE_HEADERS_INCLUDE_SOURCE
#include "Deck.cpp"
#endif

