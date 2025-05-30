// eitan.derdiger@gmail.com
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "../game/Game.hpp"
#include "../roles/Governor.hpp"
#include "../roles/Spy.hpp"
#include "../roles/Baron.hpp"
#include "../roles/General.hpp"
#include "../roles/Judge.hpp"
#include "../roles/Merchant.hpp"
#include "LobbyScreen.hpp"
#include "TurnScreen.hpp"
#include "PlayerView.hpp"

class ScreenManager {
public:
    ScreenManager();
    void run();

private:
    enum class State { Lobby, Turn, Over };

    sf::RenderWindow                    window_;
    sf::Font                            font_;
    State                               state_;
    LobbyScreen                         lobby_;

    std::unique_ptr<coup::Game>         game_;
    std::vector<std::unique_ptr<coup::Player>> players_;
    std::vector<PlayerView>             views_;
    size_t                              currentIdx_ = 0;

    std::unique_ptr<TurnScreen>         turn_;
    sf::Text                            winner_;
    sf::Text                            globalError_;

    /* helpers */
    void handle(const sf::Event& e);
    void draw();
    void startGame();
    void buildViews();
    void refreshViews();
    static std::string roleOf(coup::Player* p);
    void openTurnScreen();
    void nextIdx();
    void processTurnChoice(const TurnChoice& ch);
};

/*====================== implementation ===============================*/
#include <sstream>

inline ScreenManager::ScreenManager()
    : window_(sf::VideoMode(1024,768), "Coup GUI"),
      font_(), state_(State::Lobby),
      lobby_(font_, window_.getSize())
{
    font_.loadFromFile("gui/resources/Roboto.ttf");
    window_.setFramerateLimit(60);

    winner_.setFont(font_); winner_.setCharacterSize(36);
    winner_.setPosition(150.f, 340.f);

    globalError_.setFont(font_); globalError_.setCharacterSize(20);
    globalError_.setFillColor(sf::Color(220,80,80));
    globalError_.setPosition(20.f, window_.getSize().y - 35.f);
}

inline void ScreenManager::run()
{
    while (window_.isOpen()) {
        sf::Event e;
        while (window_.pollEvent(e)) {
            if (e.type==sf::Event::Closed) window_.close();
            handle(e);
        }
        window_.clear(sf::Color(25,25,25));
        draw();
        window_.display();
    }
}

/*-- handle -----------------------------------------------------------*/
inline void ScreenManager::handle(const sf::Event& e)
{
    if (state_ == State::Lobby) {
        if (lobby_.handle(e, window_)) startGame();
        return;
    }
    if (state_ == State::Turn) {
        if (!turn_) return;
        if (turn_->handle(e, window_)) {
            processTurnChoice(turn_->choice());
            if (globalError_.getString().isEmpty())
                openTurnScreen();
            else
                turn_->reset();
        }
        return;
    }
    if (state_ == State::Over &&
        e.type==sf::Event::KeyPressed &&
        e.key.code==sf::Keyboard::Escape)
        window_.close();
}

/*-- draw -------------------------------------------------------------*/
inline void ScreenManager::draw()
{
    if (state_ == State::Lobby) { lobby_.draw(window_); return; }
    if (state_ == State::Turn)  {
        if (turn_) turn_->draw(window_);
        if (!globalError_.getString().isEmpty())
            window_.draw(globalError_);
        return;
    }
    window_.draw(winner_);
}

/*-- startGame --------------------------------------------------------*/
inline void ScreenManager::startGame()
{
    auto list = lobby_.takePlayers();
    if (list.size() < 2) return;

    game_ = std::make_unique<coup::Game>();
    players_.clear();  currentIdx_ = 0;

    for (auto& pr : list) {
        coup::Player* p=nullptr;
        if (pr.second=="Governor")     p=new coup::Governor (*game_, pr.first);
        else if (pr.second=="Spy")     p=new coup::Spy      (*game_, pr.first);
        else if (pr.second=="Baron")   p=new coup::Baron    (*game_, pr.first);
        else if (pr.second=="General") p=new coup::General  (*game_, pr.first);
        else if (pr.second=="Judge")   p=new coup::Judge    (*game_, pr.first);
        else                           p=new coup::Merchant (*game_, pr.first);

        players_.emplace_back(p);
        game_->addPlayer(p);
    }

    buildViews();
    players_[currentIdx_]->startTurn();
    openTurnScreen();
    state_ = State::Turn;
}

/*-- views ------------------------------------------------------------*/
inline void ScreenManager::buildViews()
{
    views_.clear();
    const float x0=20.f, y0=20.f;
    for (size_t i=0;i<players_.size();++i) {
        const float x=x0+(i%5)*190.f;
        const float y=y0+(i/5)*100.f;
        views_.emplace_back(font_, sf::Vector2f(x,y));
    }
}
inline void ScreenManager::refreshViews()
{
    for (size_t i=0;i<players_.size();++i) {
        auto& v = views_[i];
        auto* p = players_[i].get();
        v.setName(p->getName());
        v.setRole(roleOf(p));
        v.setCoins(p->getCoins());
        v.setEliminated(p->isEliminated());
        v.setActive(i==currentIdx_);
        v.setMinimal(false);                     // debug-mode
    }
}
inline std::string ScreenManager::roleOf(coup::Player* p)
{
    using namespace coup;
    if (dynamic_cast<Governor*>(p)) return "Governor";
    if (dynamic_cast<Spy*>(p))      return "Spy";
    if (dynamic_cast<Baron*>(p))    return "Baron";
    if (dynamic_cast<General*>(p))  return "General";
    if (dynamic_cast<Judge*>(p))    return "Judge";
    return "Merchant";
}

/*-- openTurnScreen ---------------------------------------------------*/
inline void ScreenManager::openTurnScreen()
{
    if (!game_ || players_.empty()) return;
    refreshViews();

    std::vector<bool> valid(players_.size(),false);
    for(size_t i=0;i<players_.size();++i)
        if(!players_[i]->isEliminated() && i!=currentIdx_)
            valid[i]=true;

    turn_ = std::make_unique<TurnScreen>(
        font_, window_.getSize(),
        views_, currentIdx_, valid,
        players_[currentIdx_].get());

    globalError_.setString("");
}

/*-- helpers ----------------------------------------------------------*/
inline void ScreenManager::nextIdx()
{
    do { currentIdx_ = (currentIdx_+1)%players_.size(); }
    while (players_[currentIdx_]->isEliminated());
}

/*-- processTurnChoice ------------------------------------------------*/
inline void ScreenManager::processTurnChoice(const TurnChoice& ch)
{
    globalError_.setString("");
    auto& me = *players_[currentIdx_];
    coup::Player* tgt = (ch.targetIdx==-1)?nullptr:players_[ch.targetIdx].get();

    try {
        switch (ch.code) {
            case ActionCode::Gather:        me.gather();                          break;
            case ActionCode::Tax:           me.tax();                             break;
            case ActionCode::Bribe:         me.bribe();                           break;
            case ActionCode::Arrest:        me.arrest(*tgt);                      break;
            case ActionCode::Sanction:      me.sanction(*tgt);                    break;
            case ActionCode::Coup:          me.coup(*tgt);                        break;
            case ActionCode::Invest:        dynamic_cast<coup::Baron&>(me).invest();           break;
            case ActionCode::PreventCoup:   dynamic_cast<coup::General&>(me).preventCoup(*tgt);break;
            case ActionCode::UndoBribe:     dynamic_cast<coup::Judge&>(me).undoBribe(*tgt);    break;
            case ActionCode::BlockTax:      dynamic_cast<coup::Governor&>(me).blockTax(*tgt);  break;
            case ActionCode::SeeCoins: {
                int c = dynamic_cast<coup::Spy&>(me).seeCoins(*tgt);
                std::ostringstream oss; oss<<tgt->getName()<<" has "<<c<<" coins";
                globalError_.setString(oss.str());                               break;
            }
            case ActionCode::BlockArrest:   dynamic_cast<coup::Spy&>(me).blockArrest(*tgt);    break;
            case ActionCode::SkipTurn:      game_->nextTurn();                    break;
            default:                        break;
        }
    }
    catch(const std::exception& ex){
        globalError_.setString(ex.what());
        return;
    }

    /* winner? */
    try {
        winner_.setString("Winner: "+game_->winner()+"\nEsc – Exit");
        state_ = State::Over;
        return;
    } catch (...) {}

    /* advance turn */
    std::string next = game_->turn();
    for(size_t i=0;i<players_.size();++i)
        if(players_[i]->getName()==next){ currentIdx_=i; break; }
}
