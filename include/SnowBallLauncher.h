#pragma once
#include "Projectile.h"
#include "SnowBall.h"

class SnowballLauncher
{
private:
    static const int MAX_PROJECTILES = 5;  // Max number of active snowballs in memory at one time
    Snowball* projectiles[MAX_PROJECTILES];

    bool isThrowing;
    float throwTimer;
    float throwDuration;
    float windowW, windowH;
    Texture* texture;

public:
    SnowballLauncher(float wW, float wH, Texture* tex);

    void fire(float x, float y, bool facingRight);
    void update(float dt, Platform* platforms, int platformCount);
    void draw(RenderWindow& window);

    Snowball* getProjectile(int index) const;
    static const int getMaxProjectiles();
    bool getIsThrowing();

    ~SnowballLauncher();
};