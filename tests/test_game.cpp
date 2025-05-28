// eitan.derdiger@gmail.com
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../game/Game.hpp"
#include "../game/Player.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

// Verifies player addition, turn order and active players list
TEST_CASE("Game::addPlayer, turn order and players list")
{
    Game g;
    Player a(g, "Alice");
    Player b(g, "Bob");
    Player c(g, "Charlie");

    CHECK(g.turn() == "Alice");                 // first player starts
    CHECK(g.players().size() == 3);
    std::vector<std::string> expected{"Alice", "Bob", "Charlie"};
    CHECK(g.players() == expected);

    // Tests rejection of adding more than 6 players
    SUBCASE("adding beyond 6 players throws")
    {
        Player d(g, "D");
        Player e(g, "E");
        Player f(g, "F");
        CHECK_THROWS_AS(Player(g, "G"), IllegalAction);
    }
}

// Checks that nextTurn advances properly and skips eliminated players
TEST_CASE("Game::nextTurn cycles and skips eliminated players")
{
    Game g;
    Player a(g, "A");
    Player b(g, "B");
    Player c(g, "C");

    g.nextTurn();                               // A → B
    CHECK(g.turn() == "B");

    c.setEliminated(true);                      // remove C
    g.nextTurn();                               // B → A (C skipped)
    CHECK(g.turn() == "A");
}

// Ensures winner() works correctly and throws if game isn't finished
TEST_CASE("Game::winner reports correctly and throws when game ongoing")
{
    Game g;
    Player a(g, "Solo");
    CHECK_THROWS_AS(g.winner(), IllegalAction); // game not done yet

    // eliminate all but one
    Player b(g, "Gone1");
    Player c(g, "Gone2");
    b.setEliminated(true);
    c.setEliminated(true);

    CHECK(g.winner() == "Solo");
}

// Validates game behavior when no players are added
TEST_CASE("Game edge cases: empty game and invalid turn access")
{
    Game g;
    CHECK_THROWS_AS(g.turn(), IllegalAction);
    CHECK_NOTHROW(g.players().empty());
    CHECK_THROWS_AS(g.winner(), IllegalAction);
}
