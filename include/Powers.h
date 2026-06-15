#pragma once
#include "PowerUp.h"


// SpeedBoost - concrete PowerUp subclass
// Effect: increases player movement speed by 50% for 15 seconds

class SpeedBoost : public PowerUp
{
public:
    SpeedBoost(float x, float y);

    // Overrides pure virtual from PowerUp
    // Calls player->activateSpeedBoost()
    void applyTo(Player* p) override;
};


// SnowballPower - concrete PowerUp subclass


class SnowballPower : public PowerUp
{
public:
    SnowballPower(float x, float y);

    // Overrides pure virtual from PowerUp
    // Calls player->activateSnowballPower()
    void applyTo(Player* p) override;
};


// DistanceIncrease - concrete PowerUp subclass
// 
class DistanceIncrease : public PowerUp
{
public:
    DistanceIncrease(float x, float y);

    // Overrides pure virtual from PowerUp
    // Calls player->activateDistanceIncrease()
    void applyTo(Player* p) override;
};

// BalloonMode - concrete PowerUp subclass

class BalloonMode : public PowerUp
{
public:
    BalloonMode(float x, float y);

    // Overrides pure virtual from PowerUp
    // Calls player->activateBalloonMode()
    void applyTo(Player* p) override;
};