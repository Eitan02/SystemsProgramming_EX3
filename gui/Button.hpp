// eitan.derdiger@gmail.com
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    Button() : enabled_(false), hovered_(false), initialEnabled_(false) {}

    Button(sf::Font& font,
           const std::string& label,
           unsigned int size,
           const sf::Vector2f& pos,
           const sf::Vector2f& dims)
        : enabled_(true),
          hovered_(false),
          initialEnabled_(true)
    {
        box_.setSize(dims);
        box_.setPosition(pos);
        box_.setFillColor(sf::Color(70, 70, 70));
        box_.setOutlineThickness(2);
        box_.setOutlineColor(sf::Color::Black);

        text_.setFont(font);
        text_.setString(label);
        text_.setCharacterSize(size);
        text_.setFillColor(sf::Color::White);

        auto b = text_.getLocalBounds();
        text_.setOrigin(b.width / 2.f, b.height / 2.f);
        text_.setPosition(pos.x + dims.x / 2.f, pos.y + dims.y / 2.f - 3.f);
    }

    void draw(sf::RenderWindow& w) const { w.draw(box_); w.draw(text_); }

    bool handlePressed(const sf::Event& e, const sf::RenderWindow& w) {
        if (!enabled_) return false;

        auto m        = sf::Mouse::getPosition(w);
        hovered_      = box_.getGlobalBounds().contains((float)m.x, (float)m.y);
        auto bgColour = hovered_ ? sf::Color(90, 90, 90) : sf::Color(70, 70, 70);
        box_.setFillColor(bgColour);

        return hovered_ &&
               e.type == sf::Event::MouseButtonPressed &&
               e.mouseButton.button == sf::Mouse::Left;
    }

    void setEnabled(bool e) {
        enabled_ = e;
        text_.setFillColor(e ? sf::Color::White : sf::Color(160, 160, 160));
        box_.setFillColor(e ? sf::Color(70, 70, 70) : sf::Color(50, 50, 50));
    }
    bool  isEnabled()          const { return enabled_;            }
    bool  wasEnabledInitially()const { return initialEnabled_;     }

    void setText(const std::string& s) {
        text_.setString(s);
        auto b = text_.getLocalBounds();
        text_.setOrigin(b.width / 2.f, b.height / 2.f);
    }
    std::string getText() const { return text_.getString().toAnsiString(); }

private:
    sf::RectangleShape box_;
    sf::Text           text_;
    bool               enabled_;
    bool               hovered_;
    bool               initialEnabled_;
};
