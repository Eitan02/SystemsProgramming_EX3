// eitan.derdiger@gmail.com
#ifndef COUP_GENERAL_HPP
#define COUP_GENERAL_HPP

#include "../game/Player.hpp"

namespace coup {

// Represents the General role who can block coup attempts
class General : public Player {
public:
    // Constructs a General and registers to the game
    General(Game& game, const std::string& name);

    // Blocks a pending coup targeting a specific player
    void preventCoup(Player& target);
};

}

#endif // COUP_GENERAL_HPP
