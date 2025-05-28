// eitan.derdiger@gmail.com
#ifndef COUP_EXCEPTIONS_HPP
#define COUP_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

// Raised when a player tries to act outside their turn
class NotYourTurn : public std::runtime_error {
public:
    explicit NotYourTurn(const std::string& msg);
};

// Raised when a player doesn't have enough coins for an action
class InsufficientCoins : public std::runtime_error {
public:
    explicit InsufficientCoins(const std::string& msg);
};

// Raised when a move violates game rules
class IllegalAction : public std::runtime_error {
public:
    explicit IllegalAction(const std::string& msg);
};

// Raised when the chosen target is no longer valid
class TargetInvalid : public std::runtime_error {
public:
    explicit TargetInvalid(const std::string& msg);
};

// Raised when an action is actively blocked by another player
class ActionBlocked : public std::runtime_error {
public:
    explicit ActionBlocked(const std::string& msg);
};

#endif // COUP_EXCEPTIONS_HPP
