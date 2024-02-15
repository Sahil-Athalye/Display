// Uno.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares operators for Color and UnoRank enum types. It also defines the constructor for UnoDeck

#include "Uno.h"

std::ostream& operator<<(std::ostream &stream, const Color &color){
    switch(color){
        case Color::red:
            stream<<"red";
            break;
        case Color::blue:
            stream<<"blue";
            break;
        case Color::green:
            stream<<"green";
            break;
        case Color::yellow:
            stream<<"yellow";
            break;
        case Color::black:
            stream<<"black";
            break;
        case Color::undefined:
            stream<<"undefined";
            break;
    }

    return stream;
}

Color& operator++ (Color &color){
    switch(color){
        case Color::red:
            color = Color::blue;
            break;
        case Color::blue:
            color = Color::green;
            break;
        case Color::green:
            color = Color::yellow;
            break;
        case Color::yellow:
            color = Color::black;
            break;
        case Color::black:
            color = Color::undefined;
            break;
        case Color::undefined:
            color = Color::undefined;
            break;
    }
    return color;
}

std::ostream& operator<<(std::ostream &stream, const UnoRank &uno_rank){
    switch(uno_rank){
        case UnoRank::zero:
            stream<<"0";
            break;
        case UnoRank::one:
            stream<<"1";
            break;
        case UnoRank::two:
            stream<<"2";
            break;
        case UnoRank::three:
            stream<<"3";
            break;
        case UnoRank::four:
            stream<<"4";
            break;
        case UnoRank::five:
            stream<<"5";
            break;
        case UnoRank::six:
            stream<<"6";
            break;
        case UnoRank::seven:
            stream<<"7";
            break;
        case UnoRank::eight:
            stream<<"8";
            break;
        case UnoRank::nine:
            stream<<"9";
            break;
        case UnoRank::skip:
            stream<<"skip";
            break;
        case UnoRank::reverse:
            stream<<"reverse";
            break;
        case UnoRank::drawtwo:
            stream<<"drawtwo";
            break;
        case UnoRank::drawfour:
            stream<<"drawfour";
            break;
        case UnoRank::wild:
            stream<<"wild";
            break;
        case UnoRank::blank:
            stream<<"blank";
            break;
        case UnoRank::undefined:
            stream<<"?";
            break;
    }

    return stream;
}

UnoRank& operator++ (UnoRank &uno_rank){
    switch(uno_rank){
        case UnoRank::zero:
            uno_rank = UnoRank::one;
            break;
        case UnoRank::one:
            uno_rank = UnoRank::two;
            break;
        case UnoRank::two:
            uno_rank = UnoRank::three;
            break;
        case UnoRank::three:
            uno_rank = UnoRank::four;
            break;
        case UnoRank::four:
            uno_rank = UnoRank::five;
            break;
        case UnoRank::five:
            uno_rank = UnoRank::six;
            break;
        case UnoRank::six:
            uno_rank = UnoRank::seven;
            break;
        case UnoRank::seven:
            uno_rank = UnoRank::eight;
            break;
        case UnoRank::eight:
            uno_rank = UnoRank::nine;
            break;
        case UnoRank::nine:
            uno_rank = UnoRank::skip;
            break;
        case UnoRank::skip:
            uno_rank = UnoRank::reverse;
            break;
        case UnoRank::reverse:
            uno_rank = UnoRank::drawtwo;
            break;
        case UnoRank::drawtwo:
            uno_rank = UnoRank::drawfour;
            break;
        case UnoRank::drawfour:
            uno_rank = UnoRank::wild;
            break;
        case UnoRank::wild:
            uno_rank = UnoRank::blank;
            break;
        case UnoRank::blank:
            uno_rank = UnoRank::undefined;
            break;
        case UnoRank::undefined:
            uno_rank = UnoRank::undefined;
            break;
    }

    return uno_rank;

}

UnoDeck::UnoDeck() {
    // one red, one blue, one green, and one yellow card of rank zero
    for (Color c = Color::red; c != Color::black; ++c) {
        cardvec.emplace_back(UnoRank::zero, c);
    }

    // two red, two blue, two green, and two yellow cards of each rank one through drawtwo
    for (Color c = Color::red; c != Color::black; ++c) {
        for (UnoRank r = UnoRank::one; r <= UnoRank::drawtwo; ++r) {
            cardvec.emplace_back(r, c);
            cardvec.emplace_back(r, c);
        }
    }

    // four black cards of each rank drawfour through blank
    for (UnoRank r = UnoRank::drawfour; r != UnoRank::undefined; ++r) {
        cardvec.emplace_back(r, Color::black);
        cardvec.emplace_back(r, Color::black);
        cardvec.emplace_back(r, Color::black);
        cardvec.emplace_back(r, Color::black);
    }

}

