#include "Enemy.h"
#include "Platform.h"
#include<iostream>

Texture Enemy::snowTexture;
bool Enemy::snowTexLoaded = false;
IntRect Enemy::snowFrames[4];
IntRect Enemy::meltFrames[4];

Enemy::Enemy(float x, float y, float wW, float wH, float hbW, float hbH) : hitbox(hbW, hbH)
{
    windowW = wW;
    windowH = wH;
    enemyX = x;
    enemyY = y;
    velX = 0;
    velY = 0;
    gravity = 1800.f;
    speed = 90.f;
    speedMultiplier = 1.0f;
    onGround = false;
    facingRight = false;
    alive = true;
    isRolling = false;
    addSnowFrameCount = 4;  // default 
    removeSnowFrameCount = 4;  // default 
    addingSnow = false;
    removingSnow = false;
    snowAnimFrame = 0;
    snowAnimTimer = 0.f;
    snowAnimSpeed = 0.1f;
    currentFrame = 0;
    animTimer = 0.f;
    animSpeed = 0.12f;

    rollSizeMultiplier = 1.0f;

    snowLevel = 0;
    snowMaxLevel = 4;
    snowCoated = false;
    snowRolling = false;
    snowRollVelX = 0;
    snowBreakTimer = 0;
    isMelting = false;
    meltFrame = 0;
    meltAnimTimer = 0;
    rollKillCount = 0;

    isDying = false;

    sprite = nullptr;
    snowSprite = nullptr;

    pendingRoll = false;
    rollDelay = 0.25f;
    rollDelayTimer = 0.f;

    if (!snowTexLoaded) {
        snowTexLoaded = true;
        snowTexture.loadFromFile("SnowBrosAssets/Images/Nick.png");
        snowFrames[0] = IntRect({ 103, 3396 }, { 201, 169 });
        snowFrames[1] = IntRect({ 394, 3328 }, { 238, 248 });
        snowFrames[2] = IntRect({ 688, 3311 }, { 257, 252 });
        snowFrames[3] = IntRect({ 996, 3266 }, { 252, 294 });
        meltFrames[0] = IntRect({ 90, 3884 }, { 230, 281 });
        meltFrames[1] = IntRect({ 370, 3879 }, { 273, 290 });
        meltFrames[2] = IntRect({ 677, 3872 }, { 274, 298 });
        meltFrames[3] = IntRect({ 962, 3873 }, { 299, 307 });
    }
    snowSprite = new Sprite(snowTexture);
}

void Enemy::addSnow(int amount)
{
    if (snowCoated || snowRolling || !alive || isMelting || isDying)
    {
        return;
    }
    snowLevel += amount;
    addingSnow = true;
    snowAnimFrame = 0;
    snowAnimTimer = 0.f;
    // Starts the break timer from the very first snow hit
    if (snowBreakTimer == 0.f)
    {
        snowBreakTimer = 0.001f; // mark as started
    }
    if (snowLevel >= snowMaxLevel)
    {
        snowLevel = snowMaxLevel;
        snowCoated = true;
    }
}

void Enemy::startMelting() {
    if (isMelting)
    {
        return;
    }
    isMelting = true;
    meltFrame = 0;
    meltAnimTimer = 0.f;
    snowRolling = false;
}

void Enemy::onHitByRollingSnowball()
{
    if (snowCoated || !alive || isDying)
    {
        return;
    }
    takeDamage(2);
    if (health <= 0)
    {
        die();
    }
}

void Enemy::drawSnowVisuals(RenderWindow& window)
{
    if (!alive || snowLevel <= 0 || snowSprite == nullptr)
    {
        return;
    }

    IntRect frame;
    if (isMelting)
    {
        frame = meltFrames[meltFrame];
    }
    else if (snowCoated)
    {
        frame = snowFrames[3];
    }
    else   // when its partially coated
    {
        if (snowLevel <= 1)
        {
            frame = snowFrames[0];  // light snow
        }
        else if (snowLevel <= 2)
        {
            frame = snowFrames[1];  // medium snow
        }
        else
        {
            frame = snowFrames[2];  // heavy snow
        }
    }

    snowSprite->setTextureRect(frame);

    // scaling to match the hitbox
    float sx = hitbox.getWidth() / (float)frame.size.x;
    float sy = hitbox.getHeight() / (float)frame.size.y;

    if (snowRolling)
    {
        sx *= rollSizeMultiplier;
        sy *= rollSizeMultiplier;
    }

    if (facingRight) {
        snowSprite->setPosition({ enemyX + hitbox.getWidth(), enemyY });
        sx = -sx;  // flip to match sprite direction
    }
    else {
        snowSprite->setPosition({ enemyX, enemyY });
    }

    snowSprite->setScale({ sx, sy });
    window.draw(*snowSprite);
}

void Enemy::pushSnowball(float dirX)
{
    if (!snowCoated || snowRolling || !alive || isDying)
    {
        return;
    }
    pendingRoll = true;
    rollDelayTimer = 0.f;
    snowRollVelX = (dirX > 0) ? 400.f : -400.f;
    rollSizeMultiplier = 1.0f;  // reset size on new roll
}

void Enemy::updateSnow(float dt, Platform* platforms, int platformCount)
{
    if (!alive)
    {
        return;
    }

    if (pendingRoll)
    {
        rollDelayTimer += dt;
        if (rollDelayTimer >= rollDelay) // starts rolling after 0.25s
        {
            pendingRoll = false;
            snowRolling = true;
        }
    }

    if (snowRolling) {
        // horizontal movement
        enemyX += snowRollVelX * dt;

        velY += gravity * dt;
        if (velY > 1200.f)
        {
            velY = 1200.f;
        }
        enemyY += velY * dt;

        hitbox.update(enemyX, enemyY);
        FloatRect enemyRect = hitbox.getRect();  // stored here to not call getRect again and again in the loop

        bool onPlatform = false;

        // check platform collisions
        for (int i = 0; i < platformCount; i++)
        {
            FloatRect platRect = platforms[i].getRect();

            bool horizontalOverlap = enemyRect.position.x + enemyRect.size.x > platRect.position.x &&
                enemyRect.position.x < platRect.position.x + platRect.size.x;

            bool verticalCollision = enemyRect.position.y + enemyRect.size.y >= platRect.position.y &&
                enemyRect.position.y + enemyRect.size.y <= platRect.position.y + 55 &&
                velY > 0;

            if (horizontalOverlap && verticalCollision)
            {
                enemyY = platRect.position.y - hitbox.getHeight();
                velY = 0;
                onPlatform = true;
                hitbox.update(enemyX, enemyY);
                enemyRect = hitbox.getRect();
            }
        }

        // screen wrap
        if (enemyX + hitbox.getWidth() < 0)
        {
            enemyX = windowW;
            velY = 0;
            hitbox.update(enemyX, enemyY);
        }
        if (enemyX > windowW)
        {
            enemyX = -hitbox.getWidth();
            velY = 0;
            hitbox.update(enemyX, enemyY);
        }

        // floor boundary so it never escapes below screen
        if (enemyY + hitbox.getHeight() > windowH - 70)
        {
            enemyY = windowH - 70 - hitbox.getHeight();
            velY = 0;
        }
    }

    else if (snowCoated)
    {
        snowBreakTimer += dt;
        if (snowBreakTimer >= 6.0f)   // start uncoating after 6s if fully coated
        {
            startMelting();
            snowBreakTimer = 0.f;
        }
    }
    else if (snowLevel > 0 && snowBreakTimer > 0.f && !addingSnow && !removingSnow)   // if partially coated
    {
        snowBreakTimer += dt;
        if (snowBreakTimer >= 3.5f)   // less time for partial coat
        {
            snowBreakTimer = 0.f;
            removingSnow = true;
            snowAnimFrame = 0;
            snowAnimTimer = 0.f;
            snowLevel = 0;
        }
    }

    if (isMelting)
    {
        meltAnimTimer += dt;
        if (meltAnimTimer >= 0.15f)
        {
            meltAnimTimer = 0.f;
            meltFrame++;
            if (meltFrame >= 4)   // all frames completed
            {
                isMelting = false;
                meltFrame = 0;
                snowLevel = 0;
                snowCoated = false;
                snowBreakTimer = 0.f;
                snowAnimFrame = 0;
                snowAnimTimer = 0.f;
            }
        }
    }
}

void Enemy::takeDamage(int amount)
{
    if (health <= 0 || isDying || !alive)
    {
        return;
    }

    health -= amount;
    if (health <= 0)
    {
        health = 0;
        die();
    }
}

void Enemy::applyGravityAndMove(float dt, Platform* platforms, int platformCount)
{
    velY += gravity * dt;

    enemyX += velX * dt;
    enemyY += velY * dt;

    onGround = false;

    hitbox.update(enemyX, enemyY);
    FloatRect enemyRect = hitbox.getRect();

    if (velY > 1200.f)
    {
        velY = 1200.f;  // capped at 1200 max
    }

    for (int i = 0; i < platformCount; i++)
    {
        FloatRect platRect = platforms[i].getRect();

        bool horizontalOverlap = enemyRect.position.x + enemyRect.size.x > platRect.position.x &&
            enemyRect.position.x < platRect.position.x + platRect.size.x;

        bool verticalCollision = enemyRect.position.y + enemyRect.size.y >= platRect.position.y &&
            enemyRect.position.y + enemyRect.size.y <= platRect.position.y + 55 &&
            velY > 0;

        if (horizontalOverlap && verticalCollision)
        {
            enemyY = platRect.position.y - hitbox.getHeight();
            velY = 0;
            onGround = true;
            hitbox.update(enemyX, enemyY);
            enemyRect = hitbox.getRect();
        }
    }

    // left wall
    if (enemyX < 80)
    {
        enemyX = 80;
        velX = speed;
        facingRight = true;
    }

    // right wall
    if (enemyX + hitbox.getWidth() > windowW - 80)
    {
        enemyX = windowW - 80 - hitbox.getWidth();
        velX = -speed;
        facingRight = false;
    }
}

void Enemy::updateSnowAnim(float dt)
{
    if (addingSnow)
    {
        snowAnimTimer += dt;
        if (snowAnimTimer >= snowAnimSpeed)
        {
            snowAnimTimer = 0.f;
            snowAnimFrame++;
            if (snowAnimFrame >= addSnowFrameCount)
            {
                snowAnimFrame = addSnowFrameCount - 1;   // last frame
                addingSnow = false;
            }
        }
        sprite->setTextureRect(addSnowFrames[snowAnimFrame]);
        return;
    }

    if (removingSnow)
    {
        snowAnimTimer += dt;
        if (snowAnimTimer >= snowAnimSpeed)
        {
            snowAnimTimer = 0.f;
            snowAnimFrame++;
            if (snowAnimFrame >= removeSnowFrameCount)
            {
                snowAnimFrame = 0;
                removingSnow = false;
                snowCoated = false;
                snowLevel = 0;
                snowBreakTimer = 0.f;  // reset for next coating
            }
        }
        sprite->setTextureRect(removeSnowFrames[snowAnimFrame]);
        return;
    }
}

bool Enemy::isAlive()
{
    return alive;
}
bool Enemy::getIsRolling()
{
    return isRolling;
}
int Enemy::getSnowLevel()
{
    return snowLevel;
}
int Enemy::getsnowMaxLevel()
{
    return snowMaxLevel;
}
int Enemy::getHealth()
{
    return health;
}
int Enemy::getMaxHealth()
{
    return maxHealth;
}
FloatRect Enemy::getRect()
{
    return hitbox.getRect();
}
FloatRect Enemy::getHitboxRect()
{
    return hitbox.getRect();
}
bool Enemy::getIsDying() { return isDying; }
float Enemy::getX() const { return enemyX; }
float Enemy::getY() const { return enemyY; }

void Enemy::drawDebug(RenderWindow& window)
{
    hitbox.draw(window, Color::Red);
}

Enemy::~Enemy()
{
    delete sprite;
}

HitBox& Enemy::getHitbox()
{
    return hitbox;
}