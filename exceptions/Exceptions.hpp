// eitan.derdiger@gmail.com
#ifndef COUP_EXCEPTIONS_HPP
#define COUP_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

class NotYourTurn : public std::runtime_error {
public:
    explicit NotYourTurn(const std::string& msg);
};

class InsufficientCoins : public std::runtime_error {
public:
    explicit InsufficientCoins(const std::string& msg);
};

class IllegalAction : public std::runtime_error {
public:
    explicit IllegalAction(const std::string& msg);
};

class TargetInvalid : public std::runtime_error {
public:
    explicit TargetInvalid(const std::string& msg);
};

class ActionBlocked : public std::runtime_error {
public:
    explicit ActionBlocked(const std::string& msg);
};

#endif // COUP_EXCEPTIONS_HPP