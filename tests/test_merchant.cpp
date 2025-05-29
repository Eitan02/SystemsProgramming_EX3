// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/Merchant.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

static void coins(Player& p, int c) { p.setCoins(c); }

static void forceTurn(Game& g, Player& p) {
    while (g.turn() != p.getName()) {
        g.nextTurn();
    }
}

TEST_SUITE("Merchant role – passive income and special arrest behaviour") {

// Validates Merchant gains 1 coin at start of turn if they have 3+
TEST_CASE("Merchant gains 1 coin automatically at start of turn")
{
    Game g;
    Merchant m(g, "Merc");
    Player   other(g, "O");
    g.addPlayer(&m);
    g.addPlayer(&other);

    coins(m, 3);
    forceTurn(g, m);
    m.startTurn();                         // passive +1
    CHECK(m.getCoins() == 4);

    m.gather();                            // +1 → 5
    forceTurn(g, other);
    other.gather();                        // advance turn
    forceTurn(g, m);
    m.startTurn();                         // passive +1 again
    CHECK(m.getCoins() == 6);
}

// Ensures no passive bonus if Merchant starts turn with <3 coins
TEST_CASE("Merchant with <3 coins gets no passive bonus")
{
    Game g;
    Merchant m(g, "M");
    Player   other(g, "O");
    g.addPlayer(&m);
    g.addPlayer(&other);

    coins(m, 2);
    forceTurn(g, m);
    m.startTurn();
    CHECK(m.getCoins() == 2);
}

// Confirms Merchant loses up to 2 coins to arrest but attacker gains nothing
TEST_CASE("Arrest subtracts up to 2 coins from Merchant and none credited to attacker")
{
    Game g;
    Player   thief(g, "Thief");
    Merchant m(g, "Merc");
    g.addPlayer(&thief);
    g.addPlayer(&m);

    coins(m, 3);
    forceTurn(g, thief);
    thief.arrest(m);                        // -2 to bank
    CHECK(m.getCoins() == 1);
    CHECK(thief.getCoins() == 0);

    forceTurn(g, m);
    m.startTurn();
    forceTurn(g, thief);
    thief.startTurn();       // rotate turns
    thief.arrest(m);                        // -1 to bank
    CHECK(m.getCoins() == 0);
    CHECK(thief.getCoins() == 0);
}

// Tests that passive bonus can trigger mustCoup when coins hit 10+
TEST_CASE("Merchant passive can trigger must-coup rule")
{
    Game g;
    Merchant m(g, "M");
    Player   target(g, "T");
    g.addPlayer(&m);
    g.addPlayer(&target);

    coins(m, 9);
    forceTurn(g, m);
    m.startTurn();                          // +1 → 10 → mustCoup_
    CHECK(m.getCoins() == 10);
    CHECK_THROWS_AS(m.gather(), IllegalAction);
    m.coup(target);                         // -7
    CHECK(m.getCoins() == 3);
    CHECK(target.isEliminated());
}

// Ensures Merchant cannot gather while sanctioned, but passive still applies
TEST_CASE("Merchant sanctioned cannot gather or tax despite passive bonus")
{
    Game g;
    Player   attacker(g, "Att");
    Merchant m(g, "M");
    g.addPlayer(&attacker);
    g.addPlayer(&m);

    coins(attacker, 3);
    forceTurn(g, attacker);
    attacker.sanction(m);                   // 2 turns blocked
    forceTurn(g, m);
    CHECK(g.turn() == "M");

    CHECK_THROWS_AS(m.gather(), ActionBlocked); // still blocked
    m.startTurn();                               // 1 turn left

    coins(m, 3);
    m.startTurn();                               // sanction ends, passive triggers
    CHECK(m.getCoins() == 4);

    m.gather();                                  // now allowed
    CHECK(m.getCoins() == 5);
}

} // TEST_SUITE
