#include "Player.h"
#include "Platform.h"
#include "SnowBall.h"
#include <iostream>

Player::Player(float x, float y, float wW, float wH) : hitbox(70.f, 100.f), stats(2), launcher(wW, wH, &snowTexture)
{
    windowW = wW;
    windowH = wH;
    playerX = x;
    playerY = y;
    velX = 0;
    velY = 0;
    speed = 350.f;
    baseSpeed = 350.f;
    sprintSpeed = 500.f;
    gravity = 1800.f;
    jumpStrength = 900.f;
    playerNum = 1;
    onGround = false;
    facingRight = false;
    isSprinting = false;
    currentFrame = 0;
    currentAnim = ANIM_IDLE;
    prevAnim = ANIM_IDLE;
    animTimer = 0.f;
    animSpeed = 0.12f;  // time per frame
    sprite = nullptr;
    isPushing = false;
    pushTimer = 0.f;
    pushKeyPressed = false;
    pushKeyHeld = false;
    pushFrame = 0;
    pushAnimSpeed = 0.4f;
    pushFrameCount = 3;
    justJumped = false;
    invincible = false;
    invincibilityTimer = 0.f;
    snowTexture.loadFromFile("SnowBrosAssets/Images/Items.png");

    // All power-ups start inactive
    speedBoostActive = false;
    speedBoostTimer = 0.f;
    snowballPowerActive = false;
    snowballPowerTimer = 0.f;
    distanceIncreaseActive = false;
    distanceIncreaseTimer = 0.f;
    balloonModeActive = false;
    balloonModeTimer = 0.f;
}


// ACTIVATE SPEED BOOST
// Called by SpeedBoost::applyTo(Player* p)
// Sets speed to 150% of base speed for 15 seconds

void Player::activateSpeedBoost()
{
    speedBoostActive = true;
    if (speedBoostTimer < 15.f) speedBoostTimer = 15.f;  // never shorten a permanent timer
    speed = baseSpeed * 1.5f;
    sprintSpeed = 600.f * 1.5f;
}


// ACTIVATE SNOWBALL POWER
// Called by SnowballPower::applyTo(Player* p)
// Sets flag that SnowBros::checkSnowballEnemyCollision() checks
// When flag is true, snowball adds maxSnowCoat hits instead of 1

void Player::activateSnowballPower()
{
    snowballPowerActive = true;
    if (snowballPowerTimer < 15.f) snowballPowerTimer = 15.f;  // never shorten a permanent timer
}


// ACTIVATE DISTANCE INCREASE
// Called by DistanceIncrease::applyTo(Player* p)
// Sets flag that SnowballLauncher checks when firing
// When flag is true, snowball wraps around screen instead of stopping

void Player::activateDistanceIncrease()
{
    distanceIncreaseActive = true;
    if (distanceIncreaseTimer < 15.f) distanceIncreaseTimer = 15.f;  // never shorten a permanent timer
}


// ACTIVATE BALLOON MODE
// Called by BalloonMode::applyTo(Player* p)
// Sets flag checked in update() - gravity is ignored while active
// velY is set once here to -80 (upward float speed).
// We do NOT reset velY every frame inside update() so that the
// bounce logic can change velY when player hits the top wall.

void Player::activateBalloonMode()
{
    balloonModeActive = true;
    if (balloonModeTimer < 10.f) balloonModeTimer = 10.f;  // never shorten a permanent timer
    velY = -150.f;  // start floating upward immediately
}



// PERMANENT ACTIVATE FUNCTIONS (Star panel on level 7)
// Same as normal activations but timer set to 99999 so it never
// expires during a level. clearAllPowerUps() still clears them
// when the level ends or player dies.

void Player::activateSpeedBoostPermanent()
{
    speedBoostActive = true;
    speedBoostTimer = 99999.f;
    speed = baseSpeed * 1.5f;
    sprintSpeed = 600.f * 1.5f;
}

void Player::activateSnowballPowerPermanent()
{
    snowballPowerActive = true;
    snowballPowerTimer = 99999.f;
}

void Player::activateDistanceIncreasePermanent()
{
    distanceIncreaseActive = true;
    distanceIncreaseTimer = 99999.f;
}

void Player::activateBalloonModePermanent()
{
    balloonModeActive = true;
    balloonModeTimer = 99999.f;
    velY = -150.f;  // start floating upward immediately
}


// UPDATE POWER UP TIMERS
// Called every frame from update()
// Counts each active timer DOWN from 15 to 0
// When timer hits 0, deactivates that effect and restores values

void Player::updatePowerUpTimers(float dt)
{
    if (speedBoostActive)
    {
        speedBoostTimer -= dt;
        if (speedBoostTimer <= 0.f)
        {
            speedBoostActive = false;
            speedBoostTimer = 0.f;
            speed = baseSpeed;   // restore original speed
            sprintSpeed = 600.f;
        }
    }

    if (snowballPowerActive)
    {
        snowballPowerTimer -= dt;
        if (snowballPowerTimer <= 0.f)
        {
            snowballPowerActive = false;
            snowballPowerTimer = 0.f;
        }
    }

    if (distanceIncreaseActive)
    {
        distanceIncreaseTimer -= dt;
        if (distanceIncreaseTimer <= 0.f)
        {
            distanceIncreaseActive = false;
            distanceIncreaseTimer = 0.f;
        }
    }

    if (balloonModeActive)
    {
        balloonModeTimer -= dt;
        if (balloonModeTimer <= 0.f)
        {
            balloonModeActive = false;
            balloonModeTimer = 0.f;
        }
    }

    if (invincible)
    {
        invincibilityTimer -= dt;
        if (invincibilityTimer <= 0.f)
        {
            invincible = false;
            invincibilityTimer = 0.f;
        }
    }
}


// CLEAR ALL POWER UPS
// Called by SnowBros::advanceLevel() when moving to next level
// Turns off ALL effects and restores original values

void Player::clearAllPowerUps()
{
    speedBoostActive = false;
    speedBoostTimer = 0.f;
    speed = baseSpeed;
    sprintSpeed = 600.f;

    snowballPowerActive = false;
    snowballPowerTimer = 0.f;

    distanceIncreaseActive = false;
    distanceIncreaseTimer = 0.f;

    balloonModeActive = false;
    balloonModeTimer = 0.f;
}


// SET STATE

void Player::setState(int newState)
{
    if (currentAnim == newState) return;
    prevAnim = currentAnim;
    currentAnim = newState;
    currentFrame = 0;
    animTimer = 0.f;
}

// HANDLE INPUT
// Checks keyboard every frame and sets velX, velY, facing direction
// Balloon mode blocks normal jumping - player floats instead

void Player::handleInput()
{
    if (stats.getIsDead())
    {
        return;
    }

    velX = 0;

    // Sprinting is ONLY possible with Speed Boost power-up
    isSprinting = speedBoostActive;

    if (playerNum == 1)
    {
        float s = isSprinting ? sprintSpeed : speed;

        if (Keyboard::isKeyPressed(Keyboard::Key::A))
        {
            velX = -s; facingRight = false;
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::D))
        {
            velX = s; facingRight = true;
        }

        // Balloon mode: jumping disabled, player floats automatically
        if (!balloonModeActive)
        {
            if (Keyboard::isKeyPressed(Keyboard::Key::W) && onGround)
            {
                velY = -jumpStrength;
                justJumped = true;
                onGround = false;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Space))
        {
            if (!launcher.getIsThrowing())
            {
                setState(ANIM_THROW);
                launcher.fire(playerX, playerY, facingRight);
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::K))
        {
            if (!pushKeyHeld)
            {
                pushKeyPressed = true;  // it is triggered only once the player presses the push key
            }
            pushKeyHeld = true;
        }
        else
        {
            pushKeyHeld = false;
        }
    }
    else if (playerNum == 2)
    {
        float s = isSprinting ? sprintSpeed : speed;

        if (Keyboard::isKeyPressed(Keyboard::Key::Left))
        {
            velX = -s; facingRight = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Right))
        {
            velX = s; facingRight = true;
        }

        // Balloon mode: jumping disabled
        if (!balloonModeActive)
        {
            if (Keyboard::isKeyPressed(Keyboard::Key::Up) && onGround)
            {
                velY = -jumpStrength;
                onGround = false;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
        {
            if (!launcher.getIsThrowing())
            {
                setState(ANIM_THROW);
                launcher.fire(playerX, playerY, facingRight);
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::O))
        {
            if (!pushKeyHeld)
            {
                pushKeyPressed = true;
            }
            pushKeyHeld = true;
        }
        else
        {
            pushKeyHeld = false;
        }
    }
}

bool Player::getJustJumped()
{
    bool j = justJumped;
    justJumped = false;
    return j;
}

// Called every frame by SnowBros::updateGameplay()
void Player::update(float dt, Platform* platforms, int platformCount)
{
    // only handles input if alive
    if (!stats.getIsDead())
    {
        handleInput();
    }

    // Tick all active power-up timers down every frame
    updatePowerUpTimers(dt);

    if (velY > 1200.f)  // capped
    {
        velY = 1200.f;
    }

    launcher.update(dt, platforms, platformCount);

    // BALLOON MODE: zero gravity, gentle upward float
    // This block replaces the normal physics section completely
    if (balloonModeActive)
    {
        // --- BALLOON PHYSICS ---
        // velY is always -80 (upward float). We never change it except at
        // the bottom wall. No drift logic -- instant snap-back on top wall.

        // Move player using constant upward velY and horizontal input
        playerX += velX * dt;
        playerY += velY * dt;

        // --- Screen boundary checks ---
        // Left / right walls: hard clamp
        if (playerX < 80)
            playerX = 80;
        if (playerX + hitbox.getWidth() > windowW - 80)
            playerX = windowW - 80 - hitbox.getWidth();

        // Top wall: instant bounce.
        // Instead of clamping to 0 (which causes infinite re-trigger since
        // Top wall bounce: snap position to 40px from top AND flip velY
        // to positive (downward) so player bounces back down.
        // Without flipping velY the player would instantly float back up.
        if (playerY < 0)
        {
            playerY = 40.f;
            velY = 150.f;   // bounce downward
        }

        // Bottom wall: clamp and keep floating up
        if (playerY + hitbox.getHeight() > windowH - 70)
        {
            playerY = windowH - 70 - hitbox.getHeight();
            velY = -80.f;  // ensure upward float resumes from floor
        }

        hitbox.update(playerX, playerY);
        onGround = false;

        // Use balloon animation while floating
        setState(ANIM_BALLOON);
        updateAnimation(dt);

        // --- 1.5x visual scale ---
        // The hitbox stays the same size (fair collision).
        // We multiply the sprite scale by 1.5 so the player LOOKS bigger.
        // textureRect.size.x/y is the size of one animation frame in pixels.
        float bScaleX = (hitbox.getWidth() / sprite->getTextureRect().size.x) * 1.5f;
        float bScaleY = (hitbox.getHeight() / sprite->getTextureRect().size.y) * 1.5f;

        if (facingRight)
        {
            // Negative X scale flips sprite to face right
            sprite->setScale({ -bScaleX, bScaleY });
            sprite->setPosition({ playerX + hitbox.getWidth(), playerY });
        }
        else
        {
            sprite->setScale({ bScaleX, bScaleY });
            sprite->setPosition({ playerX, playerY });
        }
        return;   // skip normal physics below
    }


    // NORMAL PHYSICS - ABDULLAH'S PART
    velY += gravity * dt;
    playerX += velX * dt;
    playerY += velY * dt;

    onGround = false;
    hitbox.update(playerX, playerY);  // updates hitbox position every frame
    FloatRect playerRect = hitbox.getRect();

    for (int i = 0; i < platformCount; i++)
    {
        FloatRect platRect = platforms[i].getRect();

        bool hOverlap = playerRect.position.x + playerRect.size.x > platRect.position.x &&  // player’s right side is past platform’s left side
            playerRect.position.x < platRect.position.x + platRect.size.x;  // player’s left side is before platform’s right side
        bool vCol = playerRect.position.y + playerRect.size.y >= platRect.position.y &&  // player’s bottom is at or below platform top
            playerRect.position.y + playerRect.size.y <= platRect.position.y + 55 &&  /* player is within a small vertical range(55 pixels)
                                                                                         so that the player doesnt seem to be inside the platform */
            velY > 0;  // collision only happens when player is landing on platform, not jumping through it

        if (hOverlap && vCol)
        {
            playerY = platRect.position.y - hitbox.getHeight();  // put player exactly on top of platform
            velY = 0;  // stop moving downward
            onGround = true;
            hitbox.update(playerX, playerY);
            playerRect = hitbox.getRect();
        }
    }

    if (playerX < 80)
    {
        playerX = 80;
    }
    if (playerX + hitbox.getWidth() > windowW - 80)
    {
        playerX = windowW - 80 - hitbox.getWidth();
    }
    if (playerY < 0)
    {
        playerY = 0;
        velY = 0;
    }
    if (playerY + hitbox.getHeight() > windowH - 70)
    {
        playerY = windowH - 70 - hitbox.getHeight();
        velY = 0;
        onGround = true;
    }
    hitbox.update(playerX, playerY);

    // Animation states
    if (!stats.getIsDead())
    {
        if (isPushing && isSprinting)
        {
            setState(ANIM_PUSH_SPRINT);
        }
        else if (isPushing)
        {
            setState(ANIM_PUSH_NORMAL);
        }
        else if (launcher.getIsThrowing())
        {
            setState(ANIM_THROW);
        }
        else if (!onGround && velY < 0)
        {
            setState(ANIM_JUMP);
        }
        else if (!onGround && velY > 0)
        {
            setState(ANIM_FALL);
        }
        else if (onGround && velX != 0 && isSprinting)
        {
            setState(ANIM_SPRINT);
        }
        else if (onGround && velX != 0)
        {
            setState(ANIM_WALK);
        }
        else
        {
            setState(ANIM_IDLE);
        }
    }
    else
    {
        setState(ANIM_DIE);
    }

    updateAnimation(dt);   // show that animation

    if (facingRight)
    {
        sprite->setScale({ -(hitbox.getWidth() / sprite->getTextureRect().size.x),
                              hitbox.getHeight() / sprite->getTextureRect().size.y });
        sprite->setPosition({ playerX + hitbox.getWidth(), playerY });
    }
    else
    {
        sprite->setScale({ hitbox.getWidth() / sprite->getTextureRect().size.x,
                            hitbox.getHeight() / sprite->getTextureRect().size.y });
        sprite->setPosition({ playerX, playerY });
    }
}


// this function displays correct frame for a particular case
void Player::updateAnimation(float dt)
{
    animTimer += dt;   // animTimer used to keep track of time after which our animation will change

    int total = 1;   // it means total frames per animation
    switch (currentAnim)
    {
    case ANIM_IDLE:
        total = 1;
        break;
    case ANIM_WALK:
        total = 3;
        break;
    case ANIM_SPRINT:
        total = 4;
        break;
    case ANIM_JUMP:
        total = 7;
        break;
    case ANIM_FALL:
        total = 3;
        break;
    case ANIM_PUSH_NORMAL:
        total = 3;
        break;
    case ANIM_PUSH_SPRINT:
        total = 3;
        break;
    case ANIM_DIE:
        total = 7;
        break;
    case ANIM_HAPPY:
        total = 3;
        break;
    case ANIM_BALLOON:
        total = 6;
        break;
    case ANIM_THROW:
        total = 4;
        break;
    }

    if (animTimer >= animSpeed)  // after 0.12s next frame comes
    {
        animTimer = 0.f;
        currentFrame++;

        if (currentAnim == ANIM_DIE)
        {
            if (currentFrame >= total)
            {
                currentFrame = total - 1;
                stats.setDeathDone(true);   // make it remain in the dead frame
            }
        }
        else if (currentAnim == ANIM_PUSH_NORMAL || currentAnim == ANIM_PUSH_SPRINT)
        {
            if (currentFrame >= total)
            {
                currentFrame = 0;
                isPushing = false;   // stops pushing once all frames are done
            }
        }
        else
        {
            if (currentFrame >= total)
            {
                currentFrame = 0;
            }
        }
    }

    switch (currentAnim)
    {
    case ANIM_IDLE:
        sprite->setTextureRect(idleFrame);
        break;
    case ANIM_WALK:
        sprite->setTextureRect(walkFrames[currentFrame]);
        break;
    case ANIM_SPRINT:
        sprite->setTextureRect(sprintFrames[currentFrame]);
        break;
    case ANIM_JUMP:
        sprite->setTextureRect(jumpFrames[currentFrame]);
        break;
    case ANIM_FALL:
        sprite->setTextureRect(fallFrames[currentFrame]);
        break;
    case ANIM_PUSH_NORMAL:
        sprite->setTextureRect(pushNormalFrames[currentFrame]);
        break;
    case ANIM_PUSH_SPRINT:
        sprite->setTextureRect(pushSprintFrames[currentFrame]);
        break;
    case ANIM_DIE:
        sprite->setTextureRect(dieFrames[currentFrame]);
        break;
    case ANIM_HAPPY:
        sprite->setTextureRect(happyFrames[currentFrame]);
        break;
    case ANIM_BALLOON:
        sprite->setTextureRect(balloonFrames[currentFrame]);
        break;
    case ANIM_THROW:
        sprite->setTextureRect(throwFrames[currentFrame]);
        break;
    }
}

void Player::draw(RenderWindow& window)
{
    launcher.draw(window);
    if (stats.getIsDead() && stats.getDeathDone())
    {
        return;
    }
    // Flicker effect during invincibility: hide sprite every other 0.1s
    // (int)(invincibilityTimer * 10) % 2 alternates between 0 and 1
    // ten times per second, giving a visible flash effect
    if (invincible && (int)(invincibilityTimer * 10) % 2 == 0)
    {
        return;  // skip drawing this frame = flicker
    }
    window.draw(*sprite);
}

void Player::drawDebug(RenderWindow& window)
{
    if (stats.getIsDead() && stats.getDeathDone())
    {
        return;
    }
    hitbox.draw(window, Color::Green);
}

FloatRect Player::getRect() { return hitbox.getRect(); }
float Player::getX() { return playerX; }
float Player::getY() { return playerY; }
int Player::getLives() { return stats.getLives(); }
int Player::getScore() { return stats.getScore(); }
bool Player::getIsDead() { return stats.getIsDead(); }
bool Player::getDeathDone() { return stats.getDeathDone(); }
bool Player::getFacingRight() { return facingRight; }

SnowballLauncher& Player::getLauncher() { return launcher; }

void Player::loseLife()
{
    if (invincible)
    {
        return;
    }
    stats.loseLife();
}

void Player::addLife() { stats.addLife(); }
void Player::addScore(int pts) { stats.addScore(pts); }

void Player::revive()
{
    stats.revive();
    playerX = (playerNum == 1) ? 200.f : windowW - 500.f;
    playerY = 0;
    velX = 0; velY = 0;

    // grants 2.5 seconds of invincibility after respawn
    invincible = true;
    invincibilityTimer = 2.5f;
}

void Player::startPush()
{
    if (isPushing)
    {
        return;
    }
    isPushing = true;
    pushFrame = 0;
    setState(isSprinting ? ANIM_PUSH_SPRINT : ANIM_PUSH_NORMAL);
}

bool Player::getPushKeyPressed() { return pushKeyPressed; }
void Player::clearPushKeyPressed() { pushKeyPressed = false; }
bool Player::getIsPushing() { return isPushing; }

Player::~Player() { delete sprite; }