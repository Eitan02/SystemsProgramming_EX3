// eitan.derdiger@gmail.com
#include "Exceptions.hpp"

NotYourTurn::NotYourTurn(const std::string& msg)
    : std::runtime_error(msg) {}

InsufficientCoins::InsufficientCoins(const std::string& msg)
    : std::runtime_error(msg) {}

IllegalAction::IllegalAction(const std::string& msg)
    : std::runtime_error(msg) {}

TargetInvalid::TargetInvalid(const std::string& msg)
    : std::runtime_error(msg) {}

ActionBlocked::ActionBlocked(const std::string& msg)
    : std::runtime_error(msg) {}