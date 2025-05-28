// eitan.derdiger@gmail.com
#include "General.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

// Represents the General role with coup-blocking ability
General::General(Game& game, const std::string& name)
    : Player(game, name) {}

// Cancels a pending coup against a specific player
void General::preventCoup(Player& target) {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (coins_ < 5) {
        throw InsufficientCoins(name_ + " has insufficient coins to prevent coup");
    }
    if (target.isEliminated()) {
        throw TargetInvalid("Cannot prevent coup: target is eliminated");
    }

    auto coups = Player::findPendingOfOthers(game_, this, ActionType::COUP, false);

    std::pair<Player*, PendingAction*> match = {nullptr, nullptr};
    for (auto& p : coups) {
        if (p.second->target == &target && !p.second->is_blocked) {
            match = p;
            break;
        }
    }

    if (!match.second) {
        throw IllegalAction("No pending coup found against " + target.getName());
    }

    match.second->is_blocked = true;
    coins_ -= 5;
    changeTurn();
}
