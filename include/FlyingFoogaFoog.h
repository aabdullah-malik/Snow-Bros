#pragma once
#include "Botom.h"
using namespace sf;

// It does NOT re-declare idleFrame, dieFrames or currentAnim those come from Botom.
class FlyingFoogaFoog : public Botom
{
protected:
    // Extra animation frames which are not own by BOTOM
    IntRect flyFrames[2];
    IntRect hitFrames[3];       
    IntRect angryFrames[2];
    IntRect spinFrame;
    IntRect fffDieFrames[3];    // renamed to avoid clash with Botom::dieFrames[9]

    // Override animation state IDs for FlyingFoogaFoog
    static const int ANIM_IDLE = 0;
    static const int ANIM_FLY = 1;
    static const int ANIM_HIT = 2;
    static const int ANIM_ANGRY = 3;
    static const int ANIM_SPIN = 4;
    static const int ANIM_DIE = 5;

    // Flight system
    bool isFlying;
    float groundTimer;
    float groundInterval;
    float flightTimer;
    float flightDuration;
    float flightDirX;
    float flightDirY;
    float flightSpeed;

    // Death-spin system
    bool isSpinning;
    float spinAngle;
    float spinSpeed;
    bool spinDone;

    static const int COLOR_RED = 0;
    static const int COLOR_ORANGE = 1;
    static const int COLOR_BLUE = 2;

public:
    FlyingFoogaFoog(float x, float y, float wW, float wH);

    void loadSprite()  override;
    void setupFrames() override;
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    void setColor(int color) override;
    void updateAnimation(float dt);
    void setState(int newState);
    void die() override;
    int getEnemyType() const override { return 1; }
    static int getColorRed() { return COLOR_RED; }
    static int getColorOrange() { return COLOR_ORANGE; }
    static int getColorBlue() { return COLOR_BLUE; }

    ~FlyingFoogaFoog() {}
};