#include "GamakichiCreature.h"

Texture GamakichiCreature::sharedTexture;
bool GamakichiCreature::textureLoaded = false;

GamakichiCreature::GamakichiCreature(float x, float y, float wW, float wH, bool goLeft) : Enemy(x, y, wW, wH, 60.f, 63.f)
{
    velX = goLeft ? -150.f : 150.f;
    velY = -800.f;  
    gravity = 1000.f;
    animFrame = 0;
    animTimer = 0.f;
    isDying = false;
    isBombDeath = false;
    alive = true;
    snowMaxLevel = 4;

    facingRight = !goLeft;
    loadSprite();
    setupFrames();

    jumpTimer = 0.f;
    jumpInterval = 1.5f + (rand() % 2);
}

bool GamakichiCreature::showHitbox = false;

void GamakichiCreature::preloadTexture() 
{
    if (!textureLoaded) 
    {
        sharedTexture.loadFromFile("SnowBrosAssets/Images/Gamakichi.png");
        textureLoaded = true;
    }
}

void GamakichiCreature::setColor(int color) {}

void GamakichiCreature::loadSprite()
{
    if (!textureLoaded) 
    {
        sharedTexture.loadFromFile("SnowBrosAssets/Images/Gamakichi.png");
        textureLoaded = true;
    }
    sprite = new Sprite(sharedTexture); // use shared texture
}

void GamakichiCreature::setupFrames()
{
    idleFrame = IntRect({ 1665, 1241 }, { 219, 229 });
    walkFrames[0] = IntRect({ 1140, 1223 }, { 216, 240 });
    walkFrames[1] = IntRect({ 1906, 1173 }, { 278, 297 });
    walkFrames[2] = IntRect({ 1398, 1177 }, { 261, 284 });
    jumpFrames[0] = IntRect({ 39, 1136 }, { 263, 319 });
    jumpFrames[1] = IntRect({ 34, 1198 }, { 206, 260 });
    deathFrames[0] = IntRect({ 35, 1492 }, { 229, 225 });
    deathFrames[1] = IntRect({ 633, 1530 }, { 161, 187 });
    deathFrames[2] = IntRect({ 805, 1476 }, { 265, 240 });
    deathFrames[3] = IntRect({ 1109, 1462 }, { 223, 249 });
    deathFrames[4] = IntRect({ 1404, 1511 }, { 200, 206 });
    bombDeathFrames[0] = IntRect({ 305, 1501 }, { 200, 216 });
    bombDeathFrames[1] = IntRect({ 2716, 0 }, { 316, 332 });
    bombDeathFrames[2] = IntRect({ 3034, 0 }, { 336, 332 });
    bombDeathFrames[3] = IntRect({ 3391, 16 }, { 297, 306 });
    bombDeathFrames[4] = IntRect({ 3739, 45 }, { 219, 191 });

    sprite->setTextureRect(idleFrame);
    spriteScaleX = 60.f / (float)idleFrame.size.x;
    spriteScaleY = 63.f / (float)idleFrame.size.y;  
    sprite->setScale({ facingRight ? -spriteScaleX : spriteScaleX, spriteScaleY });
}

void GamakichiCreature::die()
{
    if (isDying)
    {
        return;
    }
    isDying = true; 
    snowRolling = false;   
    snowCoated = false;
    isBombDeath = false;
    animFrame = 0;
    animTimer = 0.f;
    velX = 0;   // stops all movement
    velY = 0;
}

void GamakichiCreature::dieByBomb()
{
    if (isDying)
    {
        return;
    }
    isDying = true;
    isBombDeath = true;
    animFrame = 0;
    animTimer = 0.f;
    velX = 0;   // stops all movement
    velY = 0;
}

void GamakichiCreature::update(float dt, Platform* platforms, int platformCount)
{
    if (!alive)
    {
        return;
    }

    if (snowCoated && !snowRolling)
    {
        // applying gravity so it falls to the ground while coated
        velY += gravity * dt;
        enemyY += velY * dt;
        hitbox.update(enemyX, enemyY);

        // Floor
        if (enemyY + hitbox.getHeight() > windowH - 70)
        {
            enemyY = windowH - 70 - hitbox.getHeight();
            hitbox.update(enemyX, enemyY);
            velY = 0.f;
        }

        // Platforms
        if (velY >= 0.f)
        {
            for (int i = 0; i < platformCount; i++)
            {
                FloatRect pRect = platforms[i].getRect();
                FloatRect cRect = hitbox.getRect();
                bool overlapX = cRect.position.x + cRect.size.x > pRect.position.x &&
                                cRect.position.x < pRect.position.x + pRect.size.x;
                bool landingOnTop = cRect.position.y + cRect.size.y >= pRect.position.y &&
                                    cRect.position.y + cRect.size.y <= pRect.position.y + pRect.size.y + velY * dt + 5.f;
                if (overlapX && landingOnTop)
                {
                    enemyY = pRect.position.y - hitbox.getHeight();
                    hitbox.update(enemyX, enemyY);
                    velY = 0.f;
                    break;
                }
            }
        }

        updateSnow(dt, platforms, platformCount);
        updateSnowAnim(dt);
        updateSpritePos();
        return;
    }

    if (snowRolling) 
    {
        updateSnow(dt, platforms, platformCount);
        return;
    }

    // Partially snow-coated: it slows down and checks the uncoating timer
    if (snowLevel > 0 && !snowCoated) 
    {
        updateSnow(dt, platforms, platformCount);      // checks snowBreakTimer so enemy unfreezes after 3.5s
        updateSnowAnim(dt);  // keeps the snow animation playing
        velX *= 0.15f;       // reduce to 15% speed
    }

    if (isDying) 
    {
        animTimer += dt;
        if (animTimer >= 0.06f) // advance to next death frame after 0.06s
        {
            animTimer = 0.f;
            animFrame++;
            if (animFrame >= 5) 
            {
                alive = false;  // all death frames played
                return;
            }
            sprite->setTextureRect(isBombDeath ? bombDeathFrames[animFrame] : deathFrames[animFrame]);
            sprite->setScale({ facingRight ? -spriteScaleX : spriteScaleX, spriteScaleY });
        }
        updateSpritePos();
        return;
    }

    velY += gravity * dt;
    enemyX += velX * dt;
    enemyY += velY * dt;
    hitbox.update(enemyX, enemyY);;

    // Floor
    if (enemyY + hitbox.getHeight() > windowH - 70)
    {
        enemyY = windowH - 70 - hitbox.getHeight();
        hitbox.update(enemyX, enemyY);
        velY *= -0.45f;    // so that it bounces at 45% of impact speed
        if (fabs(velY) < 40.f) velY = 0.f;
    }

    // Platform collision only when falling down
    if (velY >= 0.f) 
    {
        for (int i = 0; i < platformCount; i++) 
        {
            FloatRect pRect = platforms[i].getRect();
            FloatRect cRect = hitbox.getRect();

            bool overlapX = cRect.position.x + cRect.size.x > pRect.position.x &&
                            cRect.position.x < pRect.position.x + pRect.size.x;
            bool landingOnTop = cRect.position.y + cRect.size.y >= pRect.position.y &&
                                cRect.position.y + cRect.size.y <= pRect.position.y + pRect.size.y + velY * dt + 5.f;

            if (overlapX && landingOnTop)
            {
                enemyY = pRect.position.y - hitbox.getHeight();
                hitbox.update(enemyX, enemyY);
                velY *= -0.45f;
                if (fabs(velY) < 40.f)
                {
                    velY = 0.f;
                }
                break;  // no neet to check more platfoems
            }
        }
    }

    bool onGround = (enemyY + hitbox.getHeight() >= windowH - 70);

    jumpTimer += dt;
    if (onGround && jumpTimer >= jumpInterval)
    {
        jumpTimer = 0.f;
        jumpInterval = 1.f + (rand() % 2);
        velY = -800.f;
        velX = facingRight ? 150.f : -150.f;
    }

    // Walls
    if (enemyX < 80.f) 
    {
        enemyX = 80.f;
        hitbox.update(enemyX, enemyY);
        velX = fabs(velX);      // now moving right
        facingRight = true;    
        sprite->setScale({ -spriteScaleX, spriteScaleY });
    }
    if (enemyX + hitbox.getWidth() > windowW - 80.f) 
    {
        enemyX = windowW - 80.f - hitbox.getWidth();
        velX = -fabs(velX);     // now moving left
        facingRight = false;
        sprite->setScale({ spriteScaleX, spriteScaleY });
    }

    animTimer += dt;
    if (animTimer >= 0.1f) 
    {
        animTimer = 0.f;
        animFrame++;
        if (fabs(velY) > 50.f)  // if jumping
        {
            animFrame %= 2;
            sprite->setTextureRect(jumpFrames[animFrame]);
        }
        else 
        {
            animFrame %= 3;
            sprite->setTextureRect(walkFrames[animFrame]);
        }
        sprite->setScale({ facingRight ? -spriteScaleX : spriteScaleX, spriteScaleY });
    }
    updateSpritePos();
}

void GamakichiCreature::draw(RenderWindow& window)
{
    if (!alive) return;

    if (snowCoated && !isMelting) 
    {
        drawSnowVisuals(window);
    }
    else 
    {
        window.draw(*sprite);
        if (snowLevel > 0 || isMelting) 
        {
            drawSnowVisuals(window);
        }
    }
    if (showHitbox) hitbox.draw(window, Color::Red);
}

void GamakichiCreature::updateSpritePos()
{
    if (facingRight)
    {
        sprite->setPosition({ enemyX + hitbox.getWidth(), enemyY });
    }
    else
    {
        sprite->setPosition({ enemyX, enemyY });
    }
}