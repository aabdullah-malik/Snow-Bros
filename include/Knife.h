#pragma once
#include "Projectile.h"
using namespace sf;

class Knife : public Projectile
{
private:
    Texture* texture;
    Sprite* sprite;
    IntRect frames[4];
    int currentFrame;
    float animTimer;
    float animSpeed;

public:
    Knife(float startX, float startY, float vX, float vY, float wW, float wH, Texture* tex);
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    ~Knife();
};
