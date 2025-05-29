// eitan.derdiger@gmail.com
#include <iostream>
#include "game/Game.hpp"
#include "roles/Governor.hpp"
#include "roles/Spy.hpp"
#include "roles/Baron.hpp"
#include "roles/General.hpp"
#include "roles/Judge.hpp"
#include "roles/Merchant.hpp"
#include "exceptions/Exceptions.hpp"

using namespace coup;

int main() {
    Game game;

    Governor gvr(game, "Alice");
    Spy spy(game, "Bob");
    Baron baron(game, "Charlie");
    General gen(game, "Diana");
    Judge judge(game, "Eli");
    Merchant merch(game, "Frank");

    game.addPlayer(&gvr);
    game.addPlayer(&spy);
    game.addPlayer(&baron);
    game.addPlayer(&gen);
    game.addPlayer(&judge);
    game.addPlayer(&merch);

    std::cout << "=== Starting Full Demo ===\n";

    try {
        gvr.startTurn();
        gvr.tax();            // Alice (Governor) gets 3 coins

        spy.gather();         // Bob gets 1 coin
        baron.bribe();        // +1 extra turn
        baron.invest();       // from bonus turn → queued
        gen.gather();
        judge.gather();
        merch.gather();       // +1 coin

        baron.startTurn();    // Bonus turn: executes invest → +3 net
        spy.blockArrest(baron); // Spy blocks arrest on Baron for 2 turns
        gvr.tax();            // Alice: +3 → now at 6 coins

        gen.coup(judge);      // Diana eliminates Eli (Judge)
        gvr.tax();            // Alice hits 9
        gvr.gather();         // Alice hits 10 — should now be forced to coup
        std::cout << "Trying invalid gather at 10+ coins...\n";
        gvr.gather();         // Should throw

    } catch (const std::exception& e) {
        std::cerr << "❗ Exception caught: " << e.what() << "\n";
    }

    std::cout << "\n=== Remaining players ===\n";
    for (const auto& name : game.players()) {
        std::cout << "- " << name << "\n";
    }

    return 0;
}
