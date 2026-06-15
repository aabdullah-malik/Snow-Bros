/* Muhammad Abdullah Malik  25I-3006
   Muhammad Habib Ashfaq    25I-3025  SE-D */

#include "StarPowerUp.h"

   // buildStarShape  (file-local helper)
   // Builds a proper 5-pointed star using ConvexShape with 10 points.
   //
   // Technical explanation:
   //   A 5-pointed star has 10 vertices total -- 5 outer tips and 5 inner
   //   concave points between each tip, alternating as you go around.
   //   We place them by computing angles and converting to X/Y offsets:
   //
   //   angle = startAngle + i * (2*PI / 10)
   //   startAngle = -PI/2  so the first point faces directly UP
   //   Even i (0,2,4,6,8) = outer tip at radius outerR
   //   Odd  i (1,3,5,7,9) = inner concave at radius innerR
   //
   //   x = r * cos(angle)   (horizontal offset from center)
   //   y = r * sin(angle)   (vertical offset from center)
   //   These are set relative to the shape's origin (0,0),
   //   then setPosition() places the whole shape on screen.

static void buildStarShape(ConvexShape& shape, float outerR, float innerR)
{
    const float PI = 3.14159265f;
    shape.setPointCount(10);  // 5 outer + 5 inner = 10 total vertices

    for (int i = 0; i < 10; i++)
    {
        // -PI/2 rotates start so first outer point faces up (12 o'clock)
        float angle = -PI / 2.f + i * (2.f * PI / 10.f);

        // Even index = outer tip, odd index = inner concave
        float r = (i % 2 == 0) ? outerR : innerR;

        // cos/sin convert polar (angle, radius) to Cartesian (x, y)
        shape.setPoint(i, { r * std::cos(angle), r * std::sin(angle) });
    }
}

// CONSTRUCTOR
// Places star at center of screen and builds its visual shapes.


StarPowerUp::StarPowerUp(float screenW, float screenH)
{
    x = screenW / 2.f;
    y = screenH / 2.f;
    displayY = y;
    collected = false;
    active = true;
    bobTimer = 0.f;
    rotationAngle = 0.f;

    // Build the star ConvexShape with outer radius 20, inner radius 8
    buildStarShape(starShape, 20.f, 8.f);
    starShape.setFillColor(Color(255, 215, 0));     // gold yellow fill
    starShape.setOutlineColor(Color(255, 140, 0));  // orange outline
    starShape.setOutlineThickness(2.f);
    // setOrigin to (0,0) -- points are already relative to center

    // Glow circle: soft semi-transparent yellow behind the star
    // Radius 26 (slightly larger than star outer radius 20)
    glowCircle.setRadius(26.f);
    glowCircle.setFillColor(Color(255, 255, 100, 80));  // alpha 80 = semi-transparent
    glowCircle.setOrigin({ 26.f, 26.f });               // center the circle on its position
}

// RESET
// Called when player dies on the star level -- reactivates the star
// so it appears again when the player respawns.

void StarPowerUp::reset(float screenW, float screenH)
{
    x = screenW / 2.f;
    y = screenH / 2.f;
    displayY = y;
    collected = false;
    active = true;
    bobTimer = 0.f;
    rotationAngle = 0.f;
}

// UPDATE
// Called every frame while star is active.
// Bobbing displayY = y + sin(bobTimer * 3) * 6
//   sin() oscillates between -1 and +1 continuously.
//   Multiplying by 6 gives a ±6 pixel vertical range.
//   bobTimer * 3 controls speed: 3 radians/sec ≈ one full bob per 2 seconds.
// Rotation  rotationAngle += 60 * dt
//   Star completes one full 360-degree rotation every 6 seconds.
//   setRotation() takes a sf::Angle -- we use degrees() to convert.

void StarPowerUp::update(float dt)
{
    if (!active || collected) return;

    // Advance timers
    bobTimer += dt;
    rotationAngle += 60.f * dt;  // 60 degrees per second
    if (rotationAngle >= 360.f) rotationAngle -= 360.f;  // wrap to 0-360

    // Compute bobbing Y position using sine wave
    displayY = y + std::sin(bobTimer * 3.f) * 6.f;

    // Apply position and rotation to star shape
    // setPosition sets the shape's origin point on screen
    starShape.setPosition({ x, displayY });
    starShape.setRotation(degrees(rotationAngle));

    // Glow circle follows the same position
    glowCircle.setPosition({ x, displayY });
}

// DRAW Renders glow circle first , then the star shape on top. last drawn = appears on top.

void StarPowerUp::draw(RenderWindow& window)
{
    if (!active || collected) return;

    // Draw glow first so star appears in front of it
    window.draw(glowCircle);
    window.draw(starShape);
}

// GETRECT
// Returns a 40x40 collision rectangle centered on the star.
// SnowBros uses this to check if the player's rect overlaps the star.
//to make collection feel responsive without being too generous.

FloatRect StarPowerUp::getRect() const
{
    return FloatRect({ x - 20.f, displayY - 20.f }, { 40.f, 40.f });
}