/* Muhammad Abdullah Malik  25I-3006
   Muhammad Habib Ashfaq    25I-3025  SE-D */

#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;

// =====================================================================
// StarPowerUp
//it is a separate collectible trigger. The PowerUp hierarchy handles effects; StarPowerUp handles
// the collection event and triggers the panel in SnowBros.

class StarPowerUp
{
private:
    float x;            
    float y;            
    float displayY;     // actual draw Y with sine-wave bob applied
    bool  collected;    // true = player collected it, panel is opening
    bool  active;       // false = not on screen (already collected)

    float bobTimer;      // counts up every frame for sine wave bobbing
    float rotationAngle; // increases every frame for slow spin

    // The star shape 
    // ConvexShape with 10 alternating outer/inner points.
    // ConvexShape is used instead of CircleShape(r,5) because CircleShape
    // with 5 points draws a pentagon A real star needs alternating outer radius
    // (20px) and inner radius (8px) at 36-degree intervals, which ConvexShape supports via setPoint().
    ConvexShape starShape;

    // Soft glow circle drawn behind the star for visual depth.
    // Drawn first (behind) so star renders on top.
    CircleShape glowCircle;

public:
    // Constructor: positions star at center of screen
    StarPowerUp(float screenW, float screenH);

    // reset()  called when player dies on the star level without collecting.
    // Repositions and re-activates the star so it reappears on respawn.
    void reset(float screenW, float screenH);

    // update(dt) -- call every frame while star is active.
    // Advances bobTimer and rotationAngle, updates displayY and shape transform.
    void update(float dt);

    // draw() -- renders glow circle then star shape.
    void draw(RenderWindow& window);

    // getRect() -- returns a 40x40 FloatRect centered on the star.
    //  SnowBros use it to check overlap with the player's collision rect.
    FloatRect getRect() const;

    // isActive()  returns true if the star is visible and can be collected.
    bool isActive() const { return active && !collected; }

    // collect()  called by SnowBros when player overlaps the star.
    // Deactivates the star so it disappears from the screen.
    void collect() { collected = true; active = false; }

    // wasCollected()  returns true if star was already collected.
    // Used by death handler to avoid resetting an already-collected star.
    bool wasCollected() const { return collected; }
};