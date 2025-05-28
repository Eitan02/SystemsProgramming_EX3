// eitan.derdiger@gmail.com
#ifndef COUP_JUDGE_HPP
#define COUP_JUDGE_HPP

#include "../game/Player.hpp"

namespace coup {

class Judge : public Player {
public:
    Judge(Game& game, const std::string& name);
    void undoBribe(Player& target);
};

}

#endif // COUP_JUDGE_HPP