// eitan.derdiger@gmail.com
#include "Governor.hpp"
#include "../exceptions/Exceptions.hpp"
#include <algorithm>

using namespace coup;

// Represents the Governor role with extended tax and block abilities
Governor::Governor(Game& game, const std::string& name)
    : Player(game, name) {}

// Performs a tax action that gives 3 coins instead of 2
void Governor::tax() {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (sanctionCount_ > 0) {
        throw ActionBlocked(name_ + " is sanctioned and cannot tax");
    }

    if (isBonusTurn()) {
        pendingActions_.push_back(PendingAction{ ActionType::TAX_Governor, this, nullptr, false, isBonusTurn() });
    } else {
        coins_ += 3;
    }

    changeTurn();
}

// Blocks a pending tax action from another player
void Governor::blockTax(Player& target) {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (target.isEliminated()) {
        throw TargetInvalid("Cannot block tax: target eliminated");
    }

    std::vector<std::pair<Player*, PendingAction*>> taxActions;
    
    auto regularTax = Player::findPendingOfOthers(game_, this, ActionType::TAX, false);
    auto govTax     = Player::findPendingOfOthers(game_, this, ActionType::TAX_Governor, false);

    for (auto& p : regularTax) if (p.first == &target && !p.second->is_blocked) taxActions.push_back(p);
    for (auto& p : govTax)     if (p.first == &target && !p.second->is_blocked) taxActions.push_back(p);

    if (taxActions.empty()) {
        throw IllegalAction("No pending tax action found for " + target.getName());
    }

    PendingAction* actionToBlock = taxActions.front().second;
    actionToBlock->is_blocked = true;

    changeTurn();
}
