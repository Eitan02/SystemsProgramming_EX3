//eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/Baron.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

static void forceTurn(Game& g, Player& p) {
    while (g.turn() != p.getName()) {
        g.nextTurn();
    }
}

TEST_SUITE("Baron role – invest ability and sanction interaction") {

// Checks that a successful invest action updates coins and changes turn
TEST_CASE("successful invest updates coins correctly and advances turn")
{
    Game g;
    Baron baron(g, "Baron");
    Player other(g, "Other");
    g.addPlayer(&baron);
    g.addPlayer(&other);

    forceTurn(g, baron);
    baron.setCoins(4);
    baron.invest();                          // pay 3, gain 6
    CHECK(baron.getCoins() == 7);
    CHECK(g.turn() == "Other");
}

// Ensures exception is thrown if Baron tries to invest with less than 3 coins
TEST_CASE("invest with insufficient coins throws InsufficientCoins")
{
    Game g;
    Baron baron(g, "Baron");
    Player dummy(g, "D");
    g.addPlayer(&baron);
    g.addPlayer(&dummy);

    forceTurn(g, baron);
    baron.setCoins(2);
    CHECK_THROWS_AS(baron.invest(), InsufficientCoins);
    CHECK(g.turn() == "Baron");              // turn unchanged after exception
    CHECK(baron.getCoins() == 2);
}

// Validates that Baron cannot invest when holding 10+ coins (must coup)
TEST_CASE("Baron holding ≥10 coins must coup, cannot invest")
{
    Game g;
    Baron baron(g, "Baron");
    Player target(g, "Target");
    g.addPlayer(&baron);
    g.addPlayer(&target);

    forceTurn(g, baron);
    baron.setCoins(10);
    baron.startTurn();                       // sets mustCoup_
    CHECK_THROWS_AS(baron.invest(), IllegalAction);
    CHECK(g.turn() == "Baron");
    baron.coup(target);                      // spend 7
    CHECK(baron.getCoins() == 3);
}

// Tests that invest from a bribe bonus turn is deferred then executed
TEST_CASE("invest executed at start of bonus extra turn")
{
    Game g;
    Baron baron(g, "Baron");
    Player donor(g, "Donor");
    g.addPlayer(&baron);
    g.addPlayer(&donor);

    forceTurn(g, baron);
    baron.setCoins(8);
    baron.bribe();                           // pay 4, extra turn
    CHECK(baron.getCoins() == 4);
    CHECK(g.turn() == "Baron");              // still his turn after bribe

    baron.invest();                          
    CHECK(g.turn() == "Baron");              // still extra turn consumed
    baron.invest();                          // queued (from bribe)
    CHECK(baron.getCoins() == 7);            // not applied yet

    forceTurn(g, donor);
    donor.gather();

    forceTurn(g, baron);
    baron.startTurn();                       // pending executes now (+3 net)
    CHECK(baron.getCoins() == 10);
}

// Validates Baron's passive: gets 1 coin if sanctioned
TEST_CASE("sanction gives Baron 1 coin compensation")
{
    Game g;
    Player attacker(g, "Att");
    Baron  baron(g, "Baron");
    g.addPlayer(&attacker);
    g.addPlayer(&baron);

    forceTurn(g, attacker);
    attacker.setCoins(3);
    attacker.sanction(baron);                // cost 3
    CHECK(attacker.getCoins() == 0);
    CHECK(baron.getCoins() == 1);            // compensation
    CHECK(baron.isEliminated() == false);
    CHECK(baron.getArrestBlockedCount() == 0);
    CHECK(g.turn() == "Baron");
}

} // TEST_SUITE
