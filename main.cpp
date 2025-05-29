// eitan.derdiger@gmail.com
#include <iostream>
#include <iomanip>
#include <vector>
#include "game/Game.hpp"
#include "roles/Governor.hpp"
#include "roles/Spy.hpp"
#include "roles/Baron.hpp"
#include "roles/General.hpp"
#include "roles/Judge.hpp"
#include "roles/Merchant.hpp"
#include "exceptions/Exceptions.hpp"

using namespace coup;

// Prints details of a single player
void printPlayer(const Player* p) {
    std::cout << std::setw(8) << p->getName()
              << " | Coins: " << p->getCoins()
              << (p->isEliminated() ? " | ☠ Eliminated" : "") << "\n";
}

// Displays full game state
void printGameState(Game& game) {
    std::cout << "\n🎲 Game State:\n";
    for (Player* p : game.allPlayers()) {
        printPlayer(p);
    }
    std::cout << "-----------------------\n";
}

// Checks if player is alive and valid
bool isAlive(Player* p) {
    return p && !p->isEliminated();
}

// Picks a living target different from given player
Player* findTarget(Game& game, Player* exclude) {
    for (Player* p : game.allPlayers()) {
        if (isAlive(p) && p != exclude) return p;
    }
    return nullptr;
}

int main() {
    Game game;

    Governor  gvr(game, "Alice");
    Spy       spy(game, "Bob");
    Baron     baron(game, "Charlie");
    General   gen(game, "Diana");
    Judge     judge(game, "Eli");
    Merchant  merch(game, "Frank");

    game.addPlayer(&gvr);
    game.addPlayer(&spy);
    game.addPlayer(&baron);
    game.addPlayer(&gen);
    game.addPlayer(&judge);
    game.addPlayer(&merch);

    std::vector<Player*> order = { &gvr, &spy, &baron, &gen, &judge, &merch };
    std::cout << "=== COUP FULL SIMULATION ===\n";

    int round = 1;
    while (true) {
        std::cout << "\n🔁 Round " << round++ << "\n";

        for (Player* player : order) {
            if (!isAlive(player)) continue;

            try {
                player->startTurn();

                if (player->getCoins() >= 10) {
                    Player* target = findTarget(game, player);
                    if (target) {
                        std::cout << player->getName() << " performs COUP on " << target->getName() << "\n";
                        player->coup(*target);
                    }
                } else if (dynamic_cast<Governor*>(player)) {
                    std::cout << player->getName() << " performs TAX (Governor)\n";
                    player->tax();
                } else if (dynamic_cast<Baron*>(player)) {
                    if (player->getCoins() >= 3) {
                        std::cout << player->getName() << " performs INVEST\n";
                        static_cast<Baron*>(player)->invest();
                    } else {
                        std::cout << player->getName() << " performs GATHER\n";
                        player->gather();
                    }
                } else if (dynamic_cast<Merchant*>(player)) {
                    std::cout << player->getName() << " performs GATHER\n";
                    player->gather();
                } else if (dynamic_cast<General*>(player)) {
                    Player* target = findTarget(game, player);
                    if (player->getCoins() >= 7 && target) {
                        std::cout << player->getName() << " performs COUP on " << target->getName() << "\n";
                        player->coup(*target);
                    } else {
                        std::cout << player->getName() << " performs GATHER\n";
                        player->gather();
                    }
                } else {
                    std::cout << player->getName() << " performs GATHER\n";
                    player->gather();
                }
            } catch (const std::exception& e) {
                std::cout << "⚠ Exception: " << e.what() << "\n";
            }
        }

        printGameState(game);

        try {
            std::string winner = game.winner();
            std::cout << "\n🏆 Winner is: " << winner << "\n";
            break;
        } catch (...) {
            continue;
        }
    }

    return 0;
}
