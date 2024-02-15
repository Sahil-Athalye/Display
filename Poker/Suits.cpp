// Suits.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files defines class methods for Suit

#include "Suits.h"
#include <iostream>


std::ostream& operator<<(std::ostream &stream, const Suit &suit){
    switch (suit){
        case Suit::Clubs:
            stream<<"C";
            break;
        case Suit::Diamonds:
            stream<<"D";
            break;
        case Suit::Hearts:
            stream<<"H";
            break;
        case Suit::Spades:
            stream<<"S";
            break;
        case Suit::undefined:
            stream<<"?";
            break;
    }
    return stream;
}


Suit& operator++ (Suit &suit){
    switch (suit){
        case Suit::Clubs:
            suit = Suit::Diamonds;
            break;
        case Suit::Diamonds:
            suit = Suit::Hearts;
            break;
        case Suit::Hearts:
            suit = Suit::Spades;
            break;
        case Suit::Spades:
            suit = Suit::undefined;
            break;
        case Suit::undefined:
            suit = Suit::undefined;
            break;
    }

    return suit;
}

