// CardSet.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares CardSet class

#pragma once
#include <iostream>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <functional>

#include "Card.h"


template <typename R, typename S> class CardSet{
    public:
        typedef typename std::vector< Card<R,S> >::iterator iter;
        typedef Card<R,S> card_type;
        typedef R rank_type;
        typedef S suit_type;

        void print(std::ostream &stream, std::size_t size);
        CardSet<R,S>& operator>>(CardSet<R, S>& setOfCards);
        bool is_empty();
        iter get_start();
        iter get_end();
        void sort_cardvec();
        void collect(CardSet<rank_type,suit_type>&);
        void collect_if(CardSet<rank_type,suit_type> & deck, std::function<bool(Card<R, S>&)>);
        bool request(CardSet<rank_type,suit_type>& deck, R rank);
    protected:
        std::vector< Card<R,S> > cardvec;
};

#ifdef TEMPLATE_HEADERS_INCLUDE_SOURCE
#include "CardSet.cpp"
#endif

