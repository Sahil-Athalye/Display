// Game.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files defines the constructor for Game

#define NON_PLAYER_ARGCOUNT 2

#include "Game.h"

Game::Game(int argc, const char * argv[]) {
    for (int i = NON_PLAYER_ARGCOUNT; i < argc; i++) {
        playerNames.push_back(std::string(argv[i]));
    }
}