#include "PowerUp.h"
#include <cmath>

// Static members - defined once here, shared by all subclass objects

Texture PowerUp::sharedTexture;
bool    PowerUp::textureLoaded = false;

// CONSTRUCTOR Called by each subclass constructor with its specific sprite rect
// x, y        = where the power-up spawns (enemy death position)
// spriteRect  = which frame from Items.png to show for this type

PowerUp::PowerUp(float spawnX, float spawnY, IntRect spriteRect)
{
    x = spawnX;
    y = spawnY;
    displayY = spawnY;
    collected = false;
    lifeTimer = 0.f;
    bobTimer = 0.f;

    // Load Items  only once the PowerUp is created
    // After that all subclasses reuse the same texture in memory
    if (!textureLoaded)
    {
        sharedTexture.loadFromFile("SnowBrosAssets/Images/Items.png");
        textureLoaded = true;
    }

    sprite = new Sprite(sharedTexture);

    // Cut out the correct icon for this specific power-up type
    sprite->setTextureRect(spriteRect);

    // Scale the icon down to 45x45 pixels on screen
    float scaleX = 45.f / spriteRect.size.x;
    float scaleY = 45.f / spriteRect.size.y;
    sprite->setScale({ scaleX, scaleY });
    sprite->setPosition({ x, y });
}

// DESTRUCTOR

PowerUp::~PowerUp()
{
    delete sprite;
}

// UPDATE
// Called every frame from SnowBros::updatePowerUps()
// Step 1: tick the life timer upward
// Step 2: calculate bobbing position using sine wave
// Step 3: update sprite position to the new bobbing y

void PowerUp::update(float dt)
{
    if (!isActive()) return;

    // Tick life timer - item disappears when this reaches 15
    lifeTimer += dt;

    // Smooth bobbing: sinf returns -1 to +1, multiply by 5 = 5px range
    bobTimer += dt;
    displayY = y + sinf(bobTimer * 3.0f) * 5.0f;

    sprite->setPosition({ x, displayY });
}

// DRAW
// Called every frame from SnowBros::drawPowerUps()
// Flashes in last 5 seconds (lifeTimer 10 to 15) to warn player
// Flash works by skipping every other draw call
void PowerUp::draw(RenderWindow& window)
{
    if (!isActive()) return;

    // Flash warning in last 5 seconds before disappearing
    if (lifeTimer >= 10.f)
    {
        // (int)(lifeTimer * 6) gives a number that alternates odd/even
        // When even we skip drawing = invisible frame = flash effect
        int flash = (int)(lifeTimer * 6.f) % 2;
        if (flash == 0) return;
    }

    window.draw(*sprite);
}

// GETRECT
// Returns 45x45 collision box at current bobbing position
// Used by SnowBros::checkPowerUpCollection() to detect player overlap

FloatRect PowerUp::getRect() const
{
    return FloatRect({ x, displayY }, { 45.f, 45.f });
}

// ISACTIVE
// Returns false if player already collected it OR if 15 seconds passed
// SnowBros::updatePowerUps() uses this to delete expired power-ups
bool PowerUp::isActive() const
{
    if (collected)         return false;
    if (lifeTimer >= 15.f) return false;
    return true;
}

// COLLECT
// Called by SnowBros::checkPowerUpCollection() when player overlaps
// Sets collected = true so isActive() returns false next frame

void PowerUp::collect()
{
    collected = true;
}