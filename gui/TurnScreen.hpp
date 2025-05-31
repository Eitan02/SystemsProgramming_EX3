// eitan.derdiger@gmail.com
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../game/Player.hpp"
#include "Button.hpp"
#include "PlayerView.hpp"

enum class ActionCode {
    None=-1,
    Gather, Tax, Bribe, Arrest, Sanction, Coup,
    Invest, PreventCoup, UndoBribe, BlockTax,
    SeeCoins, BlockArrest,
    SkipTurn
};

struct TurnChoice { ActionCode code; int targetIdx; };

class TurnScreen {
public:
    TurnScreen(sf::Font& f,const sf::Vector2u& ws,
               const std::vector<PlayerView>& v,size_t meIdx,
               const std::vector<bool>& valid,coup::Player* me);

    bool handle(const sf::Event& e,const sf::RenderWindow& w);
    TurnChoice choice() const { return {code_, target_}; }
    void reset();
    void draw(sf::RenderWindow& w) const;
    bool ready() const { return ready_; }

private:
    struct ActBtn { Button btn; ActionCode code; int cost; bool needsT; bool base; };
    struct TgtBtn { Button btn; int playerIdx; bool base; };

    void buildActions(); void buildTargets();
    void refreshHighlight();
    static int cost(ActionCode c);
    static bool needsTarget(ActionCode c);

    sf::Font& font_; sf::Vector2u win_;
    const std::vector<PlayerView>& views_;
    size_t meIdx_; std::vector<bool> valid_;
    coup::Player* me_;

    sf::Text title_, info_, chooseT_, err_;
    Button submit_, exitB_, restartB_, finishB_;
    std::vector<ActBtn> acts_;
    std::vector<TgtBtn> tgts_;

    ActionCode code_; int target_;
    bool ready_;
};

#include <sstream>

inline int TurnScreen::cost(ActionCode c){
    switch(c){
        case ActionCode::Bribe: return 4;
        case ActionCode::Sanction: return 3;
        case ActionCode::Coup: return 7;
        case ActionCode::Invest: return 3;
        case ActionCode::PreventCoup: return 5;
        default:return 0;
    }
}
inline bool TurnScreen::needsTarget(ActionCode c){
    return (c==ActionCode::Arrest||c==ActionCode::Sanction||c==ActionCode::Coup||
            c==ActionCode::PreventCoup||c==ActionCode::UndoBribe||
            c==ActionCode::BlockTax||c==ActionCode::SeeCoins||
            c==ActionCode::BlockArrest);
}

inline TurnScreen::TurnScreen(sf::Font& f,const sf::Vector2u& ws,
                              const std::vector<PlayerView>& v,size_t meIdx,
                              const std::vector<bool>& valid,coup::Player* me)
    :font_(f),win_(ws),views_(v),meIdx_(meIdx),valid_(valid),me_(me),
     code_(ActionCode::None),target_(-1),ready_(false)
{
    title_.setFont(font_); title_.setCharacterSize(26);
    title_.setString("Turn : "+views_[meIdx_].getName());
    title_.setPosition(20,315);

    info_.setFont(font_); info_.setCharacterSize(18);
    std::ostringstream os; os<<"Role: "<<me_->getRole()
        <<"  |  Coins: "<<me_->getCoins();
    info_.setString(os.str()); info_.setPosition(20,345);

    chooseT_.setFont(font_); chooseT_.setCharacterSize(20);
    chooseT_.setString("Choose target:");
    chooseT_.setPosition(20+3*190+60,360);

    err_.setFont(font_); err_.setCharacterSize(18);
    err_.setFillColor(sf::Color(200,80,80));
    err_.setPosition(20, win_.y-38);

    submit_ = Button(font_, "Submit", 20,
                 {static_cast<float>(win_.x) - 220.f, static_cast<float>(win_.y) - 100.f},
                 {180, 60});

    submit_.setEnabled(false);

    buildActions(); buildTargets(); refreshHighlight();
}

inline void TurnScreen::buildActions(){
    auto add=[&](std::string lbl,ActionCode c){
        int idx=acts_.size();
        float x=20+(idx%3)*190, y=380+(idx/3)*60;
        int cs=cost(c); if(cs) lbl+=" ("+std::to_string(cs)+')';
        acts_.push_back({Button(font_,lbl,16,{x,y},{180,50}),
                        c,cs,needsTarget(c),true});
        if(me_->getCoins()<cs){ acts_.back().btn.setEnabled(false);
                                acts_.back().base=false; }
    };
    add("Gather",ActionCode::Gather);
    add("Tax",ActionCode::Tax);
    add("Bribe",ActionCode::Bribe);
    add("Arrest",ActionCode::Arrest);
    add("Sanction",ActionCode::Sanction);
    add("Coup",ActionCode::Coup);
    add("Skip Turn",ActionCode::SkipTurn);

    if(dynamic_cast<coup::Baron*>(me_))    add("Invest",ActionCode::Invest);
    if(dynamic_cast<coup::General*>(me_))  add("Prevent Coup",ActionCode::PreventCoup);
    if(dynamic_cast<coup::Judge*>(me_))    add("Undo Bribe",ActionCode::UndoBribe);
    if(dynamic_cast<coup::Governor*>(me_)) add("Block Tax",ActionCode::BlockTax);
    if(dynamic_cast<coup::Spy*>(me_)){ add("See Coins",ActionCode::SeeCoins);
                                       add("Block Arrest",ActionCode::BlockArrest); }
}

inline void TurnScreen::buildTargets(){
    float x=20+3*190+60, y=390;
    for(size_t i=0;i<views_.size();++i){
        if(i==meIdx_) continue;
        TgtBtn tb{ Button(font_,views_[i].getName(),16,{x,y},{180,40}),
                   (int)i, true };
        tb.btn.setEnabled(valid_[i]); tb.base = valid_[i];
        tgts_.push_back(std::move(tb)); y+=46;
    }
}

inline void TurnScreen::refreshHighlight(){
    for(auto& a:acts_) a.btn.setEnabled(a.base);
    for(auto& a:acts_) if(a.code==code_) a.btn.setEnabled(false);

    for(auto& t:tgts_) t.btn.setEnabled(t.base);
    if(target_!=-1)
        for(auto& t:tgts_) if(t.playerIdx==target_) t.btn.setEnabled(false);

    bool formReady = (code_!=ActionCode::None) &&
                     (!needsTarget(code_) || target_!=-1);
    submit_.setEnabled(formReady);
}

inline bool TurnScreen::handle(const sf::Event& e,const sf::RenderWindow& w)
{
    for(auto& a:acts_) if(a.btn.handlePressed(e,w)){
        code_=a.code; if(!needsTarget(code_)) target_=-1;
        refreshHighlight();
    }

    if(needsTarget(code_))
        for(auto& t:tgts_) if(t.btn.handlePressed(e,w)&&t.btn.isEnabled()){
            target_=t.playerIdx; refreshHighlight();
        }

    if(submit_.handlePressed(e,w)&&submit_.isEnabled()){ ready_=true; }

    return ready_;
}

inline void TurnScreen::reset(){ ready_=false; code_=ActionCode::None; target_=-1; refreshHighlight(); }

inline void TurnScreen::draw(sf::RenderWindow& w) const
{
    for(auto& v:views_) v.draw(w);
    w.draw(title_); w.draw(info_);

    for(auto& a:acts_) a.btn.draw(w);
    if(needsTarget(code_)){ w.draw(chooseT_); for(auto& t:tgts_) t.btn.draw(w); }

    submit_.draw(w);
    if(!err_.getString().isEmpty()) w.draw(err_);
}
