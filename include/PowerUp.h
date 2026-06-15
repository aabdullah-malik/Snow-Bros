#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

// Forward declaration  because applyTo() takes a Player*
// We cannot include Player.h here would cause circular include
// Player.h includes PowerUp.h, so PowerUp.h cannot include Player.h
class Player;

// PowerUp - ABSTRACT BASE CLASS

class PowerUp
{
protected:
    float    x, y;       // spawn position on screen
    float    displayY;   // y position with bobbing applied
    bool     collected;  // true = player walked over it
    float    lifeTimer;  // counts UP  item disappears when >= 15 seconds
    float    bobTimer;   // counts up for sine wave bobbing

    Sprite* sprite;     // for visuals

    // Static shared texture - loaded from disk only once
    // All 4 subclasses share the same Items.png texture in memory
    static Texture sharedTexture;
    static bool    textureLoaded;

public:
    // Constructor takes the specific sprite rect for that power-up type
    // Each subclass calls this with its own IntRect from Shop.h coordinates
    PowerUp(float x, float y, IntRect spriteRect);

    virtual ~PowerUp();

   
    // Each subclass overrides this with its specific effect on the player
    //  SnowBros call this checkPowerUpCollection() when player collects a power up
    virtual void applyTo(Player* p) = 0;

    
    // update() handles bobbing animation + 15-second life timer
    void      update(float dt);

    // draw() renders sprite with flashing in last 5 seconds
    void      draw(RenderWindow& window);

    // getRect() returns 45x45 collision box for player overlap check
    FloatRect getRect()  const;

    // isActive() returns false if collected OR timer >= 15 seconds
    bool      isActive() const;

    // collect() marks this power-up as taken so it disappears
    void      collect();
};