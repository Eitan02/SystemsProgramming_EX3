// eitan.derdiger@gmail.com
#ifndef COUP_PLAYER_HPP
#define COUP_PLAYER_HPP

#include <string>
#include "Game.hpp"
#include "../exceptions/Exceptions.hpp"

namespace coup {

enum class ActionType { TAX, BRIBE, COUP, INVEST, GATHER, ARREST, SANCTION, TAX_Governor };

struct PendingAction {
    ActionType type;
    Player* source;
    Player* target;
    bool is_blocked = false;
    bool from_bribe = false;
};

class Player {
public:
    Player(Game& game, const std::string& name);
    Player(const Player& other);
    Player& operator=(const Player& other);
    virtual ~Player();

    std::string getName() const;
    int getCoins() const;
    bool isEliminated() const;
    int getArrestBlockedCount() const;

    void setCoins(int coins);
    void setEliminated(bool eliminated);
    void setArrestBlockedCount(int count);

    virtual void startTurn();
    virtual void tax();
    void gather();
    void bribe();
    void arrest(Player& target);
    void sanction(Player& target);
    void coup(Player& target);
    void executePendingAction(const PendingAction &action);

    static std::vector<std::pair<Player*, PendingAction*>> findPendingOfOthers(Game& game, Player* exclude, ActionType type, bool onlyBribe = false);

protected:
    void checkTurn() const;
    void changeTurn();
    bool isBonusTurn() const;

    Game& game_;
    std::string name_;
    int coins_;
    bool eliminated_;
    Player* lastArrestTarget_;
    int extraTurns_;
    std::vector<PendingAction> pendingActions_;
    int sanctionCount_;
    int arrestBlockedCount_;
    bool mustCoup_;
    int turnsSinceLastBribe_;
};

}

#endif // COUP_PLAYER_HPP