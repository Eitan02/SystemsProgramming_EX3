// eitan.derdiger@gmail.com
#include "Merchant.hpp"

using namespace coup;

// Represents the Merchant role with passive income bonus
Merchant::Merchant(Game& game, const std::string& name)
    : Player(game, name) {}

// Starts turn and grants extra coin if player has 3 or more
void Merchant::startTurn() {
    Player::startTurn();
    if (coins_ >= 3) {
        coins_ += 1;
    }
    mustCoup_ = (coins_ >= 10);
}
