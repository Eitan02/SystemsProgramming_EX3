// eitan.derdiger@gmail.com
#include "Player.hpp"
#include "../roles/Baron.hpp"
#include "../roles/Judge.hpp"
#include "../roles/Governor.hpp"
#include "../roles/General.hpp"
#include "../roles/Merchant.hpp"
#include "../exceptions/Exceptions.hpp"
#include <algorithm> // For std::min

using namespace coup;

Player::Player(Game& game, const std::string& name)
    : game_(game), name_(name), coins_(0),
      eliminated_(false), lastArrestTarget_(nullptr),
      sanctionCount_(0), arrestBlockedCount_(0), mustCoup_(false),
      extraTurns_(0), turnsSinceLastBribe_(0) {
    game_.addPlayer(this);
}

Player::Player(const Player& other)
    : game_(other.game_), name_(other.name_),
      coins_(other.coins_), eliminated_(other.eliminated_), lastArrestTarget_(other.lastArrestTarget_),
      sanctionCount_(other.sanctionCount_), arrestBlockedCount_(other.arrestBlockedCount_),
      mustCoup_(other.mustCoup_), extraTurns_(other.extraTurns_), turnsSinceLastBribe_(other.turnsSinceLastBribe_) {}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        // We cannot reassign game_ reference here, it's bound at construction.
        // For a game simulation, players are usually tied to one game instance.
        // If a Player is copied, it's typically within the context of the same game.
        // This is a common design decision for simplicity in game object models.
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

Player::~Player() {}

// getters / setters

std::string Player::getName() const { return name_; }

int Player::getCoins() const { return coins_; }
void Player::setCoins(int coins) { coins_ = coins; }

bool Player::isEliminated() const { return eliminated_; }
void Player::setEliminated(bool eliminated) { eliminated_ = eliminated; }

int Player::getArrestBlockedCount() const { return arrestBlockedCount_; }
void Player::setArrestBlockedCount(int count) { arrestBlockedCount_ = count; }



void Player::startTurn() {
    if (extraTurns_ > 0) {
        turnsSinceLastBribe_++;
    } else {
        turnsSinceLastBribe_ = 0;
    }

    if (sanctionCount_ > 0) {
        --sanctionCount_;
    }
    if (arrestBlockedCount_ > 0) {
        --arrestBlockedCount_;
    }
    

    for (const auto& action : pendingActions_) {
        if (!action.is_blocked) {
            executePendingAction(action);
        }
    }
    pendingActions_.clear();

    mustCoup_ = (coins_ >= 10);
}


void Player::checkTurn() const {
    if (game_.turn() != name_) {
        throw NotYourTurn("It's not " + name_ + "'s turn");
    }
    if (eliminated_) {
        throw IllegalAction(name_ + " is eliminated");
    }
}

void Player::changeTurn() {
    if (extraTurns_ > 0) {
        extraTurns_--;
        return;
    }
    game_.nextTurn();
}


bool Player::isBonusTurn() const {
    return extraTurns_ > 0 && turnsSinceLastBribe_ > 0;
}


void Player::gather() {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (sanctionCount_ > 0) {
        throw ActionBlocked(name_ + " is sanctioned and cannot gather");
    }

    if (isBonusTurn()) {
        pendingActions_.push_back(PendingAction{ ActionType::GATHER, this, nullptr, false, isBonusTurn() });
    } else {
        coins_ += 1;
    }

    changeTurn();
}


void Player::tax() {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (sanctionCount_ > 0) {
        throw ActionBlocked(name_ + " is sanctioned and cannot tax");
    }

    pendingActions_.push_back(PendingAction{ ActionType::TAX, this, nullptr, false, isBonusTurn() });
    changeTurn();
}


void Player::bribe() {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (coins_ < 4) {
        throw InsufficientCoins(name_ + " has insufficient coins for bribe");
    }
    coins_ -= 4;
    extraTurns_++;
    turnsSinceLastBribe_ = 0;
}


void Player::arrest(Player& target) {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }
    if (arrestBlockedCount_ > 0) {
        throw ActionBlocked(name_ + " is blocked from arrest");
    }
    if (target.isEliminated()) {
        throw TargetInvalid("Cannot arrest " + target.getName() + ": already eliminated");
    }
    if (&target == lastArrestTarget_) {
        throw IllegalAction("Cannot arrest the same target twice in a row");
    }
    if (target.getCoins() == 0) {
        throw InsufficientCoins("Cannot arrest " + target.getName() + ": has no coins");
    }

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


void Player::sanction(Player& target) {
    checkTurn();
    if (mustCoup_) {
        throw IllegalAction(name_ + " must perform coup with 10+ coins");
    }

    int cost = 3;
    if (dynamic_cast<Judge*>(&target)) {
        cost += 1;
    }

    if (coins_ < cost) {
        throw InsufficientCoins(name_ + " has insufficient coins for sanction (needs " + std::to_string(cost) + ")");
    }
    if (target.isEliminated()) {
        throw TargetInvalid("Cannot sanction " + target.getName() + ": already eliminated");
    }

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


void Player::coup(Player& target) {
    checkTurn();
    if (coins_ < 7) {
        throw InsufficientCoins(name_ + " has insufficient coins for coup");
    }
    if (target.isEliminated()) {
        throw TargetInvalid("Cannot coup " + target.getName() + ": already eliminated");
    }
    
    pendingActions_.push_back(PendingAction{ ActionType::COUP, this, &target, false, isBonusTurn() });
    coins_ -= 7;
    mustCoup_ = false;
    changeTurn();
}



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
            if (action.target && !action.target->isEliminated() && coins_ >= 7) {
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


std::vector<std::pair<Player*, PendingAction*>> Player::findPendingOfOthers(Game& game, Player* exclude, ActionType type, bool onlyBribe) {
    std::vector<std::pair<Player*, PendingAction*>> result;
    for (Player* p : game.players_) {
        if (!p || p == exclude || p->isEliminated()) {
            continue;
        }

        for (auto& action : p->pendingActions_) {
            if (action.type == type && (!onlyBribe || action.from_bribe)) {
                result.emplace_back(p, &action);
            }
        }
    }
    return result;
}

