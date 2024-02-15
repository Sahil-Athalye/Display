// Game.h
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files declares base class Game with its constructor, pure virtual function play(), and member variable for playerNames

#pragma once

#include <string>
#include <vector>


class Game {
  public:
    Game(int num, const char * strs[]);
    virtual int play() = 0;
  protected:
    std::vector<std::string> playerNames;
};
