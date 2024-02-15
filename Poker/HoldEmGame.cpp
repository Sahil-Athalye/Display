// HoldEmGame.cpp
// authors: Sahil Athalye
//          Yuheng Ding
//          Reedham Kalariya
// This files defines class methods for HoldEmGame

#define HAND_SIZE 5
#define NUM_POKERRANK 13

#include "HoldEmGame.h"

#include <iostream>
#include <algorithm>
#include <vector>

HoldEmGame::HoldEmGame(int argc, const char * argv[]) : Game(argc, argv) {
    for (int i = 2; i < argc; i++) {
        hands.push_back(CardSet<PokerRank, Suit>());
    }
}

void HoldEmGame::deal() {
    int numPlayers = hands.size();
    if (numPlayers == 0) {
        return;
    }

    switch (currstate){
        case HoldEmState::preflop:
        for (int i = 0; i < numPlayers && !currdeck.is_empty(); i++) {
            currdeck >> hands[i];
        }
        for (int i = 0; i < numPlayers && !currdeck.is_empty(); i++) {
            currdeck >> hands[i];
        }      
        currstate = HoldEmState::flop;
        break;

        case HoldEmState::flop:
        for (int i = 0; i < 3 && !currdeck.is_empty(); i++) {
            currdeck >> board;
        }
        currstate = HoldEmState::turn;      
        break;

        case HoldEmState::turn:
        for (int i = 0; i < 1 && !currdeck.is_empty(); i++) {
            currdeck >> board;
        }
        currstate = HoldEmState::river;  
        break;

        case HoldEmState::river:
        for (int i = 0; i < 1 && !currdeck.is_empty(); i++) {
            currdeck >> board;
        }
        currstate = HoldEmState::undefined;         
        break;

        case HoldEmState::undefined:
        break;
    }
}

int HoldEmGame::play(){
    unsigned numPlayers = hands.size();
    std::string answer;

    do {
        currdeck.shuffle();
        currstate = HoldEmState::preflop;

        try {
            deal(); //deal cards to players
        } catch (const std::runtime_error& error) {
            std::cout << error.what() << std::endl;
            return -1;
        }

        printHands();

        try {
            deal(); //deal flop
        } catch (const std::runtime_error& error) {
            std::cout << error.what() << std::endl;
            return -1;
        }
        
        std::cout << "BOARD (flop):" << std::endl;
        board.print(std::cout, 5);

        std::vector<Player> players;
        for (unsigned i = 0; i < numPlayers; i++) {
            players.push_back(Player(hands[i], playerNames[i], HoldEmHandRank::undefined));
        }

        for (unsigned i = 0; i < numPlayers; i++) {
            CardSet<PokerRank, Suit> boardCopy(board);
            while (!boardCopy.is_empty()) {
                boardCopy >> players[i].hand;
            }
            players[i].hand.sort_cardvec(); // sort the cardset by rank for better display
            players[i].handRank = holdem_hand_evaluation(players[i].hand);
        }

        std::sort(players.begin(), players.end());
        std::reverse(players.begin(), players.end());
        std::cout << "Player rank: (highest to lowest hand): " << std::endl;
        for (unsigned i = 0; i < numPlayers; i++) {
            std::cout << players[i].playerName << " " << players[i].handRank;
            players[i].hand.print(std::cout, 5);
        }


        try {
            deal(); //deal turn
        } catch (const std::runtime_error& error) {
            std::cout << error.what() << std::endl;
            return -1;
        }

        std::cout << "BOARD (turn):" << std::endl;
        board.print(std::cout, 5);

        try {
            deal(); //deal river
        } catch (const std::runtime_error& error) {
            std::cout << error.what() << std::endl;
            return -1;
        }

        std::cout << "BOARD (river):" << std::endl;
        board.print(std::cout, 5);

        collectCards();

        answer = takeUserResponse();

    } while (answer!="yes");

    return 0;
}

void HoldEmGame::printHands() {
    int numPlayers = hands.size();
    for (int i = 0; i < numPlayers; i++) {
        std::string name = playerNames[i];
        std::cout << name << std::endl;
        hands[i].print(std::cout, 8);

    }
}

void HoldEmGame::collectCards() {
    int numPlayers = hands.size();
    for (int i = 0; i < numPlayers; i++) {
        currdeck.collect(hands[i]);
    }
    currdeck.collect(board);
}

std::string HoldEmGame::takeUserResponse() {
    std::string answer;
    std::cout << "end the game?" << std::endl;  
    std::cin >> answer;
    return answer;
}

std::ostream& operator<<(std::ostream &stream,
    const HoldEmHandRank &hand_rank){
    switch (hand_rank){
        case HoldEmHandRank::xhigh:
            stream<<"XHigh ";
            break;
        case HoldEmHandRank::pair:
            stream<<"Pair ";
            break;
        case HoldEmHandRank::twopair:
            stream<<"Two Pair ";
            break;
        case HoldEmHandRank::threeofakind:
            stream<<"Three of a Kind ";
            break;
        case HoldEmHandRank::straight:
            stream<<"Straight ";
            break;
        case HoldEmHandRank::flush:
            stream<<"Flush ";
            break;
        case HoldEmHandRank::fullhouse:
            stream<<"Full House ";
            break;
        case HoldEmHandRank::fourofakind:
            stream<<"Four of a Kind ";
            break;
        case HoldEmHandRank::straightflush:
            stream<<"Straight Flush ";
            break;
        case HoldEmHandRank::undefined:
            stream<<"undefined ";
            break;
    }
    return stream;
}

HoldEmHandRank HoldEmGame::holdem_hand_evaluation(
    const CardSet<PokerRank, Suit>& playerHand
    ) {
    // make a copy of playerHand because it's const &
    CardSet<PokerRank, Suit> hand(playerHand);

    hand.sort_cardvec();

    std::vector<Card<PokerRank, Suit>> cards(hand.get_start(), hand.get_end());
    
    if (hand.get_end() - hand.get_start() != HAND_SIZE) {
        return HoldEmHandRank::undefined;
    }

    bool allSameSuit = true;

    for (auto prev = hand.get_start(), it = prev + 1; it != hand.get_end(); ++prev, ++it) {
        if (it->suit != prev->suit) {
            allSameSuit = false;
            break;
        }
    }

    bool consecutive = true;
    for (auto prev = hand.get_start(), it = prev + 1;  it != hand.get_end(); ++prev, ++it) {
        if ((int) it->rank != (int) prev->rank + 1) {
            consecutive = false;
            break;
        }
    }

    // special case: 2 3 4 5 A
    auto it = hand.get_start();
    if (it->rank == PokerRank::Two
    && (++it)->rank == PokerRank::Three
    && (++it)->rank == PokerRank::Four
    && (++it)->rank == PokerRank::Five
    && (++it)->rank == PokerRank::Ace) {
        consecutive = true;
    }

    // count number of each rank in the hand
    std::vector<int> rankCnt(NUM_POKERRANK, 0);
    for (auto it = hand.get_start(); it != hand.get_end(); ++it) {
        if (it->rank != PokerRank::undefined) {
            rankCnt[(int) it->rank]++;
        }
    }
    
    bool fourofakind = false;
    bool threeofakind = false;
    int twoPairs = 0;
    for (unsigned i = 0; i < NUM_POKERRANK; i++) {
        if (rankCnt[i] == 4) {
            fourofakind = true;
        } else if (rankCnt[i] == 3) {
            threeofakind = true;
        } else if (rankCnt[i] == 2) {
            twoPairs++;
        }
    }

    if (allSameSuit && consecutive) {
        return HoldEmHandRank::straightflush;
    } else if (fourofakind) {
        return HoldEmHandRank::fourofakind;
    } else if (threeofakind && twoPairs == 1) {
        return HoldEmHandRank::fullhouse;
    } else if (allSameSuit) {
        return HoldEmHandRank::flush;
    } else if (consecutive) {
        return HoldEmHandRank::straight;
    } else if (threeofakind) {
        return HoldEmHandRank::threeofakind;
    } else if (twoPairs == 2) {
        return HoldEmHandRank::twopair;
    } else if (twoPairs == 1) {
        return HoldEmHandRank::pair;
    }

    return HoldEmHandRank::xhigh;
}

HoldEmGame::Player::Player(const CardSet<PokerRank, Suit> & oHand, 
    const std::string & oName, const HoldEmHandRank & oRank)
    : hand(oHand), playerName(oName), handRank(oRank) {}

bool operator<(const HoldEmGame::Player &player1,
const HoldEmGame::Player &player2) {
    if (player1.handRank != player2.handRank) {
        return player1.handRank < player2.handRank;
    }

    HoldEmGame::Player p1(player1);
    p1.hand.sort_cardvec();
    HoldEmGame::Player p2(player2);
    p2.hand.sort_cardvec();

    CardSet<PokerRank, Suit> & hand1 = p1.hand;
    CardSet<PokerRank, Suit> & hand2 = p2.hand;

    // both hands have same pattern
    if (p1.handRank == HoldEmHandRank::pair) {
        // first step: find card rank for the (only) pair in each hand
        PokerRank player1pairRank = PokerRank::undefined;
        PokerRank prev = PokerRank::undefined;
        for (auto it = hand1.get_start(); it != hand2.get_end(); ++it) {
            if (it->rank == prev) {
                player1pairRank = it->rank;
                break;
            }
            prev = it->rank;
        }

        PokerRank player2pairRank = PokerRank::undefined;
        prev = PokerRank::undefined;
        for (auto it = hand2.get_start(); it != hand2.get_end(); ++it) {
            if (it->rank == prev) {
                player2pairRank = it->rank;
                break;
            }
            prev = it->rank;
        }

        if (player1pairRank != player2pairRank) {
            return player1pairRank < player2pairRank;
        }

        // find highest non-pair card in each hand
        // (since they are sorted)
        // ignore the pair
        auto curr1 = hand1.get_end() - 1;
        auto curr2 = hand2.get_end() - 1;
        while (true) {
            while (curr1->rank == player1pairRank) {
                if (curr1 == hand1.get_start()) {
                    break;
                }
                --curr1;
                continue;
            }
            while (curr2->rank == player2pairRank) {
                if (curr2 == hand2.get_start()) {
                    break;
                }
                --curr2;
                continue;
            }
            if (curr1->rank != curr2->rank) {
                return curr1->rank < curr2->rank;
            }
            if (curr1 == hand1.get_start() || curr2 == hand2.get_start()) {
                break;
            }
            --curr1;
            --curr2;
        }

        return false;
    } else if (player1.handRank == HoldEmHandRank::twopair) {
        // find ranks of each pair and rank of the single card
        std::vector<PokerRank> pairRanks1;
        std::vector<PokerRank> pairRanks2;
        PokerRank single1 = PokerRank::undefined;
        PokerRank single2 = PokerRank::undefined;

        for (auto prev = hand1.get_start(), it = prev + 1; it != hand1.get_end(); ++prev, ++it) {
            if (it->rank == prev->rank) {
                pairRanks1.insert(pairRanks1.begin(), it->rank);
                break;
            }
        }
        for (auto it = hand1.get_start(); it != hand1.get_end(); ++it) {
            bool add = true;
            for (PokerRank r : pairRanks1) {
                if (it->rank == r) {
                    add = false;
                    break;
                }
            }
            if (add) {
                single1 = it->rank;
            }
        }

        
        for (auto prev = hand2.get_start(), it = prev + 1; it != hand2.get_end(); ++prev, ++it) {
            if (it->rank == prev->rank) {
                pairRanks2.insert(pairRanks2.begin(), it->rank);
                break;
            }
        }
        for (auto it = hand2.get_start(); it != hand2.get_end(); ++it) {
            bool add = true;
            for (PokerRank r : pairRanks2) {
                if (it->rank == r) {
                    add = false;
                    break;
                }
            }
            if (add) {
                single2 = it->rank;
            }
        }

        for (unsigned i = 0; i < pairRanks1.size(); i++) {
            if (pairRanks1[i] != pairRanks2[i]) {
                return pairRanks1[i] < pairRanks2[i];
            }
        }

        return single1 < single2;
    } else if (player1.handRank == HoldEmHandRank::threeofakind) {
        // same logic as (single) pair
        // (there can't be another pair because 
        // otherwise it would have been a fullhouse)
        // first step: find card rank for the triple in each hand
        PokerRank three_rank1 = PokerRank::undefined;
        for (auto prev = hand1.get_start(), it = prev + 1; it != hand1.get_end(); ++prev, ++it) {
            if (it->rank != prev->rank) {
                three_rank1 = it->rank;
                break;
            }
        }

        PokerRank three_rank2 = PokerRank::undefined;
        for (auto prev = hand2.get_start(), it = prev + 1; it != hand2.get_end(); ++prev, ++it) {
            if (it->rank != prev->rank) {
                three_rank2 = it->rank;
                break;
            }
        }
        if (three_rank1 != three_rank2) {
            return three_rank1 < three_rank2;
        }

        // iterate through two card vectors from end to beginning 
        // (since they are sorted)
        // ignore the triple
        auto curr1 = hand1.get_end() - 1;
        auto curr2 = hand2.get_end() - 1;
        while (true) {
            while (curr1->rank == three_rank1) {
                if (curr1 == hand1.get_start()) {
                    break;
                }
                --curr1;
                continue;
            }
            while (curr2->rank == three_rank2) {
                if (curr2 == hand2.get_start()) {
                    break;
                }
                --curr2;
                continue;
            }
            if (curr1->rank != curr2->rank) {
                return curr1->rank < curr2->rank;
            }
            if (curr1 == hand1.get_start() || curr2 == hand2.get_start()) {
                break;
            }
            --curr1;
            --curr2;
        }

        return false;
    } else if (player1.handRank == HoldEmHandRank::straight
        || player1.handRank == HoldEmHandRank::straightflush) {
        PokerRank first1 = hand1.get_start()->rank;
        PokerRank first2 = hand2.get_start()->rank;
        PokerRank last1 = (hand1.get_end() - 1)->rank;
        PokerRank last2 = (hand2.get_end() - 1)->rank;
        if (first1 != first2) {
            return first1 < first2;
        } else {
            if (first1 == PokerRank::Two) {
                return last1 == PokerRank::Six
                    && last2 != PokerRank::Six;
            }
            return false;
        }
    } else if (player1.handRank == HoldEmHandRank::flush 
    || player1.handRank == HoldEmHandRank::xhigh) {
        for (auto it1 = hand1.get_end() - 1, it2 = hand2.get_end() - 1;
            it1 > hand1.get_start() && it2 > hand2.get_start(); --it1, --it2) {
            if (it1->rank != it2->rank) {
                return it1->rank < it2->rank;
            }
        }
        return hand1.get_start()->rank < hand2.get_start()->rank;
    } else if (player1.handRank == HoldEmHandRank::fullhouse) {
        // figure out rank for three of a kind and other
        // fullhouse (after sorting) can either be 
        // XXXOO or OOXXX (can't be OXXXO),
        // so check middle is enough
        auto midit1 = hand1.get_start() + (hand1.get_end() - hand1.get_start()) / 2;
        auto midit2 = hand1.get_start() + (hand2.get_end() - hand2.get_start()) / 2;
        PokerRank threeRank1 = midit1->rank;
        PokerRank twoRank1 =
            hand1.get_start()->rank != threeRank1
            ? hand1.get_start()->rank
            : (hand1.get_end() - 1)->rank;
        PokerRank threeRank2 = midit2->rank;
        PokerRank twoRank2 =
            hand2.get_start()->rank != threeRank2
            ? hand2.get_start()->rank
            : (hand2.get_end() - 1)->rank;

        if (threeRank1 != threeRank2) {
            return threeRank1 < threeRank2;
        }

        return twoRank1 < twoRank2;
        
    } else if (player1.handRank == HoldEmHandRank::fourofakind) {
        // same logic as (single) pair
        // (there can't be another pair because 
        // otherwise it would have been a fullhouse)
        // first step: find card rank for the triple in each hand
        PokerRank four_rank1 = PokerRank::undefined;
        for (auto prev = hand1.get_start(), it = prev + 1; it != hand1.get_end(); ++prev, ++it) {
            if (it->rank != prev->rank) {
                four_rank1 = it->rank;
                break;
            }
        }

        PokerRank four_rank2 = PokerRank::undefined;
        for (auto prev = hand2.get_start(), it = prev + 1; it != hand2.get_end(); ++prev, ++it) {
            if (it->rank != prev->rank) {
                four_rank2 = it->rank;
                break;
            }
        }
        if (four_rank1 != four_rank2) {
            return four_rank1 < four_rank2;
        }

        // iterate through two card vectors from end to beginning 
        // (since they are sorted)
        // ignore the four (of a kind)
        auto curr1 = hand1.get_end() - 1;
        auto curr2 = hand2.get_end() - 1;
        while (true) {
            while (curr1->rank == four_rank1) {
                if (curr1 == hand1.get_start()) {
                    break;
                }
                --curr1;
                continue;
            }
            while (curr2->rank == four_rank2) {
                if (curr2 == hand2.get_start()) {
                    break;
                }
                --curr2;
                continue;
            }
            if (curr1->rank != curr2->rank) {
                return curr1->rank < curr2->rank;
            }
            if (curr1 == hand1.get_start() || curr2 == hand2.get_start()) {
                break;
            }
            --curr1;
            --curr2;
        }

        return false;

    }



    return false; // default (should never reach here)
}