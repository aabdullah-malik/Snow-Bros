#pragma once
#include "Enemy.h"
#include "GamakichiCreature.h"
#include "GamakichiBomb.h"
using namespace sf;

class Gamakichi : public Enemy
{
protected:
    static const int STATE_ENTERING = 0;  // rising from below to bottom position
    static const int STATE_AT_BOTTOM = 1;  // sitting at bottom, waiting
    static const int STATE_RISING = 2;  // moving up to top position  
    static const int STATE_AT_TOP = 3;  // sitting at top, waiting
    static const int STATE_DESCENDING = 4;  // moving back down
    static const int STATE_ATTACKING = 5;  // attacking (can happen at either position)
    static const int STATE_DYING = 6;

    // Fire - 4 frame animation
    IntRect fireFrames[4];
    Sprite* fireSprites[4];
    int fireFrame;
    float fireAnimTimer;
    int firePosFrame;

    float positionTop;
    float positionBottom;
    float moveSpeed;  // how fast it moves between positions
    float dwellTime;  // how long it stays at each position

    // Body
    IntRect idleFrame;
    IntRect attackFrame;

    // Arms
    static const int MAX_ARMS = 6;
    Vector2f armOffsets[MAX_ARMS];  // it holds two float values - an x and a y
    IntRect lightningFrames[2];
    Sprite* lightningSprites[MAX_ARMS];
    bool armActive[MAX_ARMS];
    float lightningTimer;

    // State
    int state;
    float stateTimer;

    bool shouldRoar;
    bool hasRoared;

    float damageCooldown;
    float damageCooldownMax; 

    // Projectiles
    static const int MAX_CREATURES = 12;
    GamakichiCreature* creatures[MAX_CREATURES];
    static const int MAX_BOMBS = 3;
    GamakichiBomb* bombs[MAX_BOMBS];

    // Set to 10000 when health reaches 0 -- polled and cleared by SnowBros
    int scoreReward;

    RectangleShape healthBarBg, healthBarFill;
public:
    Gamakichi(float wW, float wH);

    void setColor(int color) override;
    void loadSprite() override;
    void setupFrames() override;
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    void takeDamage(int amount) override;

    void startAttack();
    void spawnCreatureFromArm(int armIndex);
    void spawnBomb();
    void updateProjectiles(float dt, Platform* platforms, int platformCount);
    void drawProjectiles(RenderWindow& window);
    void addSnow(int amount = 2) override;
    void die() override;
    void onHitByRollingSnowball() override;

    int getEnemyType() const override { return 4; }
    int getCreatureCount() const;
    bool getShouldRoar();
    GamakichiBomb* getBomb(int index) const;

    // scoreReward is set to 10000 the moment Gamakichi's health hits 0.
    // SnowBros polls getScoreReward() each frame and adds it to score,
    // then calls clearScoreReward() so it is only counted once.
    int  getScoreReward()   const { return scoreReward; }
    void clearScoreReward() { scoreReward = 0; }

    GamakichiCreature* getCreature(int index) const;

    ~Gamakichi();
};