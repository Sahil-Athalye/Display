// Card.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This file defines the constructor and the extractor operator for Card class
#include "Card.h"

template <typename R, typename S>
Card<R,S>::Card(const R &r,const S &s){
    rank = r;
    suit = s;
}

template <typename R, typename S>
std::ostream& operator <<(ostream &stream, const Card<R, S> &card){
    stream<<card.rank<<card.suit;
    return stream;
}

template <typename R, typename S>
bool rankFirstCompare(const Card<R,S> & lhs, const Card<R, S> & rhs) {
    if (lhs.rank < rhs.rank) {
        return true;
    } else {
        if (lhs.rank == rhs.rank) {
            return lhs.suit < rhs.suit;
        }
        return false;
    }
}
template <typename R, typename S>
bool suitFirstCompare(const Card<R,S> & lhs, const Card<R, S> & rhs) {
    if (lhs.suit < rhs.suit) {
        return true;
    } else {
        if (lhs.suit == rhs.suit) {
            return lhs.rank < rhs.rank;
        }
        return false;
    }
}
