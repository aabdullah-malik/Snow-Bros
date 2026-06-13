#pragma once
#include <SFML/Graphics.hpp>
#include "Platform.h"
#include "HitBox.h"
#include "StatsComponent.h"
#include "SnowBallLauncher.h"
using namespace sf;

class Player
{
protected:  // protected to be accessed by derived classes
    static const int ANIM_IDLE = 0;
    static const int ANIM_WALK = 1;
    static const int ANIM_SPRINT = 2;
    static const int ANIM_JUMP = 3;
    static const int ANIM_FALL = 4;
    static const int ANIM_KICK = 5;
    static const int ANIM_PUSH_NORMAL = 6;
    static const int ANIM_PUSH_SPRINT = 7;
    static const int ANIM_DIE = 8;
    static const int ANIM_HAPPY = 9;
    static const int ANIM_BALLOON = 10;
    static const int ANIM_THROW = 11;

    float playerX, playerY;
    float velX, velY;
    float speed, sprintSpeed;
    float baseSpeed;       // original speed saved to restore after the speed boost ends
    float gravity, jumpStrength;
    float windowW, windowH;
    bool onGround, facingRight;
    bool isSprinting;
    int playerNum;

    Texture texture;
    Sprite* sprite;
    Texture snowTexture;

    // Composed objects
    HitBox hitbox;
    StatsComponent stats;
    SnowballLauncher launcher;

    // Animation frames
    IntRect idleFrame;
    IntRect walkFrames[3];
    IntRect sprintFrames[4];
    IntRect jumpFrames[7];
    IntRect fallFrames[3];
    IntRect pushNormalFrames[3];
    IntRect pushSprintFrames[3];
    IntRect dieFrames[7];
    IntRect happyFrames[3];
    IntRect balloonFrames[6];
    IntRect throwFrames[4];

    // Invincibility
    bool  invincible;
    float invincibilityTimer;

    // Pushing the snowball
    bool isPushing;
    float pushTimer;
    bool pushKeyPressed;   // key just pressed
    bool pushKeyHeld;      // key is being hold
    int pushFrame;
    float pushAnimSpeed;
    int pushFrameCount;

    int currentFrame;
    int currentAnim;
    int prevAnim;
    float animTimer;
    float animSpeed;

    bool justJumped;

    // Power-up state 
    // Each flag + timer pair represents one active power-up effect
    // Timer counts DOWN from 15 to 0
    bool speedBoostActive;
    float speedBoostTimer;

    bool snowballPowerActive;
    float snowballPowerTimer;

    bool distanceIncreaseActive;
    float distanceIncreaseTimer;

    bool balloonModeActive;
    float balloonModeTimer;

public:
    Player(float x, float y, float wW, float hH);

    virtual void loadSprite() = 0;
    virtual void setupFrames() = 0;

    void handleInput();
    void update(float dt, Platform* platforms, int platformCount);
    void updateAnimation(float dt);
    void updatePowerUpTimers(float dt);
    void setState(int newState);
    void draw(RenderWindow& window);
    void drawDebug(RenderWindow& window);

    FloatRect getRect();
    float getX();
    float getY();
    int getLives();
    int getScore();
    bool getIsDead();
    bool getDeathDone();
    bool getFacingRight();
    SnowballLauncher& getLauncher();

    void loseLife();
    void addScore(int pts);
    void revive();
    void startPush();
    bool getPushKeyPressed();
    void clearPushKeyPressed();
    bool getIsPushing();
    bool getJustJumped();

    // allows external code (shop) to grant an extra life
    void addLife();

    // Power-up activate methods
    // Called by PowerUp subclasses through applyTo(Player*)
    void activateSpeedBoost();
    void activateSnowballPower();
    void activateDistanceIncrease();
    void activateBalloonMode();

    // Star panel versions -- last the entire level (timer = 99999)
    void activateSpeedBoostPermanent();
    void activateSnowballPowerPermanent();
    void activateDistanceIncreasePermanent();
    void activateBalloonModePermanent();

    // Called by SnowBros::advanceLevel() to clear all effects
    void clearAllPowerUps();

    // Getters used by SnowBros for HUD display and snowball collision
    bool getSnowballPowerActive() const { return snowballPowerActive; }
    bool getDistanceIncreaseActive() const { return distanceIncreaseActive; }
    bool getBalloonModeActive() const { return balloonModeActive; }
    bool getSpeedBoostActive() const { return speedBoostActive; }
    float getSpeedBoostTimer() const { return speedBoostTimer; }
    float getSnowballPowerTimer() const { return snowballPowerTimer; }
    float getDistanceIncreaseTimer() const { return distanceIncreaseTimer; }
    float getBalloonTimer() const { return balloonModeTimer; }

    virtual ~Player();
};
