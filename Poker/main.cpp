// main.cpp
// authors: Sahil Athalye
// 	    Yuheng Ding
// 	    Reedham Kalariya
// This file creates either a Pinochle, HoldEm, or GoFish game based on user input and allows the user to play the game.

#include <iostream>
#include <memory>
#include "Suits.h"
#include "Card.h"
#include "Pinochle.h"
#include "Poker.h"
#include "Game.h"
#include "PinochleGame.h"
#include "HoldEmGame.h"
#include "CardSet.h"
#include "Uno.h"
#include "GoFishGame.h"

#define ERR 1
#define NULLGAME 2



std::shared_ptr<Game> create(int argc, const char * argv[]){
    std::shared_ptr<Game> gametype;
    if(std::string(argv[1]) == "Pinochle"){
        gametype = std::make_shared<PinochleGame>(argc, argv);
    } else if ((std::string(argv[1]) == "HoldEm")) {
        gametype = std::make_shared<HoldEmGame>(argc, argv);
    } 
    else if((std::string(argv[1]) == "GoFish")&& (std::string(argv[2]) == "HoldEm")) { 
        gametype = std::make_shared< GoFishGame< Suit, PokerRank, PokerDeck> >(argc, argv);
    }
    else if((std::string(argv[1]) == "GoFish")&& (std::string(argv[2]) == "Pinochle")) {  
        gametype = std::make_shared<GoFishGame<Suit, PinochleRank, PinochleDeck>>(argc, argv);
    }
    else if((std::string(argv[1]) == "GoFish")&& (std::string(argv[2]) == "Uno")) {  
        gametype = std::make_shared<GoFishGame<Color, UnoRank, UnoDeck>>(argc, argv);
    }
    return gametype;
}



int main(int argc, const char * argv[]){
    if(argc<2){
        std::cout << "Gameboard inputs were incorrect. Please enter correct game name and correct number of players. Refer to the Usage Messages below." << std::endl;
        std::cout << "Usage Message: ./Lab3 Pinochle <player1_name> <player2_name> <player3_name> <player4_name>" << std::endl;
        std::cout << "Usage Message: ./Lab3 HoldEm <player1_name> <player2_name> <optional_player3_name> <optional_player4_name> <optional_player5_name> <optional_player6_name> <optional_player7_name> <optional_player8_name> <optional_player9_name>" << std::endl;
        std::cout << "Usage Message: ./Lab3 GoFish <DECK_TYPE> <player1_name> <player2_name> <optional player3_name> <optional player4_name> <optional player5_name>" << std::endl;
        std::cout<<"DECK_TYPE can be <HoldEm> OR <Pinochle> OR <Uno>"<<std::endl;
        return ERR;
    }

    std::shared_ptr<Game> gameboard;
    if((std::string(argv[1]) == "Pinochle") && (argc == 6)){
        gameboard = create(argc, argv);
    } else if ((std::string(argv[1]) == "HoldEm") && (argc <= 11) && (argc>=4)){
        gameboard = create(argc,argv); 
    } else if ((std::string(argv[1]) == "GoFish") && (argc <= 8) && (argc>=5)){
        gameboard = create(argc,argv); 
    }else {
        std::cout << "Gameboard inputs were incorrect. Please enter correct game name and correct number of players. Refer to the Usage Messages below." << std::endl;
        std::cout << "Usage Message: ./Lab3 Pinochle <player1_name> <player2_name> <player3_name> <player4_name>" << std::endl;
        std::cout << "Usage Message: ./Lab3 HoldEm <player1_name> <player2_name> <optional_player3_name> <optional_player4_name> <optional_player5_name> <optional_player6_name> <optional_player7_name> <optional_player8_name> <optional_player9_name>" << std::endl;
        std::cout << "Usage Message: ./Lab3 GoFish <DECK_TYPE> <player1_name> <player2_name> <optional player3_name> <optional player4_name> <optional player5_name>" << std::endl;
        std::cout<<"DECK_TYPE can be <HoldEm> OR <Pinochle> OR <Uno>"<<std::endl;
        return ERR;
    }

    if(gameboard){
        gameboard->play();
    } else {
        std::cout << "Gameboard was null. It is improperly initialized. Please enter correct inputs." << std::endl;
        return NULLGAME;
    }
    return 0;
}
