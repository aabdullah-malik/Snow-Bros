#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "Platform.h"
using namespace sf;

class Enemy
{
protected:
    float enemyX, enemyY;
    float velX, velY;
    float gravity;
    float speed;
    float speedMultiplier;
    float windowW, windowH;
    bool onGround;
    bool alive;
    bool isRolling;
    bool facingRight;
    int colorVariant;

    float rollSizeMultiplier;  // grows as rolling snowball kills enemies

    // health - mainly for bosses
    int health;
    int maxHealth;

    // snow coat 
    bool addingSnow;
    bool removingSnow;
    static const int MAX_SNOW_FRAMES = 5;
    IntRect addSnowFrames[MAX_SNOW_FRAMES];
    IntRect removeSnowFrames[MAX_SNOW_FRAMES];

    int addSnowFrameCount;  // enemy's own frames while being trapped
    int removeSnowFrameCount;  // enemy's own frames while getting free
    int snowAnimFrame;
    float snowAnimTimer;
    float snowAnimSpeed;

    HitBox hitbox;
    Texture texture;
    Sprite* sprite;

    int currentFrame;
    float animTimer;
    float animSpeed;

    int snowLevel;
    int snowMaxLevel;
    bool snowCoated;
    bool snowRolling;
    float snowRollVelX;
    float snowBreakTimer;
    bool isMelting;
    int meltFrame;
    float meltAnimTimer;   // how fast melting frames are played
    int rollKillCount;

    // A short delay before rolling begins
    bool pendingRoll;
    float rollDelay;
    float rollDelayTimer;
  
    bool isDying;

    Sprite* snowSprite;
    // made static so that they load only once and prevent lagging
    static Texture snowTexture;
    static bool snowTexLoaded;
    static IntRect snowFrames[4];
    static IntRect meltFrames[4];


public:
    Enemy(float x, float y, float wW, float wH, float hbW, float hbH);

    virtual void update(float dt, Platform* platforms, int platformCount) = 0;
    virtual void draw(RenderWindow& window) = 0;
    virtual void loadSprite() = 0;
    virtual void setupFrames() = 0;
    virtual void setColor(int color) = 0;

    void drawDebug(RenderWindow& window);
    virtual void takeDamage(int amount = 1);  
    void startMelting();

    bool isAlive();
    bool getIsRolling();
    int getSnowLevel();
    int getsnowMaxLevel();
    int getHealth();
    int getMaxHealth();
    FloatRect getRect();
    HitBox& getHitbox();
    FloatRect getHitboxRect();
    float getX() const;
    float getY() const;

    virtual int getEnemyType() const = 0;

    virtual void die() = 0;

    void applyGravityAndMove(float dt, Platform* platforms, int platformCount);
    void updateSnowAnim(float dt);

    // Snow interface
    virtual void addSnow(int amount = 1);
    virtual void pushSnowball(float dirX);
    virtual void updateSnow(float dt, Platform* platforms, int platformCount);
    virtual void onHitByRollingSnowball();
    virtual void drawSnowVisuals(RenderWindow& window);


    bool isFullyCoated()      const { return snowCoated; }
    bool isRollingSnowball()  const { return snowRolling; }
    int  getRollKillCount()   const { return rollKillCount; }
    void addRollKill() { rollKillCount++; rollSizeMultiplier += 0.005f; }
    bool getIsDying();

    virtual ~Enemy();
};