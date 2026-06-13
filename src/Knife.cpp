#include "Knife.h"

Knife::Knife(float startX, float startY, float vX, float vY, float wW, float wH, Texture* tex)
      : Projectile(startX, startY, vX, vY, wW, wH)
{
    gravity = 0;  // as it moves completely straight
    width = 30;
    height = 15;
    texture = tex;
    sprite = new Sprite(*texture);
    currentFrame = 0;
    animTimer = 0.f;
    animSpeed = 0.08f;

    frames[0] = IntRect({ 280, 888 }, { 70, 32 });
    frames[1] = IntRect({ 351, 888 }, { 70, 32 });
    frames[2] = IntRect({ 422, 888 }, { 70, 32 });
    frames[3] = IntRect({ 493, 888 }, { 70, 32 });

    sprite->setTextureRect(frames[0]);
    sprite->setScale({ width / frames[0].size.x, height / frames[0].size.y });
    sprite->setPosition({ x, y });
}

void Knife::update(float dt, Platform* platforms, int platformCount)
{
    if (!active)
    {
        return;
    }

    x += velX * dt;
    y += velY * dt;

    animTimer += dt;
    if (animTimer >= animSpeed)
    {
        animTimer = 0.f;
        currentFrame = (currentFrame + 1) % 4;
        sprite->setTextureRect(frames[currentFrame]);
        sprite->setScale({ width / frames[currentFrame].size.x,
                           height / frames[currentFrame].size.y });
    }

    if (x < 80 || x + width > windowW - 80)  // hits a wall
    {
        active = false;
        return;
    }

    sprite->setPosition({ x, y });
    hitbox.update(x, y);
}

void Knife::draw(RenderWindow& window)
{
    if (!active)
    {
        return;
    }
    window.draw(*sprite);
}

Knife::~Knife() 
{
    delete sprite; 
}
