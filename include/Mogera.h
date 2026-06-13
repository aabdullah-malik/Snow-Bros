#pragma once
#include "Enemy.h"
#include "MogeraChild.h"
using namespace sf;

class Mogera : public Enemy
{
protected:
    IntRect bodySitting, bodyMouthOpen, bodyRoaring;
    IntRect bodyDeathFrames[2];
    IntRect legFrames[3];
    Sprite* legsSprite; // only legsprite here as for body we used the inherited sprite

    static const int MAX_CHILDREN = 10;
    MogeraChild* children[MAX_CHILDREN];

    // for mogerachild
    float spawnTimer, spawnInterval;

    float jumpTimer;  // counts time since last jump
    float jumpInterval;  // jumps every 2 seconds
    bool isJumping;
    int jumpFrameIndex;  // which leg frame to show (0=idle, 1=going up, 2=going down)
    bool isDying;
    int deathFrameIndex;
    float deathTimer;
    float jumpPhaseTimer;   // controls how often jumpFrameIndex updates
    float deathAnimSpeed;

    int currentPlatform;  // 0 = upper, 1 = lower
    int targetPlatform;   // where jumping to

    RectangleShape healthBarBg, healthBarFill;

    bool shouldRoar;
    bool hasRoared;

    int scoreReward;

    int bodyState;  // 0=sitting, 1=mouthOpen, 2=roaring
    float bodyStateTimer;  // how long current body state has been showing
public:
    Mogera(float x, float y, float wW, float wH);

    void setColor(int color) override;
    void loadSprite() override;
    void setupFrames() override;
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    void addSnow(int amount);
    void die() override;

    void startJump();
    void updateJump(float dt);
    void spawnChildrenBurst(int count);
    void updateChildren(float dt, Platform* platforms, int platformCount);
    void drawChildren(RenderWindow& window);
    bool getShouldRoar();
    int getEnemyType() const override { return 3; }
    void onHitByRollingSnowball() override;

    int  getScoreReward()   const { return scoreReward; }
    void clearScoreReward() { scoreReward = 0; }

    int getCreatureCount() const;
    MogeraChild* getCreature(int index) const;
    ~Mogera();
};