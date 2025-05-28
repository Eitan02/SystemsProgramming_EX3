// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../game/Game.hpp"
#include "../game/Player.hpp"
#include "../roles/Judge.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

TEST_SUITE("Base Player - common actions & edge-cases") {

// Basic checks for getter/setter correctness
TEST_CASE("getters and setters basic") {
    Game g;
    Player p(g, "P");
    p.setCoins(5);
    p.setEliminated(false);
    p.setArrestBlockedCount(3);

    CHECK(p.getName() == "P");
    CHECK(p.getCoins() == 5);
    CHECK(p.isEliminated() == false);
    CHECK(p.getArrestBlockedCount() == 3);
}

// Tests gather and tax behavior when nothing blocks them
TEST_CASE("gather and tax increment coins when permitted") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");

    a.gather();
    CHECK(a.getCoins() == 1);
    CHECK(g.turn() == "B");

    b.tax();
    CHECK(b.getCoins() == 0);
    b.startTurn();
    CHECK(b.getCoins() == 2);
}

// Sanction blocks gather and tax for two full turns
TEST_CASE("sanction blocks gather/tax for two turns") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");

    a.setCoins(3);
    a.sanction(b);
    CHECK(a.getCoins() == 0);
    CHECK(g.turn() == "B");

    CHECK_THROWS_AS(b.gather(), ActionBlocked);
    CHECK(g.turn() == "B");
    CHECK_THROWS_AS(b.tax(), ActionBlocked);

    b.startTurn(); b.startTurn();
    b.gather();
    CHECK(b.getCoins() == 1);
}

// Validates bribe grants 1 extra immediate turn
TEST_CASE("bribe gives one extra immediate turn") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");

    a.setCoins(5);
    a.bribe();
    CHECK(a.getCoins() == 1);
    CHECK(g.turn() == "A");

    a.tax();
    CHECK(g.turn() == "A");

    a.gather();
    CHECK(g.turn() == "B");
}

// Arrest steals 1 coin and target can't be arrested twice in a row
TEST_CASE("successful arrest transfers 1 coin, cannot repeat same target") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");

    b.setCoins(2);
    a.arrest(b);
    CHECK(a.getCoins() == 1);
    CHECK(b.getCoins() == 1);

    b.gather();
    CHECK_THROWS_AS(a.arrest(b), IllegalAction);
}

// Player with 10+ coins must perform coup or get blocked
TEST_CASE("player with ≥10 coins must coup – other actions throw and keep turn") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");

    a.setCoins(10);
    a.startTurn();

    CHECK_THROWS_AS(a.gather(), IllegalAction);
    CHECK(g.turn() == "A");
    CHECK_THROWS_AS(a.bribe(), IllegalAction);

    a.coup(b);
    CHECK(a.getCoins() == 3);
    CHECK(b.isEliminated());
}

// Coup eliminates target and requires 7 coins
TEST_CASE("coup requires 7 coins and eliminates target") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");

    a.setCoins(6);
    CHECK_THROWS_AS(a.coup(b), InsufficientCoins);
    a.setCoins(7);
    a.coup(b);
    CHECK(b.isEliminated());
}

// Actions out of turn throw and do not change turn state
TEST_CASE("NotYourTurn thrown when acting out of turn; turn unchanged on error") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");

    CHECK_THROWS_AS(b.gather(), NotYourTurn);
    CHECK(g.turn() == "A");

    a.gather();
    CHECK(g.turn() == "B");
}

// Copy constructor copies visible state, does not re-register player
TEST_CASE("Player copy constructor duplicates visible state without re-registering") {
    Game g;
    Player original(g, "Orig");
    original.setCoins(5);

    Player clone(original);
    CHECK(clone.getName() == "Orig");
    CHECK(clone.getCoins() == 5);
    CHECK(g.players().size() == 1);
}

// Assignment operator copies fields and supports self-assignment
TEST_CASE("assignment operator copies fields and survives self-assignment") {
    Game g;
    Player p1(g, "P1");
    Player p2(g, "P2");

    p1.setCoins(4);
    p2 = p1;
    CHECK(p2.getName() == "P1");
    CHECK(p2.getCoins() == 4);

    p2 = p2;
    CHECK(p2.getCoins() == 4);
}

// Directly test execution of simple pending actions
TEST_CASE("executePendingAction basic paths") {
    Game g; Player p(g, "P");

    PendingAction tax{ActionType::TAX, &p, nullptr, false, false};
    p.executePendingAction(tax);
    CHECK(p.getCoins() == 2);

    PendingAction gather{ActionType::GATHER, &p, nullptr, false, false};
    p.executePendingAction(gather);
    CHECK(p.getCoins() == 3);
}

// Verifies pending tax is found correctly by search
TEST_CASE("findPendingOfOthers locates pending TAX") {
    Game g; Player a(g, "A"); Player b(g, "B");
    a.tax();

    auto list = Player::findPendingOfOthers(g, &b, ActionType::TAX, false);
    CHECK(list.size() == 1);
    CHECK(list.front().first == &a);
}

// Second sanction restarts duration rather than stack
TEST_CASE("second sanction resets sanctionCount to two more turns") {
    Game g;
    Player att(g,"Att"); Player tgt(g,"Tar");
    att.setCoins(6);
    att.sanction(tgt);

    tgt.startTurn();
    CHECK_THROWS_AS(tgt.gather(), ActionBlocked);

    att.startTurn();
    att.sanction(tgt);

    CHECK_THROWS_AS(tgt.gather(), ActionBlocked);
}

// Arrest fails when target has no coins
TEST_CASE("arrest on target with zero coins throws") {
    Game g;
    Player thief(g,"Thief"); Player poor(g,"Poor");
    CHECK_THROWS_AS(thief.arrest(poor), InsufficientCoins);
}

} // TEST_SUITE

TEST_SUITE("Edge cases – InsufficientCoins on various actions") {

// Bribe fails when player has fewer than 4 coins
TEST_CASE("bribe throws InsufficientCoins when player has <4 coins") {
    Game g;
    Player a(g, "A");
    a.setCoins(3);
    CHECK_THROWS_AS(a.bribe(), InsufficientCoins);
    CHECK(g.turn() == "A");
    CHECK(a.getCoins() == 3);
}

// Sanction fails when attacker lacks coins: 3 for regular, 4 for Judge
TEST_CASE("sanction throws InsufficientCoins when attacker coins < cost") {
    Game g;
    Player attacker(g, "Att");
    Player target(g, "Tar");

    attacker.setCoins(2);
    CHECK_THROWS_AS(attacker.sanction(target), InsufficientCoins);
    CHECK(g.turn() == "Att");

    Judge judge(g, "Judge");
    g.nextTurn();
    g.nextTurn();
    attacker.setCoins(3);
    CHECK_THROWS_AS(attacker.sanction(judge), InsufficientCoins);
    CHECK(g.turn() == "Att");
}

// Coup throws if player has less than 7 coins
TEST_CASE("quick check: coup with 0 coins throws InsufficientCoins") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    CHECK_THROWS_AS(a.coup(b), InsufficientCoins);
}

} // TEST_SUITE

// Arrest still fails if target already has 0 coins from earlier arrest
TEST_CASE("arrest on target previously emptied still throws InsufficientCoins") {
    Game g;
    Player thief(g, "T");
    Player victim(g, "V");
    victim.setCoins(1);
    thief.arrest(victim);
    thief.startTurn();
    victim.startTurn();
    CHECK_THROWS_AS(thief.arrest(victim), InsufficientCoins);
}

// Validates that a deferred COUP from bribe executes and eliminates
TEST_CASE("COUP queued from bonus turn executes correctly") {
    Game g;
    Player killer(g, "K");
    Player victim(g, "V");
    Player dummy(g, "D");

    killer.setCoins(11);
    killer.bribe();
    killer.setCoins(7);
    killer.coup(victim);
    killer.gather();
    dummy.gather();
    killer.startTurn();
    CHECK(victim.isEliminated());
    CHECK(killer.getCoins() == 0);
}

// Checks that only bonus-turn actions are returned with onlyBribe=true
TEST_CASE("findPendingOfOthers filters only bonus-turn actions") {
    Game g;
    Player a(g, "A");
    Player b(g, "B");

    a.setCoins(4);
    a.bribe();
    a.gather(); // from_bribe
    a.gather(); // regular
    b.gather();

    auto bonus = Player::findPendingOfOthers(g, &b, ActionType::GATHER, true);
    auto all = Player::findPendingOfOthers(g, &b, ActionType::GATHER, false);

    CHECK(bonus.size() == 1);
    CHECK(all.size() == 2);
}

// Arrest fails if target is eliminated
TEST_CASE("arrest on eliminated target throws TargetInvalid") {
    Game g;
    Player thief(g, "T");
    Player dead(g, "D");
    dead.setEliminated(true);
    CHECK_THROWS_AS(thief.arrest(dead), TargetInvalid);
}

// Sanction fails if target is eliminated
TEST_CASE("sanction on eliminated target throws TargetInvalid") {
    Game g;
    Player attacker(g, "A");
    Player dead(g, "D");
    attacker.setCoins(3);
    dead.setEliminated(true);
    CHECK_THROWS_AS(attacker.sanction(dead), TargetInvalid);
}
