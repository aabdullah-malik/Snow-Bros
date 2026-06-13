#include "SnowBall.h"
#include <cmath>

// pointer used here to prevent the game from lagging as every time a new copy of texture would be generated which would slow down the game
Snowball::Snowball(float startX, float startY, bool goingRight, float wW, float wH, Texture* sharedTexture)
         : Projectile(goingRight ? startX + 40 : startX - 40, startY + 30, goingRight ? 500.f : -500.f, -150.f, wW, wH)
{
    facingRight = goingRight;
    width = 40;
    height = 40;
    currentFrame = 0;
    animTimer = 0.f;
    animSpeed = 0.12f;   // how long shall one frame stay on screen
    texture = sharedTexture;
    sprite = new Sprite(*texture);

    frames[0] = IntRect({ 338, 823 }, { 90, 127 });
    frames[1] = IntRect({ 495, 823 }, { 82, 127 });

    sprite->setTextureRect(frames[0]);
    sprite->setScale({ width / frames[0].size.x, height / frames[0].size.y });    // converts it into a fix size
    sprite->setPosition({ x, y });
}

void Snowball::update(float dt, Platform* platforms, int platformCount)
{
    if (!active)
    {
        return;
    }

    // velocity increases over time due to gravity
    velY += gravity * dt;
    x += velX * dt;
    y += velY * dt;

    animTimer += dt;   // has control of time i.e after how much time the frame is to be changed
    if (animTimer >= animSpeed)
    {
        animTimer = 0.f;
        currentFrame = (currentFrame + 1) % 2;
        sprite->setTextureRect(frames[currentFrame]);
        sprite->setScale({ width / frames[0].size.x, height / frames[0].size.y }); // always scaled to the same size
    }

    if (x < 80 || x + width > windowW - 80)   // touches a wall
    {
        active = false; 
        return;
    }
    if (y + height > windowH - 70) // falls to the floor
    {
        active = false; 
        return;
    }

    for (int i = 0; i < platformCount; i++)
    {
        FloatRect p = platforms[i].getRect();  // coordinates of platforms
        FloatRect s = getRect();  // coordinates of snowball
        if (s.position.x + s.size.x > p.position.x &&   // snowball’s right side is to the right of platform’s left side
            s.position.x < p.position.x + p.size.x &&   // snowball’s left side is to the left of platform’s right side
            s.position.y + s.size.y >= p.position.y &&  // snowball’s bottom is below (or touching) platform’s top
            s.position.y < p.position.y + p.size.y)     // snowball’s top is above platform’s bottom
        {
            active = false; 
            return;
        }
    }

    sprite->setPosition({ x, y });
    hitbox.update(x, y);
}

void Snowball::draw(RenderWindow& window)
{
    if (!active) return;
    window.draw(*sprite);
}

void Snowball::deactivate() 
{
    active = false; 
}

Snowball::~Snowball() 
{
    delete sprite; 
}