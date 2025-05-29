// eitan.derdiger@gmail.com
#ifndef COUP_GAME_HPP
#define COUP_GAME_HPP

#include <vector>
#include <string>

namespace coup {
class Player;

class Game {
    friend class Player;

public:
    Game();
    ~Game();

    void addPlayer(Player* player);        // Adds a new player to the game
    std::string turn() const;             // Returns the current player's name
    void nextTurn();                       // Advances to the next valid player
    std::vector<std::string> players() const; // Returns names of all active players
    std::string winner() const;            // Returns name of the winner (if any)

private:
    std::vector<Player*> players_;         // All players in game (some may be eliminated)
    size_t turnIdx_;                       // Index of the player whose turn it is
};
}

#endif // COUP_GAME_HPP
