#include "GamakichiBomb.h"

Texture GamakichiBomb::sharedTexture;
bool GamakichiBomb::textureLoaded = false;
bool GamakichiBomb::showHitbox = false;  

GamakichiBomb::GamakichiBomb(float x, float y, float wW, float wH) : Enemy(x, y, wW, wH, 80.f, 80.f)
{
    velX = (rand() % 2 == 0) ? -180.f : 180.f;
    velY = -400.f;
    gravity = 1200.f;
    alive = true;
    loadSprite();
    setupFrames();
}

void GamakichiBomb::preloadTexture() 
{
    if (!textureLoaded) 
    {
        sharedTexture.loadFromFile("SnowBrosAssets/Images/Gamakichi.png");
        textureLoaded = true;
    }
}

void GamakichiBomb::setColor(int color) {}

void GamakichiBomb::loadSprite() {
    if (!textureLoaded) {
        sharedTexture.loadFromFile("SnowBrosAssets/Images/Gamakichi.png");
        textureLoaded = true;
    }
    sprite = new Sprite(sharedTexture);
}

void GamakichiBomb::die() { alive = false; }

void GamakichiBomb::setupFrames()
{
    IntRect bombFrame = IntRect({ 1674, 1511 }, { 201, 206 });
    sprite->setTextureRect(bombFrame);
    float sx = hitbox.getWidth() / (float)bombFrame.size.x;
    float sy = hitbox.getHeight() / (float)bombFrame.size.y;
    sprite->setScale({ sx, sy });
}

void GamakichiBomb::update(float dt, Platform* platforms, int platformCount)
{
    if (!alive)
    {
        return;
    }
    velY += gravity * dt;
    enemyX += velX * dt;
    enemyY += velY * dt;
    hitbox.update(enemyX, enemyY);

    if (enemyY + hitbox.getHeight() > windowH - 70)
    {
        enemyY = windowH - 70 - hitbox.getHeight();
        velY *= -0.4f;  // multiplying by -0.4 flips the direction (bounce upward)
        if (fabs(velY) < 30.f)  // once the bounce speed drops below 30 pixels/sec, the bomb has basically settled
        {
            alive = false; 
            return;  // dies when it settles
        }
    }

    // if the bomb touches either side wall (within 80 pixels of either edge), kill it immediately
    if (enemyX < 80.f || enemyX + hitbox.getWidth() > windowW - 80.f) 
    {
        alive = false; 
        return;
    }

    sprite->setPosition({ enemyX, enemyY });
}
void GamakichiBomb::draw(RenderWindow& window)
{
    if (!alive)
    {
        return;
    }
    window.draw(*sprite);
    if (showHitbox)
    {
        hitbox.draw(window, Color::Red);
    }
}