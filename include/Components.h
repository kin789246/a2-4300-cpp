#pragma once

#include "SFML/Graphics/RectangleShape.hpp"
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <string>

class CTransform
{
    public:
        Vec2 pos = { 0.0, 0.0 };
        Vec2 velocity = { 0.0, 0.0 };
        float angle = 0;

        CTransform(const Vec2& p, const Vec2& v, float a)
            : pos(p), velocity(v), angle(a) {}
};

class CShape
{
    public:
        sf::CircleShape circle;

        CShape(
            float radius, int points, 
            const sf::Color& fill, const sf::Color& outline,
            float thickness
        ) : circle(radius, points) {
            circle.setFillColor(fill);
            circle.setOutlineColor(outline);
            circle.setOutlineThickness(thickness);
            circle.setOrigin(radius, radius);
        }
};

class CCollision
{
    public:
        float radius = 0;

        CCollision(float r) : radius(r) {}
};

class CScore
{
    public:
        int score = 0;

        CScore(int s) : score(s) {}
};

class CLifespan
{
    public:
        int remaining = 0;
        int total = 0;

        CLifespan(int total) : remaining(total), total(total) {}
};

class CInput
{
    public:
        bool up = false;
        bool left = false;
        bool right = false;
        bool down = false;
        bool shoot = false;

        CInput() {}
};

class CText
{
    public:
        sf::Text text;

        CText(std::string str, sf::Font& f, int s) {
            text = sf::Text(str, f, s);
        }
};

class CCdMeter {
    public:
        float barLen;
        float remainTime;
        Vec2 offset;
        sf::Clock clock;
        sf::RectangleShape rectangle;

        CCdMeter(float w, float h, Vec2 o, float s)
        : barLen(w), remainTime(s), offset(o) {
            rectangle = sf::RectangleShape(sf::Vector2(w, h));
            rectangle.setFillColor(sf::Color(255, 255, 200, 220));
            clock.restart();
        }
};
