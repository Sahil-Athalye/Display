// Card.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This file declares Card class

#pragma once
#include "Suits.h"

#include <iostream>

using std::ostream;
template <typename R, typename S> class Card;

template <typename R, typename S>
ostream& operator<< (ostream &stream, const Card<R,S> &card);

template <typename R, typename S> class Card{
    public:
        Card(const R &rank,const S &suit);

        friend ostream& operator<<<R, S>
        (ostream &stream, const Card<R, S> &card);
        R rank;
        S suit;
};

template <typename R, typename S> bool rankFirstCompare(const Card<R,S> & lhs, const Card<R, S> & rhs);
template <typename R, typename S> bool suitFirstCompare(const Card<R,S> & lhs, const Card<R, S> & rhs);

#ifdef TEMPLATE_HEADERS_INCLUDE_SOURCE
#include "Card.cpp"
#endif
