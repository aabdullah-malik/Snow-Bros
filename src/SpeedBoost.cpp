#include "Powers.h"
#include "Player.h"

// CONSTRUCTOR Passes the correct sprite rect to the base PowerUp constructor
// These coordinates come directly from Shop.h iconRects[1]

SpeedBoost::SpeedBoost(float x, float y)
    : PowerUp(x, y, IntRect({ 345, 25 }, { 109, 143 }))
{
    // Base class constructor handles everything else so no extra initialization needed for this subclass
}

// Called by SnowBros::checkPowerUpCollection() when player collects this
// Tells the player to activate its speed boost effect

void SpeedBoost::applyTo(Player* p)
{
    p->activateSpeedBoost();
}