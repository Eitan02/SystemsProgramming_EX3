// eitan.derdiger@gmail.com
#include "Baron.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

// Represents a Baron role with investment ability
Baron::Baron(Game& game, const std::string& name)
    : Player(game, name) {}

// Performs an investment action: trade 3 coins for 6
void Baron::invest() {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (coins_ < 3) {
        throw InsufficientCoins(name_ + " has insufficient coins to invest");
    }

    if (isBonusTurn()) {
        pendingActions_.push_back(PendingAction{ ActionType::INVEST, this, nullptr, false, isBonusTurn() });
    } else {
        coins_ -= 3;
        coins_ += 6;
    }

    changeTurn();
}
