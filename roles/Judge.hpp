// eitan.derdiger@gmail.com
#ifndef COUP_JUDGE_HPP
#define COUP_JUDGE_HPP

#include "../game/Player.hpp"

namespace coup {

// Represents the Judge role who can cancel bribes
class Judge : public Player {
public:
    // Constructs a Judge and registers to the game
    Judge(Game& game, const std::string& name);

    // Cancels a bonus-turn action caused by a bribe
    void undoBribe(Player& target);
};

}

#endif // COUP_JUDGE_HPP
