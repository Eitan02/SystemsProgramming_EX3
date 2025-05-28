// eitan.derdiger@gmail.com
#ifndef COUP_GOVERNOR_HPP
#define COUP_GOVERNOR_HPP

#include "../game/Player.hpp"

namespace coup {

class Governor : public Player {
public:
    Governor(Game& game, const std::string& name);
    void tax() override;
    void blockTax(Player& target);
};

}

#endif // COUP_GOVERNOR_HPP