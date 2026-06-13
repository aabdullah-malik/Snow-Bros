#pragma once
#include "Enemy.h"

class GamakichiCreature : public Enemy
{
protected:
    IntRect idleFrame;
    IntRect walkFrames[3];
    IntRect jumpFrames[2];
    IntRect deathFrames[5];
    IntRect bombDeathFrames[5];
    int animFrame;  // tracks which frame of the current animation we're on
    float animTimer;  // counts time between frame switches
    bool isDying;
    bool isBombDeath;
    // facingRight inherited from Enemy base class
    float spriteScaleX;
    float spriteScaleY;
    static Texture sharedTexture;
    static bool textureLoaded;
    static bool showHitbox;
    float jumpTimer;
    float jumpInterval;  // how many seconds between jumps 

public:
    GamakichiCreature(float x, float y, float wW, float wH, bool goLeft);

    static void preloadTexture();
    void setColor(int color) override;
    void loadSprite() override;
    void setupFrames() override;
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    void die() override;
    int getEnemyType() const override { return 6; }
    void dieByBomb();
    FloatRect getHitboxRect() { return hitbox.getRect(); }
    static void toggleHitbox() { showHitbox = !showHitbox; }
    void updateSpritePos();
    static void setShowHitbox(bool val) { showHitbox = val; }

    ~GamakichiCreature() {}
};