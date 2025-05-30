// eitan.derdiger@gmail.com
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Button.hpp"

class LobbyScreen {
public:
    LobbyScreen(sf::Font& f ,const sf::Vector2u& ws)
        : font_(f) ,focus_(false) ,roleIdx_(0) ,exit_(false) ,win_(ws)
    {
        roles_ = {"Governor","Spy","Baron","General","Judge","Merchant"};
        const float cx = win_.x * 0.5f;

        /* כותרת */
        title_.setFont(font_);
        title_.setCharacterSize(34);
        title_.setString("Coup – Lobby");
        title_.setPosition(cx - title_.getLocalBounds().width*0.5f ,40.f);

        /* תיבת-שם */
        nameBox_.setSize({260,40});
        nameBox_.setPosition(cx-130.f ,120.f);
        nameBox_.setFillColor(sf::Color(50,50,50));
        nameBox_.setOutlineThickness(2);
        nameBox_.setOutlineColor(sf::Color::White);

        nameText_.setFont(font_);
        nameText_.setCharacterSize(20);
        nameText_.setPosition(nameBox_.getPosition().x+6.f ,nameBox_.getPosition().y+6.f);

        /* בחירת Role */
        roleLeft_  = Button(font_,"<",18 ,{cx-160.f ,190.f} ,{40,40});
        roleRight_ = Button(font_,">",18 ,{cx+120.f ,190.f} ,{40,40});
        roleShow_  = Button(font_,roles_.front(),18 ,{cx-110.f ,190.f} ,{210,40});

        /* כפתורים עיקריים */
        addBtn_    = Button(font_,"Add Player",18 ,{cx-100.f ,260.f} ,{200,50});
        startBtn_  = Button(font_,"Start Game",18 ,{cx-100.f ,330.f} ,{200,50});
        startBtn_.setEnabled(false);

        exitBtn_   = Button(font_,"Exit",20 ,{20.f ,static_cast<float>(win_.y)-60.f} ,{100,40});
    }

    /*=============================================================*/
    bool handle(const sf::Event& e ,const sf::RenderWindow& w)
    {
        /* קליק בתוך תיבת-שם מפעיל focus  */
        if (e.type==sf::Event::MouseButtonPressed) {
            const auto mp = sf::Mouse::getPosition(w);
            focus_ = nameBox_.getGlobalBounds().contains(static_cast<float>(mp.x)
                                                        ,static_cast<float>(mp.y));
        }

        /* הזנת שם */
        if (focus_ && e.type==sf::Event::TextEntered) {
            if (e.text.unicode==8) { if (!input_.empty()) input_.pop_back(); }
            else if (e.text.unicode>=32 && e.text.unicode<128 && input_.size()<12)
                input_ += static_cast<char>(e.text.unicode);
            nameText_.setString(input_);
        }

        /* בחירת Role */
        if (roleLeft_ .handlePressed(e,w)) { roleIdx_=(roleIdx_+roles_.size()-1)%roles_.size(); roleShow_.setText(roles_[roleIdx_]); }
        if (roleRight_.handlePressed(e,w)) { roleIdx_=(roleIdx_+1)%roles_.size();               roleShow_.setText(roles_[roleIdx_]); }

        /* Add Player */
        if (addBtn_.handlePressed(e,w) && !input_.empty() && players_.size()<6) {
            players_.push_back({input_,roles_[roleIdx_]});
            input_.clear();
            nameText_.setString("");
        }

        /* כפתורי-מצב */
        if (players_.size()>=6) addBtn_.setEnabled(false);
        startBtn_.setEnabled(players_.size()>=2);

        if (exitBtn_.handlePressed(e,w)) { exit_ = true; return false; }
        if (startBtn_.handlePressed(e,w) && players_.size()>=2) return true;
        return false;
    }

    /*=============================================================*/
    void draw(sf::RenderWindow& w) const
    {
        w.draw(title_);
        w.draw(nameBox_); w.draw(nameText_);

        roleLeft_.draw(w);  roleRight_.draw(w); roleShow_.draw(w);
        addBtn_ .draw(w);   startBtn_.draw(w);  exitBtn_.draw(w);

        /* רשימת שחקנים */
        float y = 430.f;
        sf::Text t;  t.setFont(font_); t.setCharacterSize(20);
        for (const auto& p : players_) {
            t.setString(p.first + " – " + p.second);
            t.setPosition(80.f ,y);
            y += 26.f;
            w.draw(t);
        }
    }

    std::vector<std::pair<std::string,std::string>> takePlayers() { return players_; }
    bool wantExit() const { return exit_; }

private:
    sf::Font&                       font_;
    sf::Vector2u                    win_;

    /* UI */
    sf::Text                        title_ ,nameText_;
    sf::RectangleShape              nameBox_;
    Button                          roleLeft_ ,roleRight_ ,roleShow_;
    Button                          addBtn_ ,startBtn_ ,exitBtn_;

    /* state */
    bool                            focus_;
    std::string                     input_;
    std::vector<std::string>        roles_;
    int                             roleIdx_;
    std::vector<std::pair<std::string,std::string>> players_;
    bool                            exit_;
};
