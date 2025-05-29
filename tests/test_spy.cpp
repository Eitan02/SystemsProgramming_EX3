// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/Spy.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

static void forceTurn(Game& g, Player& p) {
    while (g.turn() != p.getName()) {
        g.nextTurn();
    }
}

TEST_SUITE("Spy role – seeCoins & blockArrest") {

// Checks that seeCoins returns the correct coin count from target
TEST_CASE("Spy sees target coin count accurately")
{
    Game g;
    Spy     spy(g, "Spy");
    Player  target(g, "Target");
    g.addPlayer(&spy);
    g.addPlayer(&target);

    target.setCoins(4);
    CHECK(spy.seeCoins(target) == 4);

    forceTurn(g, target);
    target.gather();
    forceTurn(g, spy);
    spy.startTurn(); // rotate turn
    CHECK(spy.seeCoins(target) == 5);
}

// Ensures seeCoins throws if the target is eliminated
TEST_CASE("seeCoins on eliminated target throws TargetInvalid")
{
    Game g;
    Spy    spy(g, "Spy");
    Player dead(g, "Dead");
    g.addPlayer(&spy);
    g.addPlayer(&dead);

    dead.setEliminated(true);
    CHECK_THROWS_AS(spy.seeCoins(dead), TargetInvalid);
}

// Validates that Spy cannot use seeCoins if mustCoup is active
TEST_CASE("Spy must coup when holding ≥10 coins")
{
    Game g;
    Spy   spy(g, "Spy");
    Player p(g, "P");
    g.addPlayer(&spy);
    g.addPlayer(&p);

    spy.setCoins(10);
    forceTurn(g, spy);
    spy.startTurn(); // triggers mustCoup_

    CHECK_THROWS_AS(spy.seeCoins(p), IllegalAction);
    CHECK(g.turn() == "Spy");
}

// Verifies blockArrest prevents arresting for two turns
TEST_CASE("blockArrest sets arrestBlockedCount and enforces ActionBlocked")
{
    Game g;
    Spy    spy(g, "Spy");
    Player blocked(g, "Blocked");
    Player victim(g, "Victim");
    g.addPlayer(&spy);
    g.addPlayer(&blocked);
    g.addPlayer(&victim);

    victim.setCoins(5);

    forceTurn(g, spy);
    spy.blockArrest(blocked);
    CHECK(blocked.getArrestBlockedCount() == 1);
    forceTurn(g, blocked);
    CHECK(g.turn() == "Blocked");

    CHECK_THROWS_AS(blocked.arrest(victim), ActionBlocked);
    CHECK(g.turn() == "Blocked");

    blocked.startTurn();
    CHECK_NOTHROW(blocked.arrest(victim));
    forceTurn(g, victim);
    victim.gather();

    forceTurn(g, blocked);
    blocked.startTurn(); // block expires

    forceTurn(g, victim);
    victim.gather(); // give 1 coin
    forceTurn(g, blocked);
    CHECK(blocked.getCoins() == 1);
    CHECK(victim.getCoins() == 6);
}

// Ensures blockArrest fails if the target is already eliminated
TEST_CASE("blockArrest on eliminated player throws TargetInvalid")
{
    Game g;
    Spy    spy(g, "Spy");
    Player dead(g, "Dead");
    g.addPlayer(&spy);
    g.addPlayer(&dead);

    dead.setEliminated(true);
    CHECK_THROWS_AS(spy.blockArrest(dead), TargetInvalid);
}

} // TEST_SUITE
