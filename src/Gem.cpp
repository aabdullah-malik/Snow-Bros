#include "Gem.h"
#include <cmath>

// Static members - defined once here, shared by all Gem objects
Texture Gem::sharedTexture;
bool    Gem::textureLoaded = false;

// CONSTRUCTOR
// x, y      = where the gem spawns (where the enemy died)
// value     = how many gems this pickup is worth (1-5 for normal)
// hasTimer  = true for bonus level gems (disappear after 30 seconds)
Gem::Gem(float spawnX, float spawnY, int val, bool timed)
{
    x = spawnX;
    y = spawnY;
    value = val;
    hasTimer = timed;
    collected = false;
    lifeTimer = 0.f;
    bobTimer = 0.f;
    displayY = y;

    // Load texture from disk only once  all gems share same texture
    if (!textureLoaded)
    {
        sharedTexture.loadFromFile("SnowBrosAssets/Backgrounds/Gems.png");
        textureLoaded = true;
    }

    sprite = new Sprite(sharedTexture);

    // Gems.png is a single 360x360 pink diamond using the whole image
    sprite->setTextureRect(IntRect({ 0, 0 }, { 360, 360 }));

    // Scale down to 40x40 pixels on screen
    float scaleX = 40.f / 360.f;
    float scaleY = 40.f / 360.f;
    sprite->setScale({ scaleX, scaleY });

    sprite->setPosition({ x, y });
}

// DESTRUCTOR
Gem::~Gem()
{
    delete sprite;
}

// UPDATE
// Called every frame from SnowBros::updateGems()
// Handles the bobbing animation and the 30 second timer for bonus gems

void Gem::update(float dt)
{
    if (!isActive()) return;

    // Tick the life timer (only matters for bonus gems)
    lifeTimer += dt;

    // Bob up and down smoothly using sine wave
    // sinf returns a value between -1 and +1
    // Multiply by 5 to give 5 pixel bob range
    bobTimer += dt;
    displayY = y + sinf(bobTimer * 3.0f) * 5.0f;

    // Update sprite position with bobbing applied
    sprite->setPosition({ x, displayY });
}

// DRAW
// Called every frame from SnowBros::drawGems()
void Gem::draw(RenderWindow& window)
{
    if (!isActive()) return;

    // Make gem flash in last 5 seconds if it has a timer (bonus gem)
    if (hasTimer && lifeTimer >= 25.f)
    {
        // Flash by only drawing every other half-second
        int flash = (int)(lifeTimer * 6.f) % 2;
        if (flash == 0) return;  // skip drawing = invisible frame = flash effect
    }

    window.draw(*sprite);
}

// GETRECT
// Returns the 40x40 collision box at current position
// Used by SnowBros::checkGemCollection() to detect player overlap
FloatRect Gem::getRect() const
{
    return FloatRect({ x, displayY }, { 40.f, 40.f });
}

// ISACTIVE
// Returns true if gem should still exist on screen
// False if: collected by player, OR bonus gem that ran out of time
bool Gem::isActive() const
{
    if (collected) return false;
    if (hasTimer && lifeTimer >= 30.f) return false;
    return true;
}

// GETVALUE
// Returns how many gems this pickup is worth
int Gem::getValue() const
{
    return value;
}

// COLLECT
// Called by SnowBros when player walks over the gem
void Gem::collect()
{
    collected = true;
}



// CONSTRUCTOR
// x, y   = starting position (y will be negative - above screen)
// wW, wH = window dimensions needed to know where the floor is
//
// We pass value=10 and hasTimer=true to the base Gem constructor
// value=10  means collecting this gives 10 gems to the player
// hasTimer=true means it uses lifeTimer (we override isActive in Gem
//          but here we use the 20 second window from bonus rain)
FallingGem::FallingGem(float spawnX, float spawnY, float wW, float wH)
    : Gem(spawnX, spawnY, 10, true)
{
    velY = 150.f;           // starts falling at 150 pixels per second
    gravity = 400.f;           // accelerates downward
    floorY = wH - 70.f;      // floor level - same as player/enemy floor
}

// UPDATE
// Overrides Gem::update()
// Step 1: tick life timer (inherited from Gem via lifeTimer)
// Step 2: apply gravity to increase falling speed
// Step 3: move gem downward
// Step 4: if gem reaches floor, stop it there so player can collect
// Step 5: update sprite position
void FallingGem::update(float dt)
{
    if (!isActive()) return;

    // Tick life timer - gem disappears when bonusTimer >= 20 externally
    // lifeTimer is used by isActive() in base class
    lifeTimer += dt;

    // Apply gravity - gem falls faster over time
    velY += gravity * dt;

    // Move gem downward
    y += velY * dt;

    // Stop at floor - gem sits on floor waiting for player
    if (y + 40.f > floorY)
    {
        y = floorY - 40.f;
        velY = 0.f;
    }

    // Update display position - no bobbing for falling gems
    displayY = y;

    // Update sprite position
    sprite->setPosition({ x, displayY });
}

// DRAW
// Overrides Gem::draw()
// No flashing effect - falling gems are always fully visible

void FallingGem::draw(RenderWindow& window)
{
    if (!isActive()) return;
    window.draw(*sprite);
}