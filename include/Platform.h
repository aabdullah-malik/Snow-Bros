#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
using namespace sf;

class Platform
{
private:
    float platformX, platformY, platformW, platformH;
    Color baseColor;
    Color waveColor;
    HitBox hitbox;

public:
    Platform();
    Platform(float x, float y, float w, float h, Color base);
    void draw(RenderWindow& window);
    FloatRect getRect();  // used for getting dimensions of platform
    void drawDebug(RenderWindow& window);
    ~Platform() {}
};
