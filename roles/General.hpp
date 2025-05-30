// eitan.derdiger@gmail.com
#pragma once
#include "../game/Player.hpp"

namespace coup {

class General : public Player {
public:
    General(Game& g ,const std::string& n);
    void preventCoup(Player& coupOwner);

    std::string getRole() const override { return "General"; }

};

} // namespace coup
