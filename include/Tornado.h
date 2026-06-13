#pragma once
#include "FlyingFoogaFoog.h"
#include "Knife.h"
using namespace sf;

class Tornado : public FlyingFoogaFoog
{
protected:
    // Own animation frames (some are already inherited from botom and fff)
    IntRect runFrames[3];
    IntRect landFrame;
    IntRect turnFrames[2];
    IntRect throwFrames[2];
    IntRect tornadoSpinFrames[8];   // renamed to avoid clash 
    IntRect tornadoDieFrames[9];    // renamed to avoid clash 

    static const int ANIM_IDLE = 0;
    static const int ANIM_RUN = 1;
    static const int ANIM_JUMP = 2;
    static const int ANIM_FALL = 3;
    static const int ANIM_LAND = 4;
    static const int ANIM_TURN = 5;
    static const int ANIM_THROW = 6;
    static const int ANIM_SPIN = 7;
    static const int ANIM_DIE = 8;
    static const int ANIM_HIT = 9;
    static const int ANIM_FLY = 10;

    // Knife throwing
    float knifeTimer;
    float knifeInterval;
    Knife* currentKnife; // single reusable knife, null when not yet thrown
    Texture knifeTexture;

    // Spin state
    bool isTornadoSpinning;
    bool tornadoSpinDone;
    bool isDying;
    int spinFrameIndex;
    float spinTimer;
    float spinFrameDelay;
    float spinCycleTimer;

    static const int COLOR_RED = 0;
    static const int COLOR_BLUE = 1;

public:
    Tornado(float x, float y, float wW, float wH);

    void loadSprite()  override;
    void setupFrames() override;
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    void setColor(int color) override;

    void updateAnimation(float dt);
    void setState(int newState);
    void throwKnife();
    void updateKnife(float dt, Platform* platforms, int platformCount);
    void die() override;
    int getEnemyType() const override { return 2; }

    static int getColorRed() { return COLOR_RED; }
    static int getColorBlue() { return COLOR_BLUE; }

    Knife* getKnife() const { return currentKnife; }

    ~Tornado();
};