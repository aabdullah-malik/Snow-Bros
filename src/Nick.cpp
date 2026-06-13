#include "Nick.h"

Nick::Nick(float x, float y, float wW, float wH) : Player(x, y, wW, wH)
{
    loadSprite();
    setupFrames();
    playerNum = 1;
}

void Nick::loadSprite()
{
    texture.loadFromFile("SnowBrosAssets/Images/Nick.png");
    sprite = new Sprite(texture);
}

void Nick::setupFrames()
{
    // IDLE - 1 frame 
    idleFrame = IntRect({ 116, 70 }, { 220, 275 });

    // NORMAL WALK - 3 frames 
    walkFrames[0] = IntRect({ 718, 64 }, { 201, 281 });
    walkFrames[1] = IntRect({ 412, 70 }, { 174, 275 });
    walkFrames[2] = IntRect({ 1023, 64 }, { 192, 281 });

    // SPRINT - 4 frames 
    sprintFrames[0] = IntRect({ 79, 672 }, { 257, 284 });
    sprintFrames[1] = IntRect({ 403, 672 }, { 229, 284 });
    sprintFrames[2] = IntRect({ 699, 672 }, { 248, 284 });
    sprintFrames[3] = IntRect({ 958, 681 }, { 276, 275 });

    // JUMP - 7 frames 
    jumpFrames[0] = IntRect({ 1310, 64 }, { 211, 282 });
    jumpFrames[1] = IntRect({ 79, 366 }, { 229, 276 });
    jumpFrames[2] = IntRect({ 403, 357 }, { 211, 238 });
    jumpFrames[3] = IntRect({ 709, 366 }, { 210, 230 });
    jumpFrames[4] = IntRect({ 1005, 366 }, { 210, 276 });
    jumpFrames[5] = IntRect({ 1329, 385 }, { 210, 247 });
    jumpFrames[6] = IntRect({ 1625, 385 }, { 238, 238 });

    // FALL - 3 frames 
    fallFrames[0] = IntRect({ 86, 1282 }, { 226, 198 });
    fallFrames[1] = IntRect({ 357, 1282 }, { 210, 257 });
    fallFrames[2] = IntRect({ 764, 1310 }, { 211, 220 });

    // PUSH NORMAL - 3 frames 
    pushNormalFrames[0] = IntRect({ 1616, 1292 }, { 220, 275 });
    pushNormalFrames[1] = IntRect({ 1292, 1282 }, { 210, 285 });
    pushNormalFrames[2] = IntRect({ 1921, 1292 }, { 211, 275 });

    // PUSH SPRINT - 3 frames
    pushSprintFrames[0] = IntRect({ 1597, 672 }, { 266, 284 });
    pushSprintFrames[1] = IntRect({ 1921, 672 }, { 230, 284 });
    pushSprintFrames[2] = IntRect({ 2227, 672 }, { 238, 284 });

    // DIE - 7 frames 
    dieFrames[0] = IntRect({ 61, 968 }, { 283, 307 });
    dieFrames[1] = IntRect({ 357, 977 }, { 303, 294 });
    dieFrames[2] = IntRect({ 672, 1033 }, { 284, 256 });
    dieFrames[3] = IntRect({ 968, 1032 }, { 284, 284 });
    dieFrames[4] = IntRect({ 1262, 1034 }, { 307, 161 });
    dieFrames[5] = IntRect({ 1575, 1036 }, { 302, 134 });
    dieFrames[6] = IntRect({ 1874, 1034 }, { 318, 88 });

    // HAPPY - 3 frames
    happyFrames[0] = IntRect({ 88, 2190 }, { 230, 322 });
    happyFrames[1] = IntRect({ 412, 2190 }, { 248, 293 });
    happyFrames[2] = IntRect({ 701, 2191 }, { 222, 300 });

    // BALLOON - 6 frames
    balloonFrames[0] = IntRect({ 79, 2569 }, { 266, 377 });
    balloonFrames[1] = IntRect({ 348, 2504 }, { 321, 424 });
    balloonFrames[2] = IntRect({ 674, 2500 }, { 419, 448 });
    balloonFrames[3] = IntRect({ 1101, 2500 }, { 450, 456 });
    balloonFrames[4] = IntRect({ 1580, 2500 }, { 437, 451 });
    balloonFrames[5] = IntRect({ 2004, 2500 }, { 471, 445 });

    // THROW - 4 frames 
    throwFrames[0] = IntRect({ 403, 1579 }, { 211, 285 });
    throwFrames[1] = IntRect({ 653, 1597 }, { 275, 267 });
    throwFrames[2] = IntRect({ 1014, 1597 }, { 220, 285 });
    throwFrames[3] = IntRect({ 1264, 1588 }, { 285, 276 });

    // initialize sprite with idle frame
    sprite->setTextureRect(idleFrame);
    sprite->setPosition({ playerX, playerY });

    float scaleX = hitbox.getWidth() / idleFrame.size.x;
    float scaleY = hitbox.getHeight() / idleFrame.size.y;
    sprite->setScale({ scaleX, scaleY });  // makes the player of the size we want
}