// eitan.derdiger@gmail.com
#ifndef COUP_BARON_HPP
#define COUP_BARON_HPP

#include "../game/Player.hpp"

namespace coup {

class Baron : public Player {
public:
    Baron(Game& game, const std::string& name);
    void invest();
};

}

#endif // COUP_BARON_HPP