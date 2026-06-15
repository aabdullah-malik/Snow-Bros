#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;


// Gem - a collectible that appears when an enemy is killed via chain roll
// Normal gems: stay on screen forever until collected
// Bonus gems:  disappear after 30 seconds if not collected

class Gem
{
protected:
    float       x, y;          
    int         value;         // how many gems this pickup is worth
    bool        collected;     // true = player picked it up
    bool        hasTimer;      // true = bonus level gem, disappears after 30s
    float       lifeTimer;     // counts up from 0
    float       bobTimer;      // used for the bobbing animation
    float       displayY;      // actual drawn y with bob offset applied

    Sprite* sprite;

    // Static shared texture loaded once, used by all Gem objects
    static Texture  sharedTexture;
    static bool     textureLoaded;

public:
    Gem(float x, float y, int value, bool hasTimer);
    virtual ~Gem();

    virtual void update(float dt);
    virtual void draw(RenderWindow& window);

    FloatRect getRect()    const;
    bool      isActive()   const;  // false if collected or timed out
    int       getValue()   const;
    void      collect();           // marks gem as collected
};



// FallingGem - inherits from Gem
// Used ONLY in bonus levels (level 4 and level 9)
// Falls from top of screen with gravity
// When player collects: +10 gems + 1000 score
// Disappears after 20 seconds if not collected
class FallingGem : public Gem
{
private:
    float velY;       // falling speed  increases due to gravity
    float gravity;    // pulls gem downward every frame
    float floorY;     // y position of the floor  gem stops here

public:
    FallingGem(float x, float y, float wW, float wH);

    // Overrides Gem::update() adds falling physics
    void update(float dt) override;

    // Overrides Gem::draw() no flashing, just draws normally
    void draw(RenderWindow& window) override;
};