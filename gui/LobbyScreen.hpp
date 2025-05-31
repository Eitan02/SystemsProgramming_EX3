// eitan.derdiger@gmail.com
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Button.hpp"

class LobbyScreen {
public:
    LobbyScreen(sf::Font& f ,const sf::Vector2u& ws)
        : font_(f), win_(ws), focus_(false), roleIdx_(0)
    {
        if (!backgroundTex_.loadFromFile("gui/resources/BackgroundPhoto.png")) {
            throw std::runtime_error("Failed to load background image");
        }
        background_.setTexture(backgroundTex_);

        sf::Vector2u texSize = backgroundTex_.getSize();
        background_.setScale(
            static_cast<float>(win_.x) / texSize.x,
            static_cast<float>(win_.y) / texSize.y
        );

        roles_ = {"Governor","Spy","Baron","General","Judge","Merchant"};
        const float cx = win_.x * 0.5f;

        title_.setFont(font_);
        title_.setCharacterSize(34);
        title_.setString("Coup - Lobby");
        title_.setPosition(cx - title_.getLocalBounds().width * 0.5f - 100.f, 40.f);

        nameBox_.setSize({260,40});
        nameBox_.setPosition(cx-130.f ,120.f);
        nameBox_.setFillColor(sf::Color(50,50,50));
        nameBox_.setOutlineThickness(2);
        nameBox_.setOutlineColor(sf::Color::White);

        nameText_.setFont(font_);
        nameText_.setCharacterSize(20);
        nameText_.setPosition(nameBox_.getPosition().x+6.f ,nameBox_.getPosition().y+6.f);

        roleLeft_  = Button(font_,"<",18 ,{cx-160.f ,190.f} ,{40,40});
        roleLeft_.moveText(-5.f, -10.f);
        roleRight_ = Button(font_,">",18 ,{cx+120.f ,190.f} ,{40,40});
        roleRight_.moveText(-5.f, -10.f);
        roleShow_  = Button(font_,roles_.front(),18 ,{cx-110.f ,190.f} ,{210,40});
        roleShow_.moveText(0.f, 0.f);

        addBtn_    = Button(font_,"Add Player",18 ,{cx-100.f ,260.f} ,{200,50});
        addBtn_.moveText(-40.f, -5.f);
        startBtn_  = Button(font_,"Start Game",18 ,{cx-100.f ,330.f} ,{200,50});
        startBtn_.moveText(-40.f, -5.f);
        startBtn_.setEnabled(false);
    }

    bool handle(const sf::Event& e ,const sf::RenderWindow& w)
    {
        if (e.type==sf::Event::MouseButtonPressed) {
            const auto mp = sf::Mouse::getPosition(w);
            focus_ = nameBox_.getGlobalBounds().contains(static_cast<float>(mp.x)
                                                        ,static_cast<float>(mp.y));
        }

        if (focus_ && e.type==sf::Event::TextEntered) {
            if (e.text.unicode==8) { if (!input_.empty()) input_.pop_back(); }
            else if (e.text.unicode>=32 && e.text.unicode<128 && input_.size()<12)
                input_ += static_cast<char>(e.text.unicode);
            nameText_.setString(input_);
        }

        if (roleLeft_ .handlePressed(e,w)) { roleIdx_=(roleIdx_+roles_.size()-1)%roles_.size(); roleShow_.setText(roles_[roleIdx_]); }
        if (roleRight_.handlePressed(e,w)) { roleIdx_=(roleIdx_+1)%roles_.size();               roleShow_.setText(roles_[roleIdx_]); }

        if (addBtn_.handlePressed(e,w) && !input_.empty() && players_.size()<6) {
            players_.push_back({input_,roles_[roleIdx_]});
            input_.clear();
            nameText_.setString("");
        }

        if (players_.size()>=6) addBtn_.setEnabled(false);
        startBtn_.setEnabled(players_.size()>=2);

        if (startBtn_.handlePressed(e,w) && players_.size()>=2) return true;
        return false;
    }

    void draw(sf::RenderWindow& w) const
    {
        w.draw(background_);
        w.draw(title_);
        w.draw(nameBox_); w.draw(nameText_);

        roleLeft_.draw(w);  roleRight_.draw(w); roleShow_.draw(w);
        addBtn_ .draw(w);   startBtn_.draw(w);

        float y = 430.f;
        sf::Text t;  t.setFont(font_); t.setCharacterSize(20);
        for (const auto& p : players_) {
            t.setString(p.first + " - " + p.second);
            t.setPosition(80.f ,y);
            y += 26.f;
            w.draw(t);
        }
    }

    std::vector<std::pair<std::string,std::string>> takePlayers() { return players_; }

private:
    sf::Font&                       font_;
    sf::Vector2u                    win_;
    sf::Texture                    backgroundTex_;
    sf::Sprite                     background_;

    sf::Text                        title_ ,nameText_;
    sf::RectangleShape              nameBox_;
    Button                          roleLeft_ ,roleRight_ ,roleShow_;
    Button                          addBtn_ ,startBtn_;

    bool                            focus_;
    std::string                     input_;
    std::vector<std::string>        roles_;
    int                             roleIdx_;
    std::vector<std::pair<std::string,std::string>> players_;
};
