// Pinochle.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files defines methods for PinochleDeck

#include "Pinochle.h"

std::ostream& operator<<(std::ostream &stream, const PinochleRank &pinochle_rank){
    switch (pinochle_rank){
        case PinochleRank::Nine:
            stream<<"9";
            break;
        case PinochleRank::Jack:
            stream<<"J";
            break;
        case PinochleRank::Queen:
            stream<<"Q";
            break;
        case PinochleRank::King:
            stream<<"K";
            break;
        case PinochleRank::Ten:
            stream<<"10";
            break;
        case PinochleRank::Ace:
            stream<<"A";
            break;
        case PinochleRank::undefined:
            stream<<"?";
            break;
    }
    return stream;
}

PinochleRank& operator++ (PinochleRank &pinochle_rank){
    switch (pinochle_rank){
        case PinochleRank::Nine:
            pinochle_rank = PinochleRank::Jack;
            break;
        case PinochleRank::Jack:
            pinochle_rank = PinochleRank::Queen;
            break;
        case PinochleRank::Queen:
            pinochle_rank = PinochleRank::King;
            break;
        case PinochleRank::King:
            pinochle_rank = PinochleRank::Ten;
            break;
        case PinochleRank::Ten:
            pinochle_rank = PinochleRank::Ace;
            break;
        case PinochleRank::Ace:
            pinochle_rank = PinochleRank::undefined;
            break;
        case PinochleRank::undefined:
            pinochle_rank = PinochleRank::undefined;
            break;
    }

    return pinochle_rank;
}

PinochleDeck::PinochleDeck(){

    for(Suit s = Suit::Clubs; s!=Suit::undefined; ++s){
        for(PinochleRank r = PinochleRank::Nine; r!=PinochleRank::undefined; ++r){

            cardvec.push_back(Card<PinochleRank, Suit>(r, s));

            cardvec.push_back(Card<PinochleRank, Suit>(r, s));
        }
    }
}


// void PinochleDeck::print(std::ostream &stream){
//     for(unsigned i = 0; i<PinochleDeck::PinochleCards.size();i++){
//         stream<<PinochleDeck::PinochleCards[i]<<std::endl;
//     }
// }
