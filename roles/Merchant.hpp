// eitan.derdiger@gmail.com
#ifndef COUP_MERCHANT_HPP
#define COUP_MERCHANT_HPP

#include "../game/Player.hpp"

namespace coup {

// Represents the Merchant role who gains bonus coin at start of turn
class Merchant : public Player {
public:
    // Constructs a Merchant and registers to the game
    Merchant(Game& game, const std::string& name);

    // Starts the turn and adds 1 coin if passive condition met
    void startTurn() override;
};

}

#endif // COUP_MERCHANT_HPP
