#pragma once
#include "Projectile.h"
using namespace sf;

class Snowball : public Projectile
{
private:
    Texture* texture;
    Sprite* sprite;
    IntRect frames[2];
    int currentFrame;
    float animTimer;
    float animSpeed;
    bool facingRight;

public:
    Snowball(float startX, float startY, bool goingRight, float wW, float wH, Texture* sharedTexture);

    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    void deactivate();
    ~Snowball();
};
