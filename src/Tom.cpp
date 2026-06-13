#include "Tom.h"

Tom::Tom(float x, float y, float wW, float wH) : Player(x, y, wW, wH)
{
    loadSprite();
    setupFrames();
    playerNum = 2;
}

void Tom::loadSprite()
{
    texture.loadFromFile("SnowBrosAssets/Images/Player_Red.png");
    sprite = new Sprite(texture);
}

void Tom::setupFrames()
{
    // IDLE - 1 frame 
    idleFrame = IntRect({ 19, 3 }, { 57, 73 });

    // NORMAL WALK - 3 frames 
    walkFrames[0] = IntRect({ 184, 1 }, { 53, 75 });
    walkFrames[1] = IntRect({ 100, 3 }, { 45, 73 });
    walkFrames[2] = IntRect({ 269, 1 }, { 50, 75 });

    // SPRINT - 4 frames 
    sprintFrames[0] = IntRect({ 8, 170 }, { 68, 75 });
    sprintFrames[1] = IntRect({ 98, 170 }, { 60, 75 });
    sprintFrames[2] = IntRect({ 179, 170 }, { 66, 75 });
    sprintFrames[3] = IntRect({ 251, 172 }, { 73, 74 });

    // JUMP - 7 frames 
    jumpFrames[0] = IntRect({ 348, 0 }, { 55, 75 });
    jumpFrames[1] = IntRect({ 8, 85 }, { 61, 73 });
    jumpFrames[2] = IntRect({ 98, 82 }, { 55, 63 });
    jumpFrames[3] = IntRect({ 182, 84 }, { 55, 63 });
    jumpFrames[4] = IntRect({ 264, 85 }, { 55, 73 });
    jumpFrames[5] = IntRect({ 353, 90 }, { 55, 65 });
    jumpFrames[6] = IntRect({ 435, 90 }, { 63, 63 });

    // FALL - 3 frames 
    fallFrames[0] = IntRect({ 12, 338 }, { 58, 52 });
    fallFrames[1] = IntRect({ 84, 338 }, { 56, 68 });
    fallFrames[2] = IntRect({ 197, 345 }, { 55, 58 });

    // PUSH NORMAL - 3 frames 
    pushNormalFrames[0] = IntRect({ 432, 341 }, { 58, 72 });
    pushNormalFrames[1] = IntRect({ 343, 338 }, { 55, 75 });
    pushNormalFrames[2] = IntRect({ 517, 341 }, { 55, 72 });

    // PUSH SPRINT - 3 frames 
    pushSprintFrames[0] = IntRect({ 427, 170 }, { 71, 75 });
    pushSprintFrames[1] = IntRect({ 517, 170 }, { 63, 75 });
    pushSprintFrames[2] = IntRect({ 601, 170 }, { 62, 75 });

    // DIE - 7 frameS
    dieFrames[0] = IntRect({ 4, 253 }, { 75, 82 });
    dieFrames[1] = IntRect({ 85, 253 }, { 81, 79 });
    dieFrames[2] = IntRect({ 172, 269 }, { 75, 68 });
    dieFrames[3] = IntRect({ 253, 269 }, { 81, 60 });
    dieFrames[4] = IntRect({ 335, 269 }, { 86, 44 });
    dieFrames[5] = IntRect({ 421, 269 }, { 83, 36 });
    dieFrames[6] = IntRect({ 503, 269 }, { 87, 26 });

    // HAPPY - 3 frames
    happyFrames[0] = IntRect({ 11, 588 }, { 60, 86 });
    happyFrames[1] = IntRect({ 100, 588 }, { 65, 79 });
    happyFrames[2] = IntRect({ 177, 588 }, { 61, 82 });

    // BALLOON - 6 frames
    balloonFrames[0] = IntRect({ 8, 691 }, { 71, 103 });
    balloonFrames[1] = IntRect({ 82, 675 }, { 86, 114 });
    balloonFrames[2] = IntRect({ 171, 672 }, { 115, 125 });
    balloonFrames[3] = IntRect({ 291, 672 }, { 125, 125 });
    balloonFrames[4] = IntRect({ 424, 672 }, { 116, 125 });
    balloonFrames[5] = IntRect({ 541, 672 }, { 123, 125 });

    // THROW - 4 frames 
    throwFrames[0] = IntRect({ 98, 421 }, { 55, 77 });
    throwFrames[1] = IntRect({ 167, 425 }, { 73, 73 });
    throwFrames[2] = IntRect({ 266, 422 }, { 58, 76 });
    throwFrames[3] = IntRect({ 335, 423 }, { 76, 75 });

    // initialize sprite with idle frame
    sprite->setTextureRect(idleFrame);
    sprite->setPosition({ playerX, playerY });

    float scaleX = hitbox.getWidth() / idleFrame.size.x;
    float scaleY = hitbox.getHeight() / idleFrame.size.y;
    sprite->setScale({ scaleX, scaleY });
}