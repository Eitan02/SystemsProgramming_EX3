// eitan.derdiger@gmail.com
#ifndef COUP_BARON_HPP
#define COUP_BARON_HPP

#include "../game/Player.hpp"

namespace coup {

// Represents the Baron role with a special invest ability
class Baron : public Player {
public:
    // Constructs a Baron and registers to the game
    Baron(Game& game, const std::string& name);

    // Allows the Baron to invest 3 coins to gain 6
    void invest();
};

}

#endif // COUP_BARON_HPP
