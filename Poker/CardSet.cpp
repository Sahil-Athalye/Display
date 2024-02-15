// CardSet.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This file defines the print function and right shift operator for CardSet class
#include "CardSet.h"


template<typename R, typename S>
void CardSet<R,S>::print(std::ostream &stream, std::size_t size){
    unsigned idx = 0;
    for (unsigned i = 0; i < CardSet::cardvec.size(); i++) {
        stream << CardSet::cardvec[i] << " ";
        idx++;
        if(idx == size){
            stream << " " <<std::endl;
            idx = 0;
        }
    }
    if (idx != 0) {
        stream << " " << std::endl;
    }
}

template <typename R, typename S>
CardSet<R,S>& CardSet<R,S>::operator>>(CardSet<R, S>& setOfCards){
    if (cardvec.empty()) {
        throw std::runtime_error("Deck is empty");
    }

    setOfCards.cardvec.push_back(cardvec.back());
    cardvec.pop_back();

    return *this;
}

template <typename R, typename S>
bool CardSet<R,S>::is_empty(){
    return cardvec.empty();
}

template <typename R, typename S>
typename CardSet<R,S>::iter CardSet<R,S>::get_start(){
    return cardvec.begin();
}

template <typename R, typename S>
typename CardSet<R,S>::iter CardSet<R,S>::get_end(){
    return cardvec.end();
}

template <typename R, typename S>
void CardSet<R,S>::sort_cardvec(){

    std::sort(get_start(), get_end(), rankFirstCompare<R,S>);
}

template <typename R, typename S>
void CardSet<R,S>::collect(CardSet<R,S>& cards){
    std::move(cards.get_start(),cards.get_end(), std::back_inserter(this->cardvec));
    cards.cardvec.clear();
}

template <typename R, typename S>
void CardSet<R,S>::collect_if(CardSet<R,S> & deck, std::function<bool(card_type &)> function){
    std::copy_if(deck.get_start(),deck.get_end(), std::back_inserter(this->cardvec), function);

    //remove replaces values with undefined values - does not shrink vector
    auto it = std::remove_if(deck.get_start(), deck.get_end(), function);

    //erases undefined values leftover by remove
    deck.cardvec.erase(it, deck.cardvec.end());
}

template <typename R, typename S>
bool CardSet<R,S>::request(CardSet<R,S>& deck, R rank){
    // find_if returns an iter to the first element in the range [first, last) that satisfies the critiria
    // returns last if there is no such iterator
    iter it = std::find_if(deck.cardvec.begin(), deck.cardvec.end(), [rank](card_type card){
        return card.rank == rank;
    });

    if (it != deck.cardvec.end()) {
        // card found
        this->cardvec.push_back(*it);
        deck.cardvec.erase(it);
        return true;
    }

    return false;
}