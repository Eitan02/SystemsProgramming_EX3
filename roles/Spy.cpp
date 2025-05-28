// eitan.derdiger@gmail.com
#include "Spy.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

Spy::Spy(Game& game, const std::string& name)
    : Player(game, name) {}

int Spy::seeCoins(Player& target) {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (target.isEliminated()) {
        throw TargetInvalid("Cannot see coins of eliminated player");
    }
    int targetCoins = target.getCoins();
    return targetCoins;
}

void Spy::blockArrest(Player& target) {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (target.isEliminated()) {
        throw TargetInvalid("Cannot block arrest for eliminated player");
    }
    target.setArrestBlockedCount(2);
    changeTurn();
}