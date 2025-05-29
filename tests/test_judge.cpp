// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/Judge.hpp"
#include "../roles/Baron.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

static void giveCoins(Player& p, int c) { p.setCoins(c); }

static void forceTurn(Game& g, Player& p) {
    while (g.turn() != p.getName()) {
        g.nextTurn();
    }
}

TEST_SUITE("Judge role – undoBribe and sanction surcharge") {

// Checks that undoBribe cancels a bonus gather gained from bribe
TEST_CASE("Judge cancels bonus action produced by bribe")
{
    Game g;
    Player briber(g, "Briber");
    Judge  judge(g, "Judge");
    Player third(g, "T");
    g.addPlayer(&briber);
    g.addPlayer(&judge);
    g.addPlayer(&third);

    giveCoins(briber, 4);
    forceTurn(g, briber);
    briber.bribe();                          // extra turn
    briber.gather();                         // bonus action → queued
    CHECK(g.turn() == "Briber");
    briber.gather();                         // finish bonus

    forceTurn(g, judge);
    CHECK(g.turn() == "Judge");
    judge.undoBribe(briber);                // cancel bonus action
    CHECK(g.turn() == "T");

    forceTurn(g, third);
    third.gather();
    CHECK(g.turn() == "Briber");

    forceTurn(g, briber);
    briber.startTurn();                     // blocked gather does not apply
    CHECK(briber.getCoins() == 1);
}

// Ensures undoBribe throws if there's no bonus action to cancel
TEST_CASE("undoBribe without any pending from-bribe action throws IllegalAction")
{
    Game g;
    Player p(g, "P");
    Judge  judge(g, "Judge");
    g.addPlayer(&p);
    g.addPlayer(&judge);

    giveCoins(p, 4);
    forceTurn(g, p);
    p.bribe();                               // no bonus yet

    forceTurn(g, judge);
    CHECK_THROWS_AS(judge.undoBribe(p), IllegalAction);
    CHECK(g.turn() == "Judge");
}

// Checks that sanctioning a Judge costs 4 coins instead of 3
TEST_CASE("Sanctioning Judge costs 4 coins instead of 3")
{
    Game g;
    Player attacker(g, "Att");
    Judge  judge(g, "Judge");
    g.addPlayer(&attacker);
    g.addPlayer(&judge);

    giveCoins(attacker, 4);
    forceTurn(g, attacker);
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
    g.addPlayer(&dead);
    g.addPlayer(&judge);
    dead.setEliminated(true);

    forceTurn(g, judge);
    CHECK_THROWS_AS(judge.undoBribe(dead), TargetInvalid);
}

// Verifies that sanction from bonus turn costs 4 when executed on Judge
TEST_CASE("sanction on Judge from bonus turn charges 4 coins upon execution")
{
    Game g;
    Player attacker(g,"Att");
    Judge judge(g,"Judge");
    Player dummy(g,"D");
    g.addPlayer(&attacker);
    g.addPlayer(&judge);
    g.addPlayer(&dummy);

    attacker.setCoins(8);
    forceTurn(g, attacker);
    attacker.bribe();                          // now 4 coins
    attacker.sanction(judge);                  // queued
    attacker.gather();                         // end bonus

    forceTurn(g, dummy);
    dummy.gather();

    forceTurn(g, attacker);
    attacker.startTurn();                      // sanction executes now
    CHECK(attacker.getCoins() == 0);

    forceTurn(g, judge);
    CHECK_THROWS_AS(judge.gather(), ActionBlocked);
}

// Ensures deferred sanction still respects Judge’s surcharge
TEST_CASE("deferred sanction on Judge charges 4 coins and blocks")
{
    Game g;
    Player attacker(g,"A");
    Judge judge(g,"J");
    Player x(g,"X");
    g.addPlayer(&attacker);
    g.addPlayer(&judge);
    g.addPlayer(&x);

    attacker.setCoins(7);
    forceTurn(g, attacker);
    attacker.bribe();               // → 3 left
    attacker.sanction(judge);       // queued
    attacker.gather();              // finish bonus

    forceTurn(g, x);
    x.gather();

    forceTurn(g, attacker);
    attacker.startTurn();           // sanction applies
    CHECK(attacker.getCoins() == 0);

    forceTurn(g, judge);
    CHECK_THROWS_AS(judge.gather(), ActionBlocked);
}

} // TEST_SUITE
