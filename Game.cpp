// eitan.derdiger@gmail.com
#include "Game.hpp"
#include "Player.hpp"
#include "Exceptions.hpp"

using namespace coup;

Game::Game()
    : turnIdx_(0) {}

Game::~Game() {}

void Game::addPlayer(Player* player) {
    if (players_.size() >= 6) {
        throw IllegalAction("Cannot add more than 6 players");
    }
    players_.push_back(player);
    if (players_.size() == 1) {
        players_[0]->startTurn();
    }
}

const std::string& Game::turn() const {
    if (players_.empty()) {
        throw IllegalAction("No players in game");
    }
    return players_[turnIdx_]->getName();
}

void Game::nextTurn() {
    if (players_.empty()) {
        return;
    }
    do {
        turnIdx_ = (turnIdx_ + 1) % players_.size();
    } while (players_[turnIdx_]->isEliminated());
    players_[turnIdx_]->startTurn();
}

std::vector<std::string> Game::players() const {
    std::vector<std::string> names;
    for (auto p : players_) {
        if (!p->isEliminated()) {
            names.push_back(p->getName());
        }
    }
    return names;
}

std::string Game::winner() const {
    auto active = players();
    if (active.size() == 1) {
        return active.front();
    }
    throw IllegalAction("Game is still ongoing");
}