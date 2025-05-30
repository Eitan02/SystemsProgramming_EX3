// eitan.derdiger@gmail.com
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <string>
#include "Button.hpp"

class ActionDialog {
public:
    ActionDialog(sf::Font& f,const sf::Vector2u& ws)
        : font_(f),open_(false),pickOpen_(false),chosenT_(0)
    {
        bg_.setSize({600.f,250.f});
        bg_.setFillColor(sf::Color(20,20,20,230));
        bg_.setOrigin(bg_.getSize()/2.f);
        bg_.setPosition(ws.x/2.f,ws.y/2.f);

        const char* lbl[6]={"Gather","Tax","Bribe","Arrest","Sanction","Coup"};
        for(int i=0;i<6;++i){
            float x=bg_.getPosition().x-260+i*90;
            float y=bg_.getPosition().y-80;
            buttons_.push_back(Button(font_,lbl[i],16,{x,y},{80,50}));
        }
        pickBtn_=Button(font_,"Target ▼",16,
                        {bg_.getPosition().x-70,bg_.getPosition().y+40},
                        {140,40});
    }

    void open(const std::vector<std::string>& names,
              const std::vector<bool>& valid)
    {
        names_=names; valid_=valid;
        pickItems_.clear();
        for(size_t i=0;i<names.size();++i){
            float y=bg_.getPosition().y+90+i*38;
            pickItems_.push_back(Button(font_,names[i],14,
                            {bg_.getPosition().x-70,y},{140,34}));
            pickItems_.back().setEnabled(valid[i]);
        }
        chosenT_=0;
        pickBtn_.setText("Target ▼");
        pickBtn_.setEnabled(!names.empty());
        open_=true; pickOpen_=false;
    }

    bool isOpen()const{ return open_; }

    std::optional<std::pair<int,int>> poll(const sf::Event& e,const sf::RenderWindow& w)
    {
        if(!open_) return std::nullopt;

        if(pickBtn_.handlePressed(e,w)) pickOpen_=!pickOpen_;

        if(pickOpen_){
            for(size_t i=0;i<pickItems_.size();++i){
                if(pickItems_[i].handlePressed(e,w) && valid_[i]){
                    chosenT_=i; pickOpen_=false;
                    pickBtn_.setText(names_[i]);
                }
            }
        }

        for(size_t i=0;i<buttons_.size();++i){
            if(buttons_[i].handlePressed(e,w)){
                if(needsTarget(i)&&!valid_[chosenT_]) return std::nullopt;
                open_=false;
                return std::make_pair<int,int>(static_cast<int>(i),static_cast<int>(chosenT_));
            }
        }
        return std::nullopt;
    }

    void draw(sf::RenderWindow& w)const{
        if(!open_) return;
        w.draw(bg_);
        for(const auto& b:buttons_) b.draw(w);
        pickBtn_.draw(w);
        if(pickOpen_) for(const auto& it:pickItems_) it.draw(w);
    }

private:
    bool needsTarget(int idx)const{ return idx>=3; }

    sf::Font& font_;
    sf::RectangleShape bg_;
    std::vector<Button> buttons_;
    Button pickBtn_;
    std::vector<Button> pickItems_;
    std::vector<std::string> names_;
    std::vector<bool> valid_;
    bool open_,pickOpen_;
    size_t chosenT_;
};
