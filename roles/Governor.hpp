// eitan.derdiger@gmail.com
#ifndef COUP_GOVERNOR_HPP
#define COUP_GOVERNOR_HPP

#include "../game/Player.hpp"

namespace coup {

// Represents the Governor role with enhanced tax and blocking
class Governor : public Player {
public:
    // Constructs a Governor and registers to the game
    Governor(Game& game, const std::string& name);

    // Performs a 3-coin tax action
    void tax() override;

    // Blocks a pending tax action from another player
    void blockTax(Player& target);
};

}

#endif // COUP_GOVERNOR_HPP
