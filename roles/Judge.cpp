// eitan.derdiger@gmail.com
#include "Judge.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

// Represents the Judge role who can undo bribe actions
Judge::Judge(Game& game, const std::string& name)
    : Player(game, name) {}

// Cancels a bonus-turn action gained from a bribe
void Judge::undoBribe(Player& target) {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (target.isEliminated()) {
        throw TargetInvalid("Cannot undo bribe: target eliminated");
    }

    auto gatherActions   = Player::findPendingOfOthers(game_, this, ActionType::GATHER, true);
    auto taxActions      = Player::findPendingOfOthers(game_, this, ActionType::TAX, true);
    auto investActions   = Player::findPendingOfOthers(game_, this, ActionType::INVEST, true);
    auto arrestActions   = Player::findPendingOfOthers(game_, this, ActionType::ARREST, true);
    auto sanctionActions = Player::findPendingOfOthers(game_, this, ActionType::SANCTION, true);
    auto coupActions     = Player::findPendingOfOthers(game_, this, ActionType::COUP, true);

    std::vector<std::pair<Player*, PendingAction*>> all;
    for (auto& v : {gatherActions, taxActions, investActions, arrestActions, sanctionActions, coupActions})
        for (auto& p : v)
            if (p.first == &target && !p.second->is_blocked) 
                all.push_back(p);

    if (all.empty()) {
        throw IllegalAction("No bonus-turn-from-bribe pending action found for " + target.getName());
    }

    all.front().second->is_blocked = true;

    changeTurn();
}
