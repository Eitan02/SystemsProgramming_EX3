// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/Judge.hpp"
#include "../roles/Baron.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

static void giveCoins(Player& p, int c) { p.setCoins(c); }

TEST_SUITE("Judge role – undoBribe and sanction surcharge") {

// Checks that undoBribe cancels a bonus gather gained from bribe
TEST_CASE("Judge cancels bonus action produced by bribe")
{
    Game g;
    Player briber(g, "Briber");
    Judge  judge(g, "Judge");
    Player third(g, "T");

    giveCoins(briber, 4);
    briber.bribe();                          // extra turn
    briber.gather();                         // bonus action → queued
    CHECK(g.turn() == "Briber");
    briber.gather();                         // finish bonus

    CHECK(g.turn() == "Judge");
    judge.undoBribe(briber);                // cancel bonus action
    CHECK(g.turn() == "T");

    third.gather();
    CHECK(g.turn() == "Briber");
    briber.startTurn();                     // blocked gather does not apply
    CHECK(briber.getCoins() == 0);
}

// Ensures undoBribe throws if there's no bonus action to cancel
TEST_CASE("undoBribe without any pending from-bribe action throws IllegalAction")
{
    Game g;
    Player p(g, "P");
    Judge  judge(g, "Judge");

    giveCoins(p, 4);
    p.bribe();                               // no bonus yet

    CHECK_THROWS_AS(judge.undoBribe(p), IllegalAction);
    CHECK(g.turn() == "Judge");
}

// Checks that sanctioning a Judge costs 4 coins instead of 3
TEST_CASE("Sanctioning Judge costs 4 coins instead of 3")
{
    Game g;
    Player attacker(g, "Att");
    Judge  judge(g, "Judge");

    giveCoins(attacker, 4);
    attacker.sanction(judge);                // should deduct 4
    CHECK(attacker.getCoins() == 0);
    CHECK(judge.isEliminated() == false);
    CHECK(judge.getArrestBlockedCount() == 0);
    CHECK(g.turn() == "Judge");
}

// Ensures undoBribe fails if the target is already eliminated
TEST_CASE("undoBribe on eliminated target throws TargetInvalid")
{
    Game g;
    Player  dead(g, "Dead");
    Judge   judge(g, "Judge");
    dead.setEliminated(true);

    CHECK_THROWS_AS(judge.undoBribe(dead), TargetInvalid);
}

// Verifies that sanction from bonus turn costs 4 when executed on Judge
TEST_CASE("sanction on Judge from bonus turn charges 4 coins upon execution")
{
    Game g;
    Player attacker(g,"Att");
    Judge judge(g,"Judge");
    Player dummy(g,"D");

    attacker.setCoins(8);
    attacker.bribe();                          // now 4 coins
    attacker.sanction(judge);                  // queued
    attacker.gather();                         // end bonus
    dummy.gather();

    attacker.startTurn();                      // sanction executes now
    CHECK(attacker.getCoins() == 0);
    CHECK_THROWS_AS(judge.gather(), ActionBlocked);
}

// Ensures deferred sanction still respects Judge’s surcharge
TEST_CASE("deferred sanction on Judge charges 4 coins and blocks")
{
    Game g;
    Player attacker(g,"A");
    Judge judge(g,"J");
    Player x(g,"X");

    attacker.setCoins(7);
    attacker.bribe();               // → 3 left
    attacker.sanction(judge);       // queued
    attacker.gather();              // finish bonus
    x.gather();

    attacker.startTurn();           // sanction applies
    CHECK(attacker.getCoins() == 0);
    CHECK_THROWS_AS(judge.gather(), ActionBlocked);
}

} // TEST_SUITE
