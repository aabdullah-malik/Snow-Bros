#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class HitBox
{
private:
    float x, y;
    float width, height;

public:
    HitBox(float w, float h);
    void update(float x, float y);
    FloatRect getRect();
    float getWidth();
    float getHeight();
    void draw(RenderWindow& window, Color color);
};
