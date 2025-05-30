// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/General.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

static void forceTurn(Game& g, Player& p) {
    while (g.turn() != p.getName()) {
        g.nextTurn();
    }
}

TEST_SUITE("General role – preventCoup and arrest refund") {

// Tests that preventCoup succeeds, blocks the coup and charges 5 coins
TEST_CASE("General blocks a pending coup with 5-coin payment")
{
    Game g;
    Player  attacker(g, "Att");
    General defender(g, "Gen");
    Player  other(g, "Other");
    g.addPlayer(&attacker);
    g.addPlayer(&defender);
    g.addPlayer(&other);

    attacker.setCoins(7);
    forceTurn(g, attacker);
    attacker.coup(defender);                   // coup → on defender (the General)
    CHECK(g.turn() == "Gen");

    defender.setCoins(5);
    forceTurn(g, defender);
    defender.preventCoup(attacker);           // cancels pending coup from attacker → defender
    CHECK(defender.getCoins() == 0);
    CHECK(g.turn() == "Other");

    forceTurn(g, other);
    other.gather();                            // move on

    forceTurn(g, attacker);
    attacker.startTurn();                      // should do nothing

    CHECK(defender.isEliminated() == false);   // General survived
    CHECK(attacker.getCoins() == 0);           // no refund to attacker

}

// Verifies that preventCoup fails if General has less than 5 coins
TEST_CASE("preventCoup with <5 coins throws InsufficientCoins")
{
    Game g;
    Player  attacker(g, "Att");
    General gen(g, "Gen");
    Player  victim(g, "Vic");
    g.addPlayer(&attacker);
    g.addPlayer(&gen);
    g.addPlayer(&victim);

    attacker.setCoins(7);
    forceTurn(g, attacker);
    attacker.coup(victim);                       // pending
    gen.setCoins(4);

    forceTurn(g, gen);
    CHECK_THROWS_AS(gen.preventCoup(victim), InsufficientCoins);
    CHECK(g.turn() == "Gen");                    // turn unchanged
}

// Checks that preventCoup fails if there's no pending coup at all
TEST_CASE("preventCoup when no pending coup throws IllegalAction")
{
    Game g;
    General gen(g, "Gen");
    Player  p(g, "P");
    g.addPlayer(&gen);
    g.addPlayer(&p);

    gen.setCoins(5);
    forceTurn(g, gen);
    CHECK_THROWS_AS(gen.preventCoup(p), IllegalAction);
}

// Checks that preventCoup fails if the target is already eliminated
TEST_CASE("preventCoup on eliminated target throws TargetInvalid")
{
    Game g;
    Player  attacker(g, "Att");
    General gen(g, "Gen");
    Player  dead(g, "Dead");
    g.addPlayer(&attacker);
    g.addPlayer(&gen);
    g.addPlayer(&dead);

    dead.setEliminated(true);
    gen.setCoins(5);
    forceTurn(g, gen);
    CHECK_THROWS_AS(gen.preventCoup(dead), TargetInvalid);
}

// Tests General's passive: refund when arrested
TEST_CASE("When General is arrested, stolen coin is refunded to General")
{
    Game g;
    Player  thief(g, "Thief");
    General gen(g, "Gen");
    g.addPlayer(&thief);
    g.addPlayer(&gen);

    gen.setCoins(2);
    forceTurn(g, thief);
    thief.arrest(gen);                           // steals 1, refunded
    CHECK(gen.getCoins() == 2);
    CHECK(thief.getCoins() == 0);
}

} // TEST_SUITE
