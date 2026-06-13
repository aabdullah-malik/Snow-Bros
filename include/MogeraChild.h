#pragma once
#include "Enemy.h"

class MogeraChild : public Enemy
{
protected:
    IntRect walkFrames[3];
    IntRect airFrames[4];
    int animFrame;  // current frame index
    float childAnimTimer;  // time since last frame
    float childAnimSpeed;

    // they are all made static to prevent lagging as more than one child is made at once
    static bool textureLoaded;
    static Texture childTexture;
    static bool showHitbox;

    float jumpTimer;  // counts time since last jump
    float jumpInterval;  // randomly 1.5 to 2.5 seconds between jumps

public:
    MogeraChild(float x, float y, float wW, float wH, float vX, float vY);

    static void preloadTexture();
    void setColor(int color) override;
    void loadSprite() override;
    void setupFrames() override;
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    static void toggleHitbox() { showHitbox = !showHitbox; }
    void die() override;
    int getEnemyType() const override { return 5; }
    void drawDebug(RenderWindow& window);
    static void setShowHitbox(bool val) { showHitbox = val; }

    ~MogeraChild() {}
};