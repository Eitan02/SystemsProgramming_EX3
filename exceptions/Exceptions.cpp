// eitan.derdiger@gmail.com
#include "Exceptions.hpp"

// Thrown when a player attempts to act out of turn
NotYourTurn::NotYourTurn(const std::string& msg)
    : std::runtime_error(msg) {}

// Thrown when a player tries an action without enough coins
InsufficientCoins::InsufficientCoins(const std::string& msg)
    : std::runtime_error(msg) {}

// Thrown when an attempted action is against game rules
IllegalAction::IllegalAction(const std::string& msg)
    : std::runtime_error(msg) {}

// Thrown when a targeted player is not valid (e.g., already out)
TargetInvalid::TargetInvalid(const std::string& msg)
    : std::runtime_error(msg) {}

// Thrown when an action is blocked by another player's ability
ActionBlocked::ActionBlocked(const std::string& msg)
    : std::runtime_error(msg) {}
