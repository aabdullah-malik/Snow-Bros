#include "Projectile.h"

Projectile::Projectile(float startX, float startY, float vX, float vY, float wW, float wH) : hitbox(40.f, 40.f)
{
    x = startX;
    y = startY;
    velX = vX;  
    velY = vY;
    windowW = wW;  
    windowH = wH;
    active = true;
    gravity = 600.f;
    width = 40.f;
    height = 40.f;
}

FloatRect Projectile::getRect()
{
    return hitbox.getRect();
}

bool Projectile::isActive() 
{
    return active;
}

void Projectile::drawDebug(RenderWindow& window)
{
    hitbox.draw(window, Color::Yellow);
}

void Projectile::deactivate()
{
    active = false; 
}
