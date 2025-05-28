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

    void addPlayer(Player* player);

    const std::string& turn() const;

    void nextTurn();

    std::vector<std::string> players() const;

    std::string winner() const;

private:
    std::vector<Player*> players_;
    size_t turnIdx_;
};
}

#endif // COUP_GAME_HPP