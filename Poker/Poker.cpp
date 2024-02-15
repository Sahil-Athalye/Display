// Poker.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files defines methods for PokerDeck

#include "Poker.h"

std::ostream& operator<<(std::ostream &stream, const PokerRank &poker_rank){
    switch (poker_rank){
        case PokerRank::Two:
            stream<<"2";
            break;
        case PokerRank::Three:
            stream<<"3";
            break;
        case PokerRank::Four:
            stream<<"4";
            break;
        case PokerRank::Five:
            stream<<"5";
            break;
        case PokerRank::Six:
            stream<<"6";
            break;
        case PokerRank::Seven:
            stream<<"7";
            break;
        case PokerRank::Eight:
            stream<<"8";
            break;
        case PokerRank::Nine:
            stream<<"9";
            break;
        case PokerRank::Ten:
            stream<<"10";
            break;
        case PokerRank::Jack:
            stream<<"J";
            break;
        case PokerRank::Queen:
            stream<<"Q";
            break;
        case PokerRank::King:
            stream<<"K";
            break;
        case PokerRank::Ace:
            stream<<"A";
            break;
        case PokerRank::undefined:
            stream<<"?";
            break;
    }
    return stream;
}

PokerRank& operator++ (PokerRank &Poker_rank){
    switch (Poker_rank){
        case PokerRank::Two:
            Poker_rank = PokerRank::Three;
            break;
        case PokerRank::Three:
            Poker_rank = PokerRank::Four;
            break;
        case PokerRank::Four:
            Poker_rank = PokerRank::Five;
            break;
        case PokerRank::Five:
            Poker_rank = PokerRank::Six;
            break;
        case PokerRank::Six:
            Poker_rank = PokerRank::Seven;
            break;
        case PokerRank::Seven:
            Poker_rank = PokerRank::Eight;
            break;
        case PokerRank::Eight:
            Poker_rank = PokerRank::Nine;
            break;
        case PokerRank::Nine:
            Poker_rank = PokerRank::Ten;
            break;
        case PokerRank::Ten:
            Poker_rank = PokerRank::Jack;
            break;
        case PokerRank::Jack:
            Poker_rank = PokerRank::Queen;
            break;
        case PokerRank::Queen:
            Poker_rank = PokerRank::King;
            break;
        case PokerRank::King:
            Poker_rank = PokerRank::Ace;
            break;
        case PokerRank::Ace:
            Poker_rank = PokerRank::undefined;
            break;
        case PokerRank::undefined:
            Poker_rank = PokerRank::undefined;
            break;
    }

    return Poker_rank;
}

PokerDeck::PokerDeck(){
    for(Suit s = Suit::Clubs; s!=Suit::undefined; ++s){
        for(PokerRank r = PokerRank::Two; r!=PokerRank::undefined; ++r){
            cardvec.push_back(Card<PokerRank, Suit>(r, s));
        }
    }
}


// void PokerDeck::print(std::ostream &stream){
//     for(unsigned i = 0; i<PokerDeck::PokerCards.size();i++){
//         stream<<PokerDeck::PokerCards[i]<<std::endl;
//     }
// }
