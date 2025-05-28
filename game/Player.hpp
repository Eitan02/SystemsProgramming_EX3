// eitan.derdiger@gmail.com
#ifndef COUP_PLAYER_HPP
#define COUP_PLAYER_HPP

#include <string>
#include "Game.hpp"
#include "../exceptions/Exceptions.hpp"

namespace coup {

// Types of actions that players can perform
enum class ActionType { TAX, BRIBE, COUP, INVEST, GATHER, ARREST, SANCTION, TAX_Governor };

// Holds information about a pending player action
struct PendingAction {
    ActionType type;
    Player* source;
    Player* target;
    bool is_blocked = false;
    bool from_bribe = false;
};

// Represents a player in the game
class Player {
public:
    // Constructs a new player and registers to the game
    Player(Game& game, const std::string& name);

    // Copy constructor
    Player(const Player& other);

    // Assignment operator
    Player& operator=(const Player& other);

    // Destructor
    virtual ~Player();

    // Returns player's name
    std::string getName() const;

    // Returns coin count
    int getCoins() const;

    // Returns whether the player is eliminated
    bool isEliminated() const;

    // Gets arrest protection count
    int getArrestBlockedCount() const;

    // Sets coin count
    void setCoins(int coins);

    // Marks player as eliminated or not
    void setEliminated(bool eliminated);

    // Sets arrest block counter
    void setArrestBlockedCount(int count);

    // Starts the player's turn
    virtual void startTurn();

    // Performs a gather action
    void gather();

    // Performs a tax action (virtual for Governor)
    virtual void tax();

    // Performs a bribe to get extra turn
    void bribe();

    // Performs arrest on a target player
    void arrest(Player& target);

    // Performs sanction on a target player
    void sanction(Player& target);

    // Performs coup on a target player
    void coup(Player& target);

    // Resolves a pending action
    void executePendingAction(const PendingAction &action);

    // Finds matching pending actions of other players
    static std::vector<std::pair<Player*, PendingAction*>> findPendingOfOthers(Game& game, Player* exclude, ActionType type, bool onlyBribe = false);

protected:
    // Validates it's the player's turn and they're not eliminated
    void checkTurn() const;

    // Advances to next player's turn or consumes extra turn
    void changeTurn();

    // Checks if it's currently a bonus turn
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
