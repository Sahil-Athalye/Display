// Deck.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This file defines the Shuffle function for Deck class
#include "Deck.h"


template <typename R, typename S>
void Deck<R,S>::shuffle(){
    std::random_device device;
    std::mt19937 rng{device()};
    std::shuffle(CardSet<R,S>::cardvec.begin(),CardSet<R,S>::cardvec.end(), rng);
}
