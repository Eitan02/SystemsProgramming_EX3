// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/General.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

TEST_SUITE("General role – preventCoup and arrest refund") {

// Tests that preventCoup succeeds, blocks the coup and charges 5 coins
TEST_CASE("General blocks a pending coup with 5-coin payment")
{
    Game g;
    Player  attacker(g, "Att");
    General defender(g, "Gen");
    Player  target(g, "Tgt");

    attacker.setCoins(7);
    attacker.coup(target);                       // pending coup, turn → Gen
    defender.setCoins(5);

    defender.preventCoup(target);                // pays 5, blocks coup
    CHECK(defender.getCoins() == 0);
    CHECK(g.turn() == "Tgt");

    target.gather();                             // trivial move
    CHECK(g.turn() == "Att");
    attacker.startTurn();                        // should do nothing

    CHECK(target.isEliminated() == false);       // coup neutralised
    CHECK(attacker.getCoins() == 0);             // no refund to attacker
}

// Verifies that preventCoup fails if General has less than 5 coins
TEST_CASE("preventCoup with <5 coins throws InsufficientCoins")
{
    Game g;
    Player  attacker(g, "Att");
    General gen(g, "Gen");
    Player  victim(g, "Vic");

    attacker.setCoins(7);
    attacker.coup(victim);                       // pending
    gen.setCoins(4);

    CHECK_THROWS_AS(gen.preventCoup(victim), InsufficientCoins);
    CHECK(g.turn() == "Gen");                    // turn unchanged
}

// Checks that preventCoup fails if there's no pending coup at all
TEST_CASE("preventCoup when no pending coup throws IllegalAction")
{
    Game g;
    General gen(g, "Gen");
    Player  p(g, "P");

    gen.setCoins(5);
    CHECK_THROWS_AS(gen.preventCoup(p), IllegalAction);
}

// Checks that preventCoup fails if the target is already eliminated
TEST_CASE("preventCoup on eliminated target throws TargetInvalid")
{
    Game g;
    Player  attacker(g, "Att");
    General gen(g, "Gen");
    Player  dead(g, "Dead");

    dead.setEliminated(true);
    gen.setCoins(5);
    CHECK_THROWS_AS(gen.preventCoup(dead), TargetInvalid);
}

// Tests General's passive: refund when arrested
TEST_CASE("When General is arrested, stolen coin is refunded to General")
{
    Game g;
    Player  thief(g, "Thief");
    General gen(g, "Gen");

    gen.setCoins(2);
    thief.arrest(gen);                           // steals 1, refunded
    CHECK(gen.getCoins() == 2);
    CHECK(thief.getCoins() == 0);
}

// Verifies that a blocked coup cannot be blocked again
TEST_CASE("preventCoup on an already-blocked coup throws")
{
    Game g;
    Player  att(g,"Att");
    General gen1(g,"G1");
    General gen2(g,"G2");
    Player tgt(g,"T");

    att.setCoins(7);
    att.coup(tgt);              // coup pending
    gen1.setCoins(5);
    gen1.preventCoup(tgt);      // blocks the coup

    gen2.setCoins(5);
    CHECK_THROWS_AS(gen2.preventCoup(tgt), IllegalAction);
}

} // TEST_SUITE
