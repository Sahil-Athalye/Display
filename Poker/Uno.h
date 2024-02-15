// Uno.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares enum types Color and UnoRank as well as their operators. It also declares the constructor for UnoDeck

#pragma once

#include <iostream>

#include "Deck.h"

enum class Color {
    red = 0,
    blue = 1,
    green = 2,
    yellow = 3,
    black = 4,
    undefined = 5,
};

enum class UnoRank {
    zero = 0,
    one = 1,
    two = 2,
    three = 3,
    four = 4,
    five = 5,
    six = 6,
    seven = 7,
    eight = 8,
    nine = 9,
    skip = 10,
    reverse = 11,
    drawtwo = 12,
    drawfour = 13,
    wild = 14,
    blank = 15,
    undefined = 16,
};

std::ostream& operator<<(std::ostream &stream, const Color &color);

Color& operator++ (Color &uno_rank);

std::ostream& operator<<(std::ostream &stream, const UnoRank &uno_rank);

UnoRank& operator++ (UnoRank &uno_rank);

class UnoDeck : public Deck<UnoRank, Color>{
    public:
        UnoDeck();
    private:
        
};