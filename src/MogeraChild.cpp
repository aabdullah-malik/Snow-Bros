#include "MogeraChild.h"

bool MogeraChild::showHitbox = false;
Texture MogeraChild::childTexture;
bool MogeraChild::textureLoaded = false;

MogeraChild::MogeraChild(float x, float y, float wW, float wH, float vX, float vY) : Enemy(x, y, wW, wH, 70.f, 70.f)
{
    velX = vX; velY = vY;
    gravity = 1200.f;
    animFrame = 0;
    childAnimTimer = 0.f;
    childAnimSpeed = 0.08f;
    alive = true;
    snowMaxLevel = 4;

    loadSprite();
    setupFrames();

    jumpTimer = 0.f;
    jumpInterval = 2.5f + (rand() % 3);
}

void MogeraChild::preloadTexture()
{
    if (!textureLoaded)
    {
        childTexture.loadFromFile("SnowBrosAssets/Images/Mogera.png");
        textureLoaded = true;
    }
}

void MogeraChild::setColor(int color) {}

void MogeraChild::loadSprite()
{
    if (!textureLoaded)
    {
        childTexture.loadFromFile("SnowBrosAssets/Images/Mogera.png");
        textureLoaded = true;
    }
    sprite = new Sprite(childTexture);
}

void MogeraChild::setupFrames()
{
    walkFrames[0] = IntRect({ 1792, 799 }, { 174, 176 });
    walkFrames[1] = IntRect({ 1964, 798 }, { 172, 177 });
    walkFrames[2] = IntRect({ 2135, 777 }, { 179, 196 });
    airFrames[0] = IntRect({ 2174, 993 }, { 134, 155 });
    airFrames[1] = IntRect({ 1983, 984 }, { 143, 159 });
    airFrames[2] = IntRect({ 1791, 980 }, { 144, 163 });
    airFrames[3] = IntRect({ 2310, 803 }, { 157, 171 });

    sprite->setTextureRect(airFrames[0]);  // initial frame set to airFrames[0] because children always spawn mid-air
    float s = hitbox.getWidth() / (float)airFrames[0].size.x;
    float scaleX = (velX > 0) ? -s : s;
    sprite->setScale({ scaleX, s });
}

void MogeraChild::die()
{
    if (isDying || !alive)
    {
        return;
    }
    isDying = true;
    animFrame = 0;
    animTimer = 0.f;
    velX = 0;
    velY = 0;
}

void MogeraChild::update(float dt, Platform* platforms, int platformCount)
{
    if (!alive) return;

    // ~0.4 s then set alive = false so Mogera's cleanup loop removes it
    if (isDying)
    {
        animTimer += dt;
        if (animTimer >= 0.4f)
        {
            alive = false;
        }
        return;
    }

    // SNOW STATES
    // 2 seperate conditions here as the updateSnow function in enemy.cpp has 2 different states
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
                bool overlapX = cRect.position.x + cRect.size.x > pRect.position.x &&  // child right edge must be past the platform's left edge
                                cRect.position.x < pRect.position.x + pRect.size.x;  // child left edge must be before the platform's right edge
                bool landingOnTop = cRect.position.y + cRect.size.y >= pRect.position.y &&  // child bottom must have reached or passed the platform's top 
                                    cRect.position.y + cRect.size.y <= pRect.position.y + pRect.size.y + velY * dt + 5.f;  // childbottom must not have gone too far past the platform's bottom
                if (overlapX && landingOnTop)
                {
                    enemyY = pRect.position.y - hitbox.getHeight();
                    hitbox.update(enemyX, enemyY);
                    velY = -velY * 0.45f;
                    if (fabs(velY) < 60.f)
                    {
                        velY = 0.f;
                        onGround = true;
                    }
                    break;
                }
            }
        }

        updateSnow(dt, platforms, platformCount);
        updateSnowAnim(dt);
        if (facingRight)
        {
            sprite->setPosition({ enemyX + hitbox.getWidth(), enemyY });
        }
        else
        {
            sprite->setPosition({ enemyX, enemyY });
        }
        return;
    }
    if (snowRolling)
    {
        updateSnow(dt, platforms, platformCount);
        return;
    }
    // Partially snow-coated: it slows down 
    // Runs before normal movement so speed reduction applies immediately.
    if (snowLevel > 0 && !snowCoated)
    {
        updateSnow(dt, platforms, platformCount);      // checks snowBreakTimer so enemy unfreezes after 3.5s
        updateSnowAnim(dt);  // keeps the snow animation playing
        velX *= 0.15f;       // reduce speed to 15% 
        velY = 0;  // stop jumping
    }

    // NORMAL MOVEMENT 
    velY += gravity * dt;
    enemyX += velX * dt;
    enemyY += velY * dt;
    hitbox.update(enemyX, enemyY);

    bool onGround = false;

    // Floor
    if (enemyY + hitbox.getHeight() > windowH - 70)
    {
        enemyY = windowH - 70 - hitbox.getHeight();
        hitbox.update(enemyX, enemyY);
        velY = -velY * 0.45f;  // so that it bounces at 45% of impact speed
        if (fabs(velY) < 60.f)
        {
            velY = 0.f;
            onGround = true;
        }
    }

    // Platforms collision when falling
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
                velY = -velY * 0.45f;
                if (fabs(velY) < 60.f)
                {
                    velY = 0.f;
                    onGround = true;
                }
                break;
            }
        }
    }

    // Ceiling
    if (enemyY < 0)
    {
        enemyY = 0;
        velY = fabs(velY) * 0.5f;  // bounce downward at 50% speed
    }

    // Walls
    if (enemyX < 80)
    {
        enemyX = 80;
        velX = fabs(velX);  // forced positive = moving right
        facingRight = true;
    }
    if (enemyX + hitbox.getWidth() > windowW - 80)
    {
        enemyX = windowW - 80 - hitbox.getWidth();
        velX = -fabs(velX);  // forced negative = moving left
        facingRight = false;
    }

    // Jumping
    jumpTimer += dt;
    if (onGround && jumpTimer >= jumpInterval)
    {
        jumpTimer = 0.f;
        srand(time(0));
        jumpInterval = 1.f + (rand() % 3);  // next jump in 1-3 seconds randomly
        velY = -800.f;  // strong upward launch
        velX = facingRight ? 150.f : -150.f;  // horizontal push in facing direction
    }

    // setting facing direction
    if (velX > 0)
    {
        facingRight = true;
    }
    else if (velX < 0)
    {
        facingRight = false;
    }

    // Animation
    bool inAir = fabs(velY) > 60.f;  // in air if moving vertically faster than 60
    childAnimTimer += dt;
    if (childAnimTimer >= childAnimSpeed)  // every 0.08s
    {
        childAnimTimer = 0.f;
        animFrame++;
        if (inAir)
        {
            animFrame %= 4;  // keeps frame in range 0-3
            sprite->setTextureRect(airFrames[animFrame]);
        }
        else
        {
            animFrame %= 3;  // keeps frame in range 0-2
            sprite->setTextureRect(walkFrames[animFrame]);
        }
        IntRect cr = sprite->getTextureRect();
        float s = hitbox.getWidth() / (float)cr.size.x;
        sprite->setScale({ facingRight ? -s : s, s });
    }

    if (facingRight)
    {
        sprite->setPosition({ enemyX + hitbox.getWidth(), enemyY });
    }
    else
    {
        sprite->setPosition({ enemyX, enemyY });
    }
}

// DRAW
void MogeraChild::draw(RenderWindow& window)
{
    if (!alive)
    {
        return;
    }

    if (snowCoated && !isMelting)
    {
        drawSnowVisuals(window);       // fully coated: show snowball only
    }
    else
    {
        window.draw(*sprite);          // normal sprite
        if (snowLevel > 0 || isMelting)
        {
            drawSnowVisuals(window);   // partial snow overlay
        }
    }

    if (showHitbox)
    {
        hitbox.draw(window, Color::Red);
    }
}

void MogeraChild::drawDebug(RenderWindow& window)
{
    if (showHitbox) hitbox.draw(window, Color::Red);
}