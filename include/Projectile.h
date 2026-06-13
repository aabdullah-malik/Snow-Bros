#pragma once
#include <SFML/Graphics.hpp>
#include "Platform.h"
#include "HitBox.h"
using namespace sf;

class Projectile
{
protected:
    float x, y;
    float velX, velY;
    float gravity;
    float width, height;
    float windowW, windowH;
    bool active;
    HitBox hitbox;

public:
    Projectile(float startX, float startY, float vX, float vY, float wW, float wH);

    virtual void update(float dt, Platform* platforms, int platformCount) = 0;
    virtual void draw(RenderWindow& window) = 0;

    FloatRect getRect();
    bool isActive();
    void deactivate();

    void drawDebug(RenderWindow& window);

    virtual ~Projectile() {}
};
