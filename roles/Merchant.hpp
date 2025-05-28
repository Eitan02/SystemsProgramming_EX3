// eitan.derdiger@gmail.com
#ifndef COUP_MERCHANT_HPP
#define COUP_MERCHANT_HPP

#include "../game/Player.hpp"

namespace coup {

class Merchant : public Player {
public:
    Merchant(Game& game, const std::string& name);
    void startTurn() override;
};

}

#endif // COUP_MERCHANT_HPP