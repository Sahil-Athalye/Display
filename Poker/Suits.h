// Suits.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares enum class Suit

#pragma once
#include <iostream>

enum class Suit {
    Clubs = 0,
    Diamonds = 1,
    Hearts = 2,
    Spades = 3,
    undefined = 4
};

std::ostream& operator<<(std::ostream &stream, const Suit &suit);

Suit& operator++ (Suit &suit);
