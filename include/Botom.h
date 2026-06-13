#pragma once
#include "Enemy.h"
using namespace sf;

class Botom : public Enemy
{
protected:
    IntRect idleFrame;
    IntRect chargeFrames[2];
    IntRect walkFrames[3];
    IntRect jumpFrame;
    IntRect fallFrame;
    IntRect stareFrame;
    IntRect dieFrames[9];
    IntRect aaFrame;

    static const int ANIM_IDLE = 0;
    static const int ANIM_CHARGE = 1;
    static const int ANIM_WALK = 2;
    static const int ANIM_JUMP = 3;
    static const int ANIM_FALL = 4;
    static const int ANIM_DIE = 5;
    static const int ANIM_AA = 6;
    static const int ANIM_STARE = 7;

    static const int COLOR_RED = 0;  
    static const int COLOR_ORANGE = 1;
    static const int COLOR_BLUE = 2;

    int currentAnim;
    bool isDying;
    float dirChangeTimer;
    float dirChangeInterval;

public:
    Botom(float x, float y, float wW, float wH);

    void loadSprite()  override;
    void setupFrames() override;
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    void setColor(int color) override;
    void updateAnimation(float dt);
    void setState(int newState);
    void die() override;
    int getEnemyType() const override { return 0; }
    static int getColorRed() { return COLOR_RED; }
    static int getColorOrange() { return COLOR_ORANGE; }
    static int getColorBlue() { return COLOR_BLUE; }
    ~Botom() {}
};