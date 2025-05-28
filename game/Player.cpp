// eitan.derdiger@gmail.com
#include "Player.hpp"
#include "../roles/Baron.hpp"
#include "../roles/Judge.hpp"
#include "../roles/Governor.hpp"
#include "../roles/General.hpp"
#include "../roles/Merchant.hpp"
#include "../exceptions/Exceptions.hpp"
#include <algorithm>

using namespace coup;

// Creates and registers a new player to the game
Player::Player(Game& game, const std::string& name)
    : game_(game), name_(name), coins_(0),
      eliminated_(false), lastArrestTarget_(nullptr),
      sanctionCount_(0), arrestBlockedCount_(0), mustCoup_(false),
      extraTurns_(0), turnsSinceLastBribe_(0) {
    game_.addPlayer(this);
}

// Copies player state except game reference
Player::Player(const Player& other)
    : game_(other.game_), name_(other.name_),
      coins_(other.coins_), eliminated_(other.eliminated_), lastArrestTarget_(other.lastArrestTarget_),
      sanctionCount_(other.sanctionCount_), arrestBlockedCount_(other.arrestBlockedCount_),
      mustCoup_(other.mustCoup_), extraTurns_(other.extraTurns_), turnsSinceLastBribe_(other.turnsSinceLastBribe_) {}

// Assignment operator, excluding game reference
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        name_ = other.name_;
        coins_ = other.coins_;
        eliminated_ = other.eliminated_;
        lastArrestTarget_ = other.lastArrestTarget_;
        sanctionCount_ = other.sanctionCount_;
        arrestBlockedCount_ = other.arrestBlockedCount_;
        mustCoup_ = other.mustCoup_;
        turnsSinceLastBribe_ = other.turnsSinceLastBribe_;
    }
    return *this;
}

// Destroys a player
Player::~Player() {}

// Returns the player's name
std::string Player::getName() const { return name_; }

// Returns the player's coin count
int Player::getCoins() const { return coins_; }

// Sets the player's coin count
void Player::setCoins(int coins) { coins_ = coins; }

// Returns if the player is eliminated
bool Player::isEliminated() const { return eliminated_; }

// Sets the elimination state
void Player::setEliminated(bool eliminated) { eliminated_ = eliminated; }

// Gets how many turns arrest is blocked
int Player::getArrestBlockedCount() const { return arrestBlockedCount_; }

// Sets how many turns arrest is blocked
void Player::setArrestBlockedCount(int count) { arrestBlockedCount_ = count; }

// Called at the start of player's turn
void Player::startTurn() {
    if (extraTurns_ > 0) {
        turnsSinceLastBribe_++;
    } else {
        turnsSinceLastBribe_ = 0;
    }

    if (sanctionCount_ > 0) --sanctionCount_;
    if (arrestBlockedCount_ > 0) --arrestBlockedCount_;

    for (const auto& action : pendingActions_) {
        if (!action.is_blocked) {
            executePendingAction(action);
        }
    }
    pendingActions_.clear();
    mustCoup_ = (coins_ >= 10);
}

// Validates if it's the player's legal turn
void Player::checkTurn() const {
    if (game_.turn() != name_) {
        throw NotYourTurn("It's not " + name_ + "'s turn");
    }
    if (eliminated_) {
        throw IllegalAction(name_ + " is eliminated");
    }
}

// Handles turn rotation logic
void Player::changeTurn() {
    if (extraTurns_ > 0) {
        extraTurns_--;
        return;
    }
    game_.nextTurn();
}

// Checks if current turn is a bonus turn
bool Player::isBonusTurn() const {
    return extraTurns_ > 0 && turnsSinceLastBribe_ > 0;
}

// Performs the gather action
void Player::gather() {
    checkTurn();
    if (mustCoup_) throw IllegalAction(name_ + " must perform coup with 10+ coins");
    if (sanctionCount_ > 0) throw ActionBlocked(name_ + " is sanctioned and cannot gather");

    if (isBonusTurn()) {
        pendingActions_.push_back(PendingAction{ ActionType::GATHER, this, nullptr, false, isBonusTurn() });
    } else {
        coins_ += 1;
    }
    changeTurn();
}

// Performs the tax action
void Player::tax() {
    checkTurn();
    if (mustCoup_) throw IllegalAction(name_ + " must perform coup with 10+ coins");
    if (sanctionCount_ > 0) throw ActionBlocked(name_ + " is sanctioned and cannot tax");

    pendingActions_.push_back(PendingAction{ ActionType::TAX, this, nullptr, false, isBonusTurn() });
    changeTurn();
}

// Performs the bribe action
void Player::bribe() {
    checkTurn();
    if (mustCoup_) throw IllegalAction(name_ + " must perform coup with 10+ coins");
    if (coins_ < 4) throw InsufficientCoins(name_ + " has insufficient coins for bribe");

    coins_ -= 4;
    extraTurns_++;
    turnsSinceLastBribe_ = 0;
}

// Performs the arrest action on a target
void Player::arrest(Player& target) {
    checkTurn();
    if (mustCoup_) throw IllegalAction(name_ + " must perform coup with 10+ coins");
    if (arrestBlockedCount_ > 0) throw ActionBlocked(name_ + " is blocked from arrest");
    if (target.isEliminated()) throw TargetInvalid("Cannot arrest " + target.getName() + ": already eliminated");
    if (&target == lastArrestTarget_) throw IllegalAction("Cannot arrest the same target twice in a row");
    if (target.getCoins() == 0) throw InsufficientCoins("Cannot arrest " + target.getName() + ": has no coins");

    if (isBonusTurn()) {
        pendingActions_.push_back(PendingAction{ ActionType::ARREST, this, &target, false, isBonusTurn() });
    } else {
        if (dynamic_cast<Merchant*>(&target)) {
            target.coins_ -= std::min(target.coins_, 2);
        } else {
            target.coins_ -= 1;
            coins_ += 1;
            if (dynamic_cast<General*>(&target)) {
                target.coins_ += 1;
                coins_ -= 1;
            }
        }
        lastArrestTarget_ = &target;
    }
    changeTurn();
}

// Performs the sanction action on a target
void Player::sanction(Player& target) {
    checkTurn();
    if (mustCoup_) throw IllegalAction(name_ + " must perform coup with 10+ coins");

    int cost = 3;
    if (dynamic_cast<Judge*>(&target)) cost += 1;
    if (coins_ < cost) throw InsufficientCoins(name_ + " has insufficient coins for sanction (needs " + std::to_string(cost) + ")");
    if (target.isEliminated()) throw TargetInvalid("Cannot sanction " + target.getName() + ": already eliminated");

    if (isBonusTurn()) {
        pendingActions_.push_back(PendingAction{ ActionType::SANCTION, this, &target, false, isBonusTurn() });
    } else {
        coins_ -= cost;
        if (dynamic_cast<Baron*>(&target)) {
            target.coins_ += 1;
        }
        target.sanctionCount_ = 2;
    }
    changeTurn();
}

// Performs the coup action on a target
void Player::coup(Player& target) {
    checkTurn();
    if (coins_ < 7) throw InsufficientCoins(name_ + " has insufficient coins for coup");
    if (target.isEliminated()) throw TargetInvalid("Cannot coup " + target.getName() + ": already eliminated");

    pendingActions_.push_back(PendingAction{ ActionType::COUP, this, &target, false, isBonusTurn() });
    coins_ -= 7;
    mustCoup_ = false;
    changeTurn();
}

// Executes a stored pending action
void Player::executePendingAction(const PendingAction& action) {
    switch (action.type) {
        case ActionType::TAX:
            coins_ += 2;
            break;

        case ActionType::TAX_Governor:
            coins_ += 3;
            break;

        case ActionType::INVEST:
            if (coins_ >= 3) {
                coins_ -= 3;
                coins_ += 6;
            }
            break;

        case ActionType::COUP:
            if (action.target && !action.target->isEliminated()) {
                action.target->setEliminated(true);
            }
            break;

        case ActionType::GATHER:
            if (sanctionCount_ == 0) {
                coins_ += 1;
            }
            break;

        case ActionType::ARREST:
            if (action.target && !action.target->isEliminated() && action.target != lastArrestTarget_) {
                if (action.target->getCoins() == 0) break;

                if (dynamic_cast<Merchant*>(action.target)) {
                    action.target->coins_ -= std::min(action.target->coins_, 2);
                } else {
                    action.target->coins_ -= 1;
                    coins_ += 1;
                    if (dynamic_cast<General*>(action.target)) {
                        action.target->coins_ += 1;
                        coins_ -= 1;
                    }
                }
                lastArrestTarget_ = action.target;
            }
            break;

        case ActionType::SANCTION:
            if (action.target && !action.target->isEliminated()) {
                int cost = 3;
                if (dynamic_cast<Judge*>(action.target)) {
                    cost += 1;
                }
                if (coins_ >= cost) {
                    coins_ -= cost;
                    if (dynamic_cast<Baron*>(action.target)) {
                        action.target->coins_ += 1;
                    }
                    action.target->sanctionCount_ = 2;
                }
            }
            break;

        default:
            break;
    }
}

// Finds pending actions of other players by type
std::vector<std::pair<Player*, PendingAction*>> Player::findPendingOfOthers(Game& game, Player* exclude, ActionType type, bool onlyBribe) {
    std::vector<std::pair<Player*, PendingAction*>> result;
    for (Player* p : game.players_) {
        if (!p || p == exclude || p->isEliminated()) continue;

        for (auto& action : p->pendingActions_) {
            if (action.type == type && (!onlyBribe || action.from_bribe)) {
                result.emplace_back(p, &action);
            }
        }
    }
    return result;
}
