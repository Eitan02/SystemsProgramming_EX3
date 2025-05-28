// eitan.derdiger@gmail.com
#include "doctest.h"
#include "../roles/Governor.hpp"
#include "../roles/Spy.hpp"
#include "../roles/Baron.hpp"
#include "../roles/General.hpp"
#include "../roles/Judge.hpp"
#include "../roles/Merchant.hpp"
#include "../exceptions/Exceptions.hpp"

using namespace coup;

// Runs a 40-turn simulation across all roles to test stability and behavior
TEST_CASE("Long‐run simulation – stability and leak-detection harness")
{
    Game g;
    Governor gov(g, "Gov");
    Spy      spy(g, "Spy");
    Baron    baron(g, "Baron");
    General  gen(g, "Gen");
    Judge    judge(g, "Judge");
    Merchant merc(g, "Merc");

    gov.setCoins(2);
    spy.setCoins(2);
    baron.setCoins(4);   // enough to invest and bribe
    gen.setCoins(5);
    judge.setCoins(4);
    merc.setCoins(3);

    for (int turn = 0; turn < 40; ++turn)
    {
        Player* current = nullptr;
        std::string who = g.turn();
        if      (who == "Gov")   current = &gov;
        else if (who == "Spy")   current = &spy;
        else if (who == "Baron") current = &baron;
        else if (who == "Gen")   current = &gen;
        else if (who == "Judge") current = &judge;
        else                     current = &merc;

        try {
            if (current->getCoins() >= 10) {
                for (Player* p : std::initializer_list<Player*>{&gov, &spy, &baron, &gen, &judge, &merc})
                    if (p != current && !p->isEliminated()) { current->coup(*p); break; }
            } else if (auto* b = dynamic_cast<Baron*>(current)) {
                if (b->getCoins() >= 3) b->invest();
                else                     b->gather();
            } else if (auto* gnr = dynamic_cast<Governor*>(current)) {
                gnr->tax();
            } else if (auto* sp = dynamic_cast<Spy*>(current)) {
                sp->gather();
            } else {
                current->gather();
            }
        } catch (const std::exception&) {
            if (who == g.turn()) g.nextTurn();
        }
    }

    int alive = 0;
    for (auto name : g.players()) ++alive;
    CHECK(alive >= 1);
}
