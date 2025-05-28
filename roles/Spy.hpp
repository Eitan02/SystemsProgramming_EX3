// eitan.derdiger@gmail.com
#ifndef COUP_SPY_HPP
#define COUP_SPY_HPP

#include "Player.hpp"

namespace coup {

class Spy : public Player {
public:
    Spy(Game& game, const std::string& name);
    int seeCoins(Player& target);
    void blockArrest(Player& target);
};

}

#endif // COUP_SPY_HPP