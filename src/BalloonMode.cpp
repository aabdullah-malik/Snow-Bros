#include "powers.h"
#include "Player.h"

// =====================================================================
// CONSTRUCTOR
// IntRect({ 331, 493 }, { 135, 149 }) = green potion icon from Items.png
// Coordinates from Shop.h iconRects[4]
// =====================================================================
BalloonMode::BalloonMode(float x, float y)
    : PowerUp(x, y, IntRect({ 331, 493 }, { 135, 149 }))
{
}

// =====================================================================
// APPLYTO
// Tells player to activate balloon mode effect
// =====================================================================
void BalloonMode::applyTo(Player* p)
{
    p->activateBalloonMode();
}