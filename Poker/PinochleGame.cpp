// PinochleGame.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files defines class methods for PinochleGame

#include "PinochleGame.h"

#include <iostream>
#include <stdexcept>

PinochleGame::PinochleGame(int argc, const char * argv[]) : Game(argc, argv) {
    for (int i = 2; i < argc; i++) {
        hands.push_back(CardSet<PinochleRank, Suit>());
    }
}

void PinochleGame::deal() {
    int numPlayers = hands.size();
    if (numPlayers == 0) {
        return;
    }
    int currPlayer = 0;
    while (!deck.is_empty()) {
        for (int i = 0; i < 3 && !deck.is_empty(); i++) {
            deck >> hands[currPlayer];
        }
        currPlayer++;
        if (currPlayer == numPlayers) {
            currPlayer = 0;
        }
    }
}

int PinochleGame::play() {
    std::string answer;

    do {
        deck.shuffle();

        try {
            deal();
        } catch (const std::runtime_error& error) {
            std::cout << error.what() << std::endl;
            break;
        }
        
        
        printHands();
        collectCards();

        answer = takeUserResponse();
    } while (answer != "yes");
    
    return 0;
}

void PinochleGame::printHands() {
    int numPlayers = hands.size();
    std::vector<PinochleMelds> points;
    for (int i = 0; i < numPlayers; i++) {
        std::string name = playerNames[i];
        std::cout << name << std::endl;
        hands[i].print(std::cout, 8);

        points.clear();
        suit_independent_evaluation(hands[i],points);
        printMelds(points);
    }
}

void PinochleGame::printMelds(std::vector<PinochleMelds> &points){
    for(unsigned int i = 0; i<points.size();i++){
        std::cout<<points[i]<<std::endl;
    }

}

void PinochleGame::collectCards() {
    int numPlayers = hands.size();
    for (int i = 0; i < numPlayers; i++) {
        deck.collect(hands[i]);
    }
}

std::string PinochleGame::takeUserResponse() {
    std::string answer;
    std::cout << "end the game?" << std::endl;  
    std::cin >> answer;
    return answer;
}

std::ostream& operator<<(std::ostream &stream, const PinochleMelds &pinochle_melds){
    switch (pinochle_melds){
        case PinochleMelds::dix:
            stream<<"dix "<< PinochleGame::pinochlePointVals[0];
            break;
        case PinochleMelds::offsuitmarriage:
            stream<<"offsuitmarriage "<< PinochleGame::pinochlePointVals[1];
            break;
        case PinochleMelds::fortyjacks:
            stream<<"fortyjacks "<<PinochleGame::pinochlePointVals[2];
            break;
        case PinochleMelds::pinochle:
            stream<<"pinochle "<<PinochleGame::pinochlePointVals[3];
            break;
        case PinochleMelds::insuitmarriage:
            stream<<"insuitmarriage "<<PinochleGame::pinochlePointVals[4];
            break;
        case PinochleMelds::sixtyqueens:
            stream<<"sixtyqueens "<<PinochleGame::pinochlePointVals[5];
            break;
        case PinochleMelds::eightykings:
            stream<<"eightykings "<<PinochleGame::pinochlePointVals[6];
            break;
        case PinochleMelds::hundredaces:
            stream<<"hundredaces "<<PinochleGame::pinochlePointVals[7];
            break;
        case PinochleMelds::insuitrun:
            stream<<"insuitrun "<<PinochleGame::pinochlePointVals[8];
            break;
        case PinochleMelds::doublepinochle:
            stream<<"doublepinochle "<<PinochleGame::pinochlePointVals[9];
            break;
        case PinochleMelds::fourhundredjacks:
            stream<<"fourhundredjacks "<<PinochleGame::pinochlePointVals[10];
            break;
        case PinochleMelds::sixhundredqueens:
            stream<<"sixhundredqueens "<<PinochleGame::pinochlePointVals[11];
            break;
        case PinochleMelds::eighthundredkings:
            stream<<"eighthundredkings "<<PinochleGame::pinochlePointVals[12];
            break;
        case PinochleMelds::thousandaces:
            stream<<"thousandaces "<<PinochleGame::pinochlePointVals[13];
            break;
        case PinochleMelds::insuitdoublerun:
            stream<<"insuitdoublerun "<<PinochleGame::pinochlePointVals[14];
            break;
    }
    return stream;
}

void PinochleGame::suit_independent_evaluation(const CardSet<PinochleRank, Suit> &cards, std::vector<PinochleMelds> &melds){
    // have to make a copy because we access non-const iter from const CardSet &
    CardSet<PinochleRank, Suit> cards_copy(cards);

    Card<PinochleRank, Suit> AceofSpades(PinochleRank::Ace,Suit::Spades);
    Card<PinochleRank, Suit> AceofHearts(PinochleRank::Ace,Suit::Hearts);
    Card<PinochleRank, Suit> AceofDiamonds(PinochleRank::Ace,Suit::Diamonds);
    Card<PinochleRank, Suit> AceofClubs(PinochleRank::Ace,Suit::Clubs);

    Card<PinochleRank, Suit> KingofSpades(PinochleRank::King,Suit::Spades);
    Card<PinochleRank, Suit> KingofHearts(PinochleRank::King,Suit::Hearts);
    Card<PinochleRank, Suit> KingofDiamonds(PinochleRank::King,Suit::Diamonds);
    Card<PinochleRank, Suit> KingofClubs(PinochleRank::King,Suit::Clubs);

    Card<PinochleRank, Suit> QueenofSpades(PinochleRank::Queen,Suit::Spades);
    Card<PinochleRank, Suit> QueenofHearts(PinochleRank::Queen,Suit::Hearts);
    Card<PinochleRank, Suit> QueenofDiamonds(PinochleRank::Queen,Suit::Diamonds);
    Card<PinochleRank, Suit> QueenofClubs(PinochleRank::Queen,Suit::Clubs);

    Card<PinochleRank, Suit> JackofSpades(PinochleRank::Jack,Suit::Spades);
    Card<PinochleRank, Suit> JackofHearts(PinochleRank::Jack,Suit::Hearts);
    Card<PinochleRank, Suit> JackofDiamonds(PinochleRank::Jack,Suit::Diamonds);
    Card<PinochleRank, Suit> JackofClubs(PinochleRank::Jack,Suit::Clubs);

    unsigned int ace_counter = 0;
    bool spades = false;
    bool hearts = false;
    bool diamonds = false;
    bool clubs = false;
    //aces check
    for(auto it = cards_copy.get_start(); it != cards_copy.get_end(); ++it){
        if(!rankFirstCompare(AceofSpades, *it) && !rankFirstCompare(*it,AceofSpades)){
            ace_counter++;
            spades = true;
        }
        else if(!rankFirstCompare(AceofHearts, *it) && !rankFirstCompare(*it,AceofHearts)){
            ace_counter++;
            hearts = true;
        }
        else if(!rankFirstCompare(AceofDiamonds, *it) && !rankFirstCompare(*it,AceofDiamonds)){
            ace_counter++;
            diamonds = true;
        }
        else if(!rankFirstCompare(AceofClubs, *it) && !rankFirstCompare(*it,AceofClubs)){
            ace_counter++;
            clubs = true;
        }
    }

    if(ace_counter == 8){
        melds.push_back(PinochleMelds::thousandaces);
    }
    else{
        if(ace_counter>3&&spades&&hearts&&diamonds&&clubs){
            melds.push_back(PinochleMelds::hundredaces);
        }
    }


    unsigned int king_counter = 0;
    spades = false;
    hearts = false;
    diamonds = false;
    clubs = false;
    //kings check
    for(auto it = cards_copy.get_start(); it != cards_copy.get_end(); ++it){
        if(!rankFirstCompare(KingofSpades, *it) && !rankFirstCompare(*it,KingofSpades)){
            king_counter++;
            spades = true;
        }
        else if(!rankFirstCompare(KingofHearts, *it) && !rankFirstCompare(*it,KingofHearts)){
            king_counter++;
            hearts = true;
        }
        else if(!rankFirstCompare(KingofDiamonds, *it) && !rankFirstCompare(*it,KingofDiamonds)){
            king_counter++;
            diamonds = true;
        }
        else if(!rankFirstCompare(KingofClubs, *it) && !rankFirstCompare(*it,KingofClubs)){
            king_counter++;
            clubs = true;
        }
    }

    if(king_counter == 8){
        melds.push_back(PinochleMelds::eighthundredkings);
    }
    else{
        if(king_counter>3&&spades&&hearts&&diamonds&&clubs){
            melds.push_back(PinochleMelds::eightykings);
        }
    }

    unsigned int queen_counter = 0;
    spades = false;
    hearts = false;
    diamonds = false;
    clubs = false;
    //queens check
    for(auto it = cards_copy.get_start(); it != cards_copy.get_end(); ++it){
        if(!rankFirstCompare(QueenofSpades, *it) && !rankFirstCompare(*it,QueenofSpades)){
            queen_counter++;
            spades = true;
        }
        else if(!rankFirstCompare(QueenofHearts, *it) && !rankFirstCompare(*it,QueenofHearts)){
            queen_counter++;
            hearts = true;
        }
        else if(!rankFirstCompare(QueenofDiamonds, *it) && !rankFirstCompare(*it,QueenofDiamonds)){
            queen_counter++;
            diamonds = true;
        }
        else if(!rankFirstCompare(QueenofClubs, *it) && !rankFirstCompare(*it,QueenofClubs)){
            queen_counter++;
            clubs = true;
        }
    }

    if(queen_counter == 8){
        melds.push_back(PinochleMelds::sixhundredqueens);
    }
    else{
        if(queen_counter>3&&spades&&hearts&&diamonds&&clubs){
            melds.push_back(PinochleMelds::sixtyqueens);
        }
    }

    unsigned int jack_counter = 0;
    spades = false;
    hearts = false;
    diamonds = false;
    clubs = false;
    //jacks check
    for(auto it = cards_copy.get_start(); it != cards_copy.get_end(); ++it){
        if(!rankFirstCompare(JackofSpades, *it) && !rankFirstCompare(*it,JackofSpades)){
            jack_counter++;
            spades = true;
        }
        else if(!rankFirstCompare(JackofHearts, *it) && !rankFirstCompare(*it,JackofHearts)){
            jack_counter++;
            hearts = true;
        }
        else if(!rankFirstCompare(JackofDiamonds, *it) && !rankFirstCompare(*it,JackofDiamonds)){
            jack_counter++;
            diamonds = true;
        }
        else if(!rankFirstCompare(JackofClubs, *it) && !rankFirstCompare(*it,JackofClubs)){
            jack_counter++;
            clubs = true;
        }
    }

    if(jack_counter == 8){
        melds.push_back(PinochleMelds::fourhundredjacks);
    }
    else{
        if(jack_counter>3&&spades&&hearts&&diamonds&&clubs){
            melds.push_back(PinochleMelds::fortyjacks);
        }
    }


    unsigned int jd_counter = 0;
    unsigned int qs_counter = 0;
    for(auto it = cards_copy.get_start(); it != cards_copy.get_end(); ++it){
        if(!rankFirstCompare(JackofDiamonds, *it) && !rankFirstCompare(*it,JackofDiamonds)){
            jd_counter++;
        }
        else if(!rankFirstCompare(QueenofSpades, *it) && !rankFirstCompare(*it,QueenofSpades)){
            qs_counter++;
        }
    }

    if(jd_counter==2&&qs_counter==2){
        melds.push_back(PinochleMelds::doublepinochle);
    }
    else{
        if(jd_counter>0&&qs_counter>0){
        melds.push_back(PinochleMelds::pinochle);
        }
    }

    return;

}