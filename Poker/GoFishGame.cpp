// GoFishGame.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
//This file defines gameplay for GoFishGame with its constructor, play, turn, and deal functions

#define MIN_PLAYERS 2
#define MIN_RANKS 4
#define FOUR_OF_A_KIND 4
#define MULTIPLAYER_CARDNUM 5
#define CARDS_EACH_LINE 6
#define TWO_PLAYER_CARDNUM 7
#define CLI_ARGS 3
#define INDEX_PLAYERNUM_OFFSET 1

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <type_traits>

#include "GoFishGame.h"

template <typename S, typename R, typename D>
GoFishGame<S, R, D>::GoFishGame(int num_players, const char* player_names[]): Game(num_players, player_names){
    books = (std::vector<CardSet<R, S>>(num_players - CLI_ARGS));
    hands = (std::vector<CardSet<R, S>>(num_players - CLI_ARGS));
    book_nums = (std::vector<int>(num_players - CLI_ARGS));

    // check if the deck has at least 4 of each rank
    for (R r = static_cast<R>(0); r < R::undefined; ++r) {
        unsigned num = count_if(deck.get_start(), deck.get_end(), [r](const Card<R, S> & card) {
            return card.rank == r;
        });
        if (num < MIN_RANKS) {
            throw std::runtime_error("GoFishGame constructor: deck needs to have at least 4 of each rank");
        }
    }

    for (int i = 0; i < num_players; i++) {
        this->player_names.push_back(std::string(player_names[i]));
    }
}

template <typename S, typename R, typename D>
GoFishGame<S, R, D>::~GoFishGame() {

}

template <typename S, typename R, typename D>
int GoFishGame<S, R, D>::play() {

    deal();

    auto canCollectBooks = true;
    for(unsigned int i = 0; i<hands.size();i++){
        canCollectBooks = true;
        while(canCollectBooks){
            canCollectBooks = collect_books(i+1);
        }
    }

    int round_num = 1;
    
    bool last_round = false;
    
    while(finished_players.size() < hands.size() - INDEX_PLAYERNUM_OFFSET){
        
        std::cout<<""<<std::endl;
        std::cout<<"IT IS ROUND #"<<round_num<<std::endl;
        std::cout<<""<<std::endl;

        round_num++;

        for(unsigned int i = 0; i<hands.size();i++){
            if(finished_players.find(i) != finished_players.end()){
                continue;
            }

            while (turn(i+1)) {}
        }

        for(unsigned int i = 0; i<hands.size(); i++){
            std::cout << "player " << i+1 << " has " <<book_nums[i]<<" books"<<std::endl;
        }


        //each player still gets a chance to steal after the deck is emptied
        if(deck.is_empty()&&!last_round){
            last_round = true;
        }

        //once deck is empty and all players have gotten a turn the game is over
        if(deck.is_empty()&&last_round){
            break;
        }
        

    }
    std::cout<<""<<std::endl;
    std::cout<<"GAME OVER"<<std::endl;
    std::cout<<""<<std::endl;

    int max_books = 0;
    for(unsigned int i = 0; i<books.size();i++){
        if(book_nums[i]>max_books){
            max_books=book_nums[i];
        }
    }


    bool one_winner = true;
    for(unsigned int i = 0; i<books.size();i++){
        if(book_nums[i]==max_books){
            if(one_winner){
                std::cout<<"PLAYER "<<i+1<<" WINS WITH "<<book_nums[i]<<" BOOKS";
                one_winner = false;
            }
            else{
                std::cout<<" AND PLAYER "<<i+1<<" WINS WITH "<<book_nums[i]<<" BOOKS";
            }
            
        }
    }
    
    return 1;
}

template <typename S, typename R, typename D>
void GoFishGame<S, R, D>::deal() {
    deck.shuffle();
    int numPlayers = hands.size();
    if (numPlayers < MIN_PLAYERS) {
        return;
    }
    if (numPlayers == MIN_PLAYERS) {
        for (int i = 0; i < TWO_PLAYER_CARDNUM && !deck.is_empty(); i++) {
            deck >> hands[0];
            deck >> hands[1];
        }
    }
    else{
        for(int j = 0; j < MULTIPLAYER_CARDNUM; j++){
            for (int i = 0; i < numPlayers && !deck.is_empty(); i++) {
                deck >> hands[i];
            }
        } 
    }
}

template <typename S, typename R, typename D>
bool GoFishGame<S, R, D>::collect_books(int player_num){
    CardSet<R, S> & player_hand = hands[player_num - INDEX_PLAYERNUM_OFFSET];
    std::vector<int> rank_counts(static_cast<int>(R::undefined));
    for(auto iter = player_hand.get_start();iter!=player_hand.get_end();iter++){
        Card<R,S> curr_card = *iter;
        R curr_rank = curr_card.rank;
        int idx = static_cast<int>(curr_rank);
        rank_counts[idx]++;
    }

    for(unsigned int i = 0; i<rank_counts.size();i++){
        if(rank_counts[i] >= FOUR_OF_A_KIND){
            //In case hand is initialized with five or more of a kind
            int max_four = 0;
            auto predicate = [i, max_four](Card<R, S>& curr_card) mutable ->bool{
                R the_rank = static_cast<R>(i);
                if(the_rank == curr_card.rank&&max_four<4){
                    max_four++;
                    return true;
                }
                else{
                    return false;
                }
            };
            books[player_num - INDEX_PLAYERNUM_OFFSET].collect_if(player_hand, predicate);
            book_nums[player_num - INDEX_PLAYERNUM_OFFSET]++;
            return true;
        }
    }

    return false;
}

template <typename S, typename R, typename D>
bool GoFishGame<S, R, D>::turn(int player_num) {

    int idx = player_num - 1;

    for(unsigned int i = 0; i<hands.size(); i++){
        std::cout << "player " << i+1 << " hand: " << std::endl;
        hands[i].print(std::cout, CARDS_EACH_LINE);
        std::cout << "books: " << std::endl;
        books[i].print(std::cout, CARDS_EACH_LINE);
        std::cout<<""<<std::endl;
    }
    
    std::cout<<"START OF PLAYER #"<<player_num<<"'s TURN"<<std::endl;

    std::string rank_str;
    std::string from_player_str;

    int rank_num;
    int from_player_num;

    while(true){
        std::cout << "Ask for card of rank: ";
        std::cin >> rank_str;

        try{
            rank_num = std::stoi(rank_str);
        }
        catch (std::invalid_argument const& ex){
            std::cout << "Please input rank in digit form"<<std::endl;
            continue;
        }
        
        if (std::is_same<R, PokerRank>::value) {
            // if poker rank enum
            rank_num -= 2;
        } else if (std::is_same<R, PinochleRank>::value) {
            // if pinochle rank
            switch (rank_num) {
                case 9:
                    rank_num = 0;
                    break;
                case 10:
                    rank_num = 4;
                    break;
                case 11:
                    rank_num = 1;
                    break;
                case 12:
                    rank_num = 2;
                    break;
                case 13:
                    rank_num = 3;
                    break;
                case 14:
                    rank_num = 5;
                    break;
                default:
                    rank_num = 100;
                    break;
            }
        }

        if(rank_num < 0 || (unsigned) rank_num >= static_cast<int>(R::undefined)){
            std::cout << "Please input valid rank" <<std::endl;
            continue;
        }
        
        
        R curr_rank = static_cast<R>(rank_num);
        
        auto it = std::find_if(hands[idx].get_start(), hands[idx].get_end(), [curr_rank](Card<R,S> card){
            return card.rank == curr_rank;
        });

        //If player's hand is empty, rank requested need not be in hand
        //...because there are zero cards in their hand
        if (it == hands[idx].get_end()&& !hands[idx].is_empty()){
            std::cout << "Please input rank currently in hand"<<std::endl;
            continue;
        }

        std::cout << "Ask from player: ";
        std::cin >> from_player_str;

        try{
            from_player_num = std::stoi(from_player_str);
        }
        catch (std::invalid_argument const& ex){
            std::cout << "Please input player number in digit form"<<std::endl;
            continue;
        }

        if(from_player_num == player_num){
            std::cout << "Cannot fish for card from yourself"<<std::endl;
            continue;
        }

        if(!(from_player_num >= 1 && (unsigned) from_player_num <= hands.size())){
            std::cout << "Please input valid player number between "<<1<<" and "<<hands.size()<<std::endl;
            continue;
        }

        break;
    }

    
    
    if (hands[idx].request(hands[from_player_num - INDEX_PLAYERNUM_OFFSET], static_cast<R>(rank_num))) {
        std::cout<<""<<std::endl;
        std::cout<<"RANK SUCCESSFULLY REQUESTED WAS "<<static_cast<R>(rank_num)<<std::endl;
        std::cout<<""<<std::endl;
        while (collect_books(player_num)) {}
        return true;
    } else {
        if (deck.is_empty()) {
            std::cout << "Player " << player_num << " is removed from the game" << std::endl;
            finished_players.insert(player_num - INDEX_PLAYERNUM_OFFSET);
            deck.collect(hands[player_num - INDEX_PLAYERNUM_OFFSET]);
            return false;
        } else {
            std::cout<<""<<std::endl;
            std::cout<<"GO FISH"<<std::endl;
            std::cout<<""<<std::endl;
            deck >> hands[idx];
            if ((*(--hands[idx].get_end())).rank == static_cast<R>(rank_num)) {
                std::cout<<"RANK SUCCESSFULLY FISHED WAS "<<static_cast<R>(rank_num)<<std::endl;
                std::cout<<""<<std::endl;
                while (collect_books(player_num)) {}
                return true;
            } else {
                while (collect_books(player_num)) {}
                return false;
            }
        }
    }


    

    return true;
}