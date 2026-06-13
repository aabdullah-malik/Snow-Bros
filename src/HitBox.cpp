#include "HitBox.h"

HitBox::HitBox(float w, float h)
{
    width = w;
    height = h;
    x = 0;
    y = 0;
}

void HitBox::update(float newX, float newY)
{
    x = newX;
    y = newY;
}

FloatRect HitBox::getRect()
{
    return FloatRect({ x, y }, { width, height });
}

float HitBox::getWidth()
{
    return width;
}

float HitBox::getHeight()
{
    return height;
}

void HitBox::draw(RenderWindow& window, Color color)
{
    RectangleShape box;
    box.setSize({ width, height });
    box.setFillColor(Color::Transparent);
    box.setOutlineColor(color);
    box.setOutlineThickness(2);
    box.setPosition({ x, y });
    window.draw(box);
}
