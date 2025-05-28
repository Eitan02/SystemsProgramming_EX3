// eitan.derdiger@gmail.com
#ifndef COUP_SPY_HPP
#define COUP_SPY_HPP

#include "../game/Player.hpp"

namespace coup {

// Represents the Spy role who can reveal coins and block arrests
class Spy : public Player {
public:
    // Constructs a Spy and registers to the game
    Spy(Game& game, const std::string& name);

    // Returns the number of coins the target has
    int seeCoins(Player& target);

    // Blocks arrest attempts on the given target
    void blockArrest(Player& target);
};

}

#endif // COUP_SPY_HPP
