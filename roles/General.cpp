// eitan.derdiger@gmail.com
#include "General.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

General::General(Game& g ,const std::string& n) : Player(g,n) {}

void General::preventCoup(Player& coupOwner)
{
    checkTurn();
    if (mustCoup_)            throw IllegalAction(name_+" must perform coup with 10+ coins");
    if (coins_ < 5)           throw InsufficientCoins(name_+" has insufficient coins to prevent coup");
    if (coupOwner.isEliminated()) throw TargetInvalid("Cannot prevent coup: target eliminated");

    auto allCoups = Player::findPendingOfOthers(game_, this ,ActionType::COUP ,false);

    PendingAction* toBlock = nullptr;
    for (auto& [owner ,act] : allCoups) {
        if (owner == &coupOwner && !act->is_blocked) { toBlock = act; break; }
    }

    if (!toBlock) throw IllegalAction("No pending coup initiated by "+coupOwner.getName());

    toBlock->is_blocked = true;
    coins_ -= 5;
    changeTurn();
}
