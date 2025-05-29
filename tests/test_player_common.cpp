// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../game/Game.hpp"
#include "../game/Player.hpp"
#include "../roles/Judge.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

static void forceTurn(Game& g, Player& p) {
    while (g.turn() != p.getName()) g.nextTurn();
}

TEST_SUITE("Base Player - common actions & edge-cases") {

TEST_CASE("getters and setters basic") {
    Game g;
    Player p(g, "P");
    g.addPlayer(&p);
    p.setCoins(5);
    p.setEliminated(false);
    p.setArrestBlockedCount(3);

    CHECK(p.getName() == "P");
    CHECK(p.getCoins() == 5);
    CHECK(p.isEliminated() == false);
    CHECK(p.getArrestBlockedCount() == 3);
}

TEST_CASE("gather and tax increment coins when permitted") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    g.addPlayer(&a);
    g.addPlayer(&b);

    forceTurn(g, a);
    a.gather();
    CHECK(a.getCoins() == 1);
    CHECK(g.turn() == "B");

    forceTurn(g, b);
    b.tax();
    CHECK(b.getCoins() == 0);
    forceTurn(g, b);
    b.startTurn();
    CHECK(b.getCoins() == 2);
}

TEST_CASE("sanction blocks gather/tax for two turns") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    g.addPlayer(&a);
    g.addPlayer(&b);

    forceTurn(g, a);
    a.setCoins(3);
    a.sanction(b);
    CHECK(a.getCoins() == 0);
    CHECK(g.turn() == "B");

    forceTurn(g, b);
    CHECK_THROWS_AS(b.gather(), ActionBlocked);
    CHECK(g.turn() == "B");
    CHECK_THROWS_AS(b.tax(), ActionBlocked);

    forceTurn(g, b);
    b.startTurn();
    forceTurn(g, b);
    b.startTurn();
    forceTurn(g, b);
    b.gather();
    CHECK(b.getCoins() == 1);
}

TEST_CASE("bribe gives one extra immediate turn") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    g.addPlayer(&a);
    g.addPlayer(&b);

    forceTurn(g, a);
    a.setCoins(5);
    a.bribe();
    CHECK(a.getCoins() == 1);
    CHECK(g.turn() == "A");

    forceTurn(g, a);
    a.tax();
    CHECK(g.turn() == "A");

    forceTurn(g, a);
    a.gather();
    CHECK(g.turn() == "B");
}

TEST_CASE("successful arrest transfers 1 coin, cannot repeat same target") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    g.addPlayer(&a);
    g.addPlayer(&b);

    forceTurn(g, a);
    b.setCoins(2);
    a.arrest(b);
    CHECK(a.getCoins() == 1);
    CHECK(b.getCoins() == 1);

    forceTurn(g, b);
    b.gather();
    forceTurn(g, a);
    CHECK_THROWS_AS(a.arrest(b), IllegalAction);
}

TEST_CASE("player with ≥10 coins must coup – other actions throw and keep turn") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    g.addPlayer(&a);
    g.addPlayer(&b);

    a.setCoins(10);
    forceTurn(g, a);
    a.startTurn();

    CHECK_THROWS_AS(a.gather(), IllegalAction);
    CHECK(g.turn() == "A");
    CHECK_THROWS_AS(a.bribe(), IllegalAction);
    CHECK(g.turn() == "A");

    a.coup(b);
    CHECK(a.getCoins() == 3);
    CHECK(!b.isEliminated());       // coup is in pending until next turn

    forceTurn(g, b);
    b.gather();

    forceTurn(g, a);
    CHECK(b.isEliminated());
}

TEST_CASE("coup requires 7 coins and eliminates target") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    g.addPlayer(&a);
    g.addPlayer(&b);

    forceTurn(g, a);
    a.setCoins(6);
    CHECK_THROWS_AS(a.coup(b), InsufficientCoins);
    a.setCoins(7);
    a.coup(b);
    CHECK(!b.isEliminated());       // coup is in pending until next turn

    forceTurn(g, b);
    b.gather();

    forceTurn(g, a);
    CHECK(b.isEliminated());
}

TEST_CASE("NotYourTurn thrown when acting out of turn; turn unchanged on error") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    g.addPlayer(&a);
    g.addPlayer(&b);

    CHECK_THROWS_AS(b.gather(), NotYourTurn);
    CHECK(g.turn() == "A");

    forceTurn(g, a);
    a.gather();
    CHECK(g.turn() == "B");
}

TEST_CASE("Player copy constructor duplicates visible state without re-registering") {
    Game g;
    Player original(g, "Orig");
    g.addPlayer(&original);
    original.setCoins(5);

    Player clone(original);
    CHECK(clone.getName() == "Orig");
    CHECK(clone.getCoins() == 5);
    CHECK(g.players().size() == 1);
}

TEST_CASE("assignment operator copies fields and survives self-assignment") {
    Game g;
    Player p1(g, "P1");
    Player p2(g, "P2");
    g.addPlayer(&p1);
    g.addPlayer(&p2);

    p1.setCoins(4);
    p2 = p1;
    CHECK(p2.getName() == "P1");
    CHECK(p2.getCoins() == 4);

    p2 = p2;
    CHECK(p2.getCoins() == 4);
}

TEST_CASE("executePendingAction basic paths") {
    Game g; Player p(g, "P"); g.addPlayer(&p);

    PendingAction tax{ActionType::TAX, &p, nullptr, false, false};
    p.executePendingAction(tax);
    CHECK(p.getCoins() == 2);

    PendingAction gather{ActionType::GATHER, &p, nullptr, false, false};
    p.executePendingAction(gather);
    CHECK(p.getCoins() == 3);
}

TEST_CASE("findPendingOfOthers locates pending TAX") {
    Game g; Player a(g, "A"); Player b(g, "B");
    g.addPlayer(&a); g.addPlayer(&b);
    forceTurn(g, a);
    a.tax();

    auto list = Player::findPendingOfOthers(g, &b, ActionType::TAX, false);
    CHECK(list.size() == 1);
    CHECK(list.front().first == &a);
}

TEST_CASE("second sanction resets sanctionCount to two more turns") {
    Game g;
    Player att(g,"Att"); Player tgt(g,"Tar");
    g.addPlayer(&att); g.addPlayer(&tgt);
    att.setCoins(6);
    forceTurn(g, att);
    att.sanction(tgt);

    CHECK_THROWS_AS(tgt.gather(), ActionBlocked);

    forceTurn(g, att);
    att.sanction(tgt);

    forceTurn(g, tgt);
    CHECK_THROWS_AS(tgt.gather(), ActionBlocked);
}

TEST_CASE("arrest on target with zero coins throws") {
    Game g;
    Player thief(g,"Thief"); Player poor(g,"Poor");
    g.addPlayer(&thief); g.addPlayer(&poor);
    forceTurn(g, thief);
    CHECK_THROWS_AS(thief.arrest(poor), InsufficientCoins);
}

} // TEST_SUITE

TEST_SUITE("Edge cases – InsufficientCoins on various actions") {

TEST_CASE("bribe throws InsufficientCoins when player has <4 coins") {
    Game g;
    Player a(g, "A");
    g.addPlayer(&a);
    forceTurn(g, a);
    a.setCoins(3);
    CHECK_THROWS_AS(a.bribe(), InsufficientCoins);
    CHECK(g.turn() == "A");
    CHECK(a.getCoins() == 3);
}

TEST_CASE("sanction throws InsufficientCoins when attacker coins < cost") {
    Game g;
    Player attacker(g, "Att");
    Player target(g, "Tar");
    g.addPlayer(&attacker);
    g.addPlayer(&target);

    forceTurn(g, attacker);
    attacker.setCoins(2);
    CHECK_THROWS_AS(attacker.sanction(target), InsufficientCoins);
    CHECK(g.turn() == "Att");

    Judge judge(g, "Judge");
    g.addPlayer(&judge);
    g.nextTurn(); g.nextTurn(); // move back to Att
    forceTurn(g, attacker);
    attacker.setCoins(3);
    CHECK_THROWS_AS(attacker.sanction(judge), InsufficientCoins);
    CHECK(g.turn() == "Att");
}

TEST_CASE("quick check: coup with 0 coins throws InsufficientCoins") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    g.addPlayer(&a);
    g.addPlayer(&b);
    forceTurn(g, a);
    CHECK_THROWS_AS(a.coup(b), InsufficientCoins);
}

} // TEST_SUITE

TEST_CASE("arrest on target previously emptied still throws InsufficientCoins") {
    Game g;
    Player thief(g, "T");
    Player victim(g, "V");
    g.addPlayer(&thief);
    g.addPlayer(&victim);

    forceTurn(g, victim);
    victim.setCoins(1);
    forceTurn(g, thief);
    thief.arrest(victim);

    forceTurn(g, thief);
    thief.startTurn();
    forceTurn(g, victim);
    victim.startTurn();

    forceTurn(g, thief);
    CHECK_THROWS_AS(thief.arrest(victim), InsufficientCoins);
}

TEST_CASE("arrest on eliminated target throws TargetInvalid") {
    Game g;
    Player thief(g, "T");
    Player dead(g, "D");
    g.addPlayer(&thief);
    g.addPlayer(&dead);

    dead.setEliminated(true);
    forceTurn(g, thief);
    CHECK_THROWS_AS(thief.arrest(dead), TargetInvalid);
}

TEST_CASE("sanction on eliminated target throws TargetInvalid") {
    Game g;
    Player attacker(g, "A");
    Player dead(g, "D");
    g.addPlayer(&attacker);
    g.addPlayer(&dead);

    attacker.setCoins(3);
    dead.setEliminated(true);
    forceTurn(g, attacker);
    CHECK_THROWS_AS(attacker.sanction(dead), TargetInvalid);
}
