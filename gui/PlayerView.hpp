// eitan.derdiger@gmail.com
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/*-----------------------------------------------------------
 *  A light “card” that shows a player inside the top bar.
 *----------------------------------------------------------*/
class PlayerView {
public:
    PlayerView(sf::Font& f, const sf::Vector2f& pos)
        : font_(f), minimal_(false), eliminated_(false), active_(false)
    {
        box_.setSize({180.f, 80.f});
        box_.setPosition(pos);
        box_.setOutlineThickness(1);
        box_.setOutlineColor(sf::Color::Black);
        box_.setFillColor(sf::Color(40, 120, 180));

        name_.setFont(font_);
        name_.setCharacterSize(18);
        name_.setPosition(pos.x + 8.f, pos.y + 3.f);

        role_.setFont(font_);
        role_.setCharacterSize(14);
        role_.setFillColor(sf::Color(230, 230, 230));
        role_.setPosition(pos.x + 8.f, pos.y + 28.f);

        coins_.setFont(font_);
        coins_.setCharacterSize(15);
        coins_.setPosition(pos.x + 8.f, pos.y + 54.f);

        skull_.setFont(font_);
        skull_.setCharacterSize(28);
        skull_.setString("☠");
        skull_.setPosition(pos.x + 145.f, pos.y + 26.f);
    }

    /*-------------- setters ---------------*/
    void setName (const std::string& n) { name_.setString(n);        }
    void setRole (const std::string& r) { role_.setString(r);        }
    void setCoins(int c)                { coins_.setString("Coins: " + std::to_string(c)); }

    void setMinimal(bool m)             { minimal_ = m; }
    void setEliminated(bool e)          { eliminated_ = e;  refreshColours(); }
    void setActive(bool a)              { active_     = a;  refreshColours(); }

    /*-------------- accessors -------------*/
    std::string getName() const { return name_.getString().toAnsiString(); }

    /*-------------- drawing ---------------*/
    void draw(sf::RenderWindow& w) const {
        w.draw(box_);
        w.draw(name_);
        if (!minimal_) { w.draw(role_); w.draw(coins_); }
        if (eliminated_) w.draw(skull_);
    }

private:
    void refreshColours() {
        if (eliminated_) {
            box_.setFillColor(sf::Color(70, 70, 70));
            name_. setFillColor(sf::Color(140, 140, 140));
            role_. setFillColor(sf::Color(120, 120, 120));
            coins_.setFillColor(sf::Color(140, 140, 140));
            return;
        }
        box_.setFillColor(active_ ? sf::Color(60, 180, 60) : sf::Color(40, 120, 180));
        name_. setFillColor(sf::Color::White);
        role_. setFillColor(sf::Color(230, 230, 230));
        coins_.setFillColor(sf::Color::White);
    }

    sf::Font&           font_;
    sf::RectangleShape  box_;
    sf::Text            name_, role_, coins_, skull_;
    bool                minimal_;
    bool                eliminated_;
    bool                active_;
};
