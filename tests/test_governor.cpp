//eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/Governor.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

static void forceTurn(Game& g, Player& p) {
    while (g.turn() != p.getName()) {
        g.nextTurn();
    }
}

TEST_SUITE("Governor role – special tax and blockTax") {

// Ensures Governor's tax action grants 3 coins and proceeds to next turn
TEST_CASE("Governor tax gives 3 coins and advances turn")
{
    Game g;
    Governor gov(g, "Gov");
    Player   p (g, "P");
    g.addPlayer(&gov);
    g.addPlayer(&p);

    forceTurn(g, gov);
    gov.tax();
    forceTurn(g,gov);
    CHECK(gov.getCoins() == 3);
    gov.gather();
    CHECK(g.turn() == "P");

}

// Tests that sanction prevents Governor from using tax for two turns
TEST_CASE("sanction prevents Governor from taxing for two turns")
{
    Game g;
    Governor gov(g, "Gov");
    Player   attacker(g, "Att");
    g.addPlayer(&gov);
    g.addPlayer(&attacker);

    attacker.setCoins(3);
    forceTurn(g, attacker);
    attacker.sanction(gov);                       // turn → Gov, sanctionCount_=2

    forceTurn(g, gov);
    CHECK_THROWS_AS(gov.tax(), ActionBlocked);    // turn stays Gov
    CHECK(g.turn() == "Gov");

    gov.startTurn(); gov.startTurn();             // wait out sanction
    forceTurn(g, gov);
    gov.tax();
    forceTurn(g,gov);                                    // now allowed
    CHECK(gov.getCoins() == 3);
}

// Confirms Governor can block another player's pending tax action
TEST_CASE("Governor blocks pending tax of another player")
{
    Game g;
    Player   victim(g, "Victim");
    Governor blocker(g, "Blocker");
    g.addPlayer(&victim);
    g.addPlayer(&blocker);

    forceTurn(g, victim);
    victim.tax();                                 // pending +2
    CHECK(g.turn() == "Blocker");

    forceTurn(g, blocker);
    blocker.blockTax(victim);                     // mark as blocked, turn → Victim
    CHECK(g.turn() == "Victim");

    forceTurn(g, victim);
    victim.startTurn();                           // pending executes (but blocked)
    CHECK(victim.getCoins() == 0);                // no gain
}

// Checks that blockTax fails when the target has no pending tax
TEST_CASE("blockTax when there is no pending tax throws IllegalAction")
{
    Game g;
    Player   p(g, "P");
    Governor gov(g, "Gov");
    g.addPlayer(&p);
    g.addPlayer(&gov);

    forceTurn(g, gov);
    CHECK_THROWS_AS(gov.blockTax(p), IllegalAction);
}

// Ensures blockTax fails when the target is eliminated
TEST_CASE("blockTax on eliminated target throws TargetInvalid")
{
    Game g;
    Player   p(g, "Dead");
    Governor gov(g, "Gov");
    g.addPlayer(&p);
    g.addPlayer(&gov);

    p.setEliminated(true);
    forceTurn(g, gov);
    CHECK_THROWS_AS(gov.blockTax(p), TargetInvalid);
}

// Validates that Governor cannot block his own tax action
TEST_CASE("Governor cannot block his own pending tax")
{
    Game g;
    Governor gov(g,"Gov");
    g.addPlayer(&gov);

    forceTurn(g, gov);
    gov.tax(); // creates his own pending action

    forceTurn(g, gov);
    CHECK_THROWS_AS(gov.blockTax(gov), IllegalAction);
}

// Ensures deferred Governor tax from bonus turn gives 3 coins later
TEST_CASE("Governor deferred tax from bonus turn gives 3 coins")
{
    Game g;
    Governor gov(g,"Gov"); Player other(g,"O");
    g.addPlayer(&gov);
    g.addPlayer(&other);

    gov.setCoins(4);
    forceTurn(g, gov);
    gov.bribe();          // pay 4, extra turn
    gov.tax();            // queued TAX_Governor
    gov.gather();         // finish bonus turn

    forceTurn(g, other);
    other.gather();       // Gov's turn again

    forceTurn(g, gov);
    gov.startTurn();      // pending executes
    CHECK(gov.getCoins() == 4);   // 0+3+1
}

} // TEST_SUITE
