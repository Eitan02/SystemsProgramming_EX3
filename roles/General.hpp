// eitan.derdiger@gmail.com
#ifndef COUP_GENERAL_HPP
#define COUP_GENERAL_HPP

#include "../game/Player.hpp"

namespace coup {

class General : public Player {
public:
    General(Game& game, const std::string& name);
    void preventCoup(Player& target);
};

}

#endif // COUP_GENERAL_HPP