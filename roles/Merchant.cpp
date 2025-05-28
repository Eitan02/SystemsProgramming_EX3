// eitan.derdiger@gmail.com
#include "Merchant.hpp"

using namespace coup;

Merchant::Merchant(Game& game, const std::string& name)
    : Player(game, name) {}

void Merchant::startTurn() {
    Player::startTurn(); // Call base class startTurn for common logic
    if (coins_ >= 3) { // Merchant passive: starts turn with 3+ coins, gains 1 extra
        coins_ += 1;
    }
    mustCoup_ = (coins_ >= 10); // Re-evaluate mustCoup_ after potential passive gain
}