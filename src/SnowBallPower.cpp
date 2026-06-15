#include "Powers.h"
#include "Player.h"


// CONSTRUCTOR
// IntRect({ 341, 179 }, { 119, 147 }) = blue potion icon from Items.png
// Coordinates from Shop.h iconRects[2]
SnowballPower::SnowballPower(float x, float y)
    : PowerUp(x, y, IntRect({ 341, 179 }, { 119, 147 }))
{
}

// APPLYTO
// Tells player to activate snowball power effect
void SnowballPower::applyTo(Player* p)
{
    p->activateSnowballPower();
}