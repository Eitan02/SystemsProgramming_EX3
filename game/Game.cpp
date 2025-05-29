// eitan.derdiger@gmail.com
#include "Game.hpp"
#include "Player.hpp"
#include "../exceptions/Exceptions.hpp"
#include <iostream>

using namespace coup;

// Represents the game state and handles player turns
Game::Game()
    : turnIdx_(0) {}

Game::~Game() {}

// Adds a new player to the game if there's space
void Game::addPlayer(Player* player) {
    if (players_.size() >= 6) {
        throw IllegalAction("Cannot add more than 6 players");
    }
    players_.push_back(player);
}

// Returns the name of the player whose turn it is
std::string Game::turn() const {
    if (players_.empty()) {
        throw IllegalAction("No players in game");
    }
    return players_[turnIdx_]->getName();
}


// Advances the turn to the next non-eliminated player
void Game::nextTurn() {
    if (players_.empty()) {
        return;
    }

    do {
        turnIdx_ = (turnIdx_ + 1) % players_.size();
    } while (players_[turnIdx_]->isEliminated());

    players_[turnIdx_]->startTurn();
}

// Returns a list of all active player names
std::vector<std::string> Game::players() const {
    std::vector<std::string> names;
    for (auto p : players_) {
        if (!p->isEliminated()) {
            names.push_back(p->getName());
        }
    }
    return names;
}

const std::vector<Player*>& Game::allPlayers() const {
    return players_;
}

// Returns the winner's name if there's only one player left
std::string Game::winner() const {
    auto active = players();

    if (active.size() != 1) {
        throw IllegalAction("Game is still ongoing");
    }
    return active.front();
}
