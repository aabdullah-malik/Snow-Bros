#include "SnowBallLauncher.h"
#include "SnowBall.h"

SnowballLauncher::SnowballLauncher(float wW, float wH, Texture* tex)
{
    windowW = wW;
    windowH = wH;
    texture = tex;
    isThrowing = false;
    throwTimer = 0.f;
    throwDuration = 0.5f;

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        projectiles[i] = nullptr;
    }
}

void SnowballLauncher::fire(float x, float y, bool facingRight)
{
    if (isThrowing)
    {
        return;
    }
    isThrowing = true;
    throwTimer = 0.f;

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectiles[i] == nullptr || !projectiles[i]->isActive())  // if there is no snowball
        {
            delete projectiles[i];
            projectiles[i] = new Snowball(x, y, facingRight, windowW, windowH, texture);
            return;
        }
    }
}

void SnowballLauncher::update(float dt, Platform* platforms, int platformCount)
{
    if (isThrowing)
    {
        throwTimer += dt;
        if (throwTimer >= throwDuration) // throws for a specific time
        {
            isThrowing = false;
            throwTimer = 0.f;
        }

    }

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectiles[i] != nullptr && projectiles[i]->isActive())
        {
            projectiles[i]->update(dt, platforms, platformCount);  // throwing snowballs now
        }
    }
}

void SnowballLauncher::draw(RenderWindow& window)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectiles[i] != nullptr && projectiles[i]->isActive())
        {
            projectiles[i]->draw(window);
        }
    }
}

Snowball* SnowballLauncher::getProjectile(int index) const
{
    return (index >= 0 && index < MAX_PROJECTILES) ? projectiles[index] : nullptr;
}

const int SnowballLauncher::getMaxProjectiles() 
{
    return MAX_PROJECTILES; 
}

bool SnowballLauncher::getIsThrowing() 
{
    return isThrowing; 
}

SnowballLauncher::~SnowballLauncher()
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        delete projectiles[i];
    }
}