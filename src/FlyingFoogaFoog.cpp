#include "FlyingFoogaFoog.h"

FlyingFoogaFoog::FlyingFoogaFoog(float x, float y, float wW, float wH) : Botom(x, y, wW, wH)
{
    snowMaxLevel = 3;
    addSnowFrameCount = 3;
    removeSnowFrameCount = 0;

    isFlying = false;
    groundTimer = 0.f;
    groundInterval = 3.f;
    flightTimer = 0.f;
    flightDuration = 4.f;
    flightDirX = 0.f;
    flightDirY = 0.f;
    flightSpeed = 180.f;  

    health = 1;
    maxHealth = 1;

    isSpinning = false;
    spinAngle = 0.f;
    spinSpeed = 360.f;
    spinDone = false;

    currentAnim = ANIM_IDLE;

    loadSprite();
    setupFrames();
}

void FlyingFoogaFoog::loadSprite()
{
    switch (colorVariant)
    {
    case COLOR_RED:
        texture.loadFromFile("SnowBrosAssets/Images/FlyingFoogaFoog_Red.png");
        break;
    case COLOR_ORANGE:
        texture.loadFromFile("SnowBrosAssets/Images/FlyingFoogaFoog_Orange.png");
        break;
    case COLOR_BLUE:
        texture.loadFromFile("SnowBrosAssets/Images/FlyingFoogaFoog_Blue.png");
        break;
    default:
        texture.loadFromFile("SnowBrosAssets/Images/FlyingFoogaFoog_Red.png");
        break;
    }
    sprite = new Sprite(texture);
}

void FlyingFoogaFoog::setupFrames()
{
    idleFrame = IntRect({ 52, 28 }, { 102, 166 });

    flyFrames[0] = IntRect({ 215, 18 }, { 171, 171 });
    flyFrames[1] = IntRect({ 426, 18 }, { 147, 166 });

    addSnowFrames[0] = IntRect({ 38,  234 }, { 141, 147 });
    addSnowFrames[1] = IntRect({ 229, 254 }, { 142, 127 });
    addSnowFrames[2] = IntRect({ 407, 220 }, { 156, 161 });

    angryFrames[0] = IntRect({ 23,  426 }, { 161, 137 });
    angryFrames[1] = IntRect({ 215, 426 }, { 161, 132 });

    spinFrame = IntRect({ 43, 805 }, { 146, 166 });

    fffDieFrames[0] = IntRect({ 18,  603 }, { 171, 152 });
    fffDieFrames[1] = IntRect({ 220, 633 }, { 161, 122 });
    fffDieFrames[2] = IntRect({ 412, 658 }, { 151,  97 });

    sprite->setTextureRect(idleFrame);
    sprite->setPosition({ enemyX, enemyY });
    float scaleX = hitbox.getWidth() / idleFrame.size.x;
    float scaleY = hitbox.getHeight() / idleFrame.size.y;
    sprite->setScale({ scaleX, scaleY });
}

void FlyingFoogaFoog::setState(int newState)
{
    if (currentAnim == newState)
    {
        return;
    }
    currentAnim = newState;
    currentFrame = 0;
    animTimer = 0.f;
}


void FlyingFoogaFoog::die() 
{
    if (isSpinning || spinDone || !alive)
    {
        return;
    }
    isSpinning = true;
    spinAngle = 0.f;
    spinDone = false;
    sprite->setRotation(sf::degrees(0.f));
}

void FlyingFoogaFoog::update(float dt, Platform* platforms, int platformCount)
{
    if (!alive)
    {
        return;
    }

    if (isSpinning || spinDone)
    {
        if (isSpinning)
        {
            spinAngle += spinSpeed * dt;
            sprite->setRotation(sf::degrees(spinAngle));
            if (spinAngle >= 360.f)
            {
                spinAngle = 0.f;
                isSpinning = false;
                spinDone = true;
                sprite->setRotation(sf::degrees(0.f));
            }
            return;
        }
        if (spinDone)
        {
            setState(ANIM_DIE);
            updateAnimation(dt);
            if (facingRight)
            {
                sprite->setScale({ -(hitbox.getWidth() / sprite->getTextureRect().size.x),
                                     hitbox.getHeight() / sprite->getTextureRect().size.y });
                sprite->setPosition({ enemyX + hitbox.getWidth(), enemyY });
            }
            else
            {
                sprite->setScale({ hitbox.getWidth() / sprite->getTextureRect().size.x,
                                    hitbox.getHeight() / sprite->getTextureRect().size.y });
                sprite->setPosition({ enemyX, enemyY });
            }
            updateSnow(dt, platforms, platformCount);
            return;
        }
    }

    if (snowCoated && !snowRolling) {
        updateSnow(dt, platforms, platformCount);
        updateSnowAnim(dt);
        if (facingRight)
        {
            sprite->setScale({ -(hitbox.getWidth() / sprite->getTextureRect().size.x),
                                 hitbox.getHeight() / sprite->getTextureRect().size.y });
            sprite->setPosition({ enemyX + hitbox.getWidth(), enemyY });
        }
        else
        {
            sprite->setScale({ hitbox.getWidth() / sprite->getTextureRect().size.x,
                                hitbox.getHeight() / sprite->getTextureRect().size.y });
            sprite->setPosition({ enemyX, enemyY });
        }
        return;
    }

    // Partially coated: it can't fly, only moves slowly on ground
    if (snowLevel > 0 && !snowRolling) {
        isFlying = false;  // force down from flight
        float slowSpeed = speed * 0.4f;
        applyGravityAndMove(dt, platforms, platformCount);
        velX = facingRight ? slowSpeed : -slowSpeed;
        updateSnow(dt, platforms, platformCount);
        updateSnowAnim(dt);
        setState(ANIM_IDLE);
        updateAnimation(dt);
        if (facingRight)
        {
            sprite->setScale({ -(hitbox.getWidth() / sprite->getTextureRect().size.x),
                                 hitbox.getHeight() / sprite->getTextureRect().size.y });
            sprite->setPosition({ enemyX + hitbox.getWidth(), enemyY });
        }
        else
        {
            sprite->setScale({ hitbox.getWidth() / sprite->getTextureRect().size.x,
                                hitbox.getHeight() / sprite->getTextureRect().size.y });
            sprite->setPosition({ enemyX, enemyY });
        }
        return;
    }

    if (snowRolling) 
    {
        updateSnow(dt, platforms, platformCount); 
        return; 
    }


    // Flying mode
    if (isFlying)
    {
        flightTimer += dt;
        enemyX += flightDirX * flightSpeed * dt;
        enemyY += flightDirY * flightSpeed * dt;

        if (enemyX < 80)
        {
            enemyX = 80;              
            flightDirX = -flightDirX; 
        }
        if (enemyX + hitbox.getWidth() > windowW - 80) 
        { 
            enemyX = windowW - 80 - hitbox.getWidth(); 
            flightDirX = -flightDirX; 
        }
        if (enemyY < 0)
        {
            enemyY = 0;    
            flightDirY = -flightDirY;
        }
        if (enemyY + hitbox.getHeight() > windowH - 60) 
        {
            enemyY = windowH - 60 - hitbox.getHeight(); 
            flightDirY = -flightDirY;
        }

        hitbox.update(enemyX, enemyY);
        setState(ANIM_FLY);

        // flies in all 8 directions
        if (flightTimer >= flightDuration)
        {
            isFlying = false;
            flightTimer = 0.f;
            groundTimer = 0.f;
            int dir = rand() % 8;
            float dirs[8][2] = { {1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1} };
            flightDirX = dirs[dir][0];
            flightDirY = dirs[dir][1];
        }
    }
    else  // not flying
    {
        applyGravityAndMove(dt, platforms, platformCount);
        updateSnowAnim(dt);

        if (snowLevel == snowMaxLevel)
        {
            velX = 0;
            setState(ANIM_IDLE);
        }
        else
        {
            groundTimer += dt;
            if (groundTimer >= groundInterval)
            {
                isFlying = true;
                flightTimer = 0.f;
                groundTimer = 0.f;
                velX = 0; velY = 0;
            }
            else
            {
                velX = facingRight ? speed : -speed;
                setState(ANIM_IDLE);
            }
        }
    }

    updateAnimation(dt);

    if (facingRight)
    {
        sprite->setScale({ -(hitbox.getWidth() / sprite->getTextureRect().size.x),
                             hitbox.getHeight() / sprite->getTextureRect().size.y });
        sprite->setPosition({ enemyX + hitbox.getWidth(), enemyY });
    }
    else
    {
        sprite->setScale({ hitbox.getWidth() / sprite->getTextureRect().size.x,
                            hitbox.getHeight() / sprite->getTextureRect().size.y });
        sprite->setPosition({ enemyX, enemyY });
    }
    updateSnow(dt, platforms, platformCount);
}

void FlyingFoogaFoog::updateAnimation(float dt)
{
    animTimer += dt;

    int total = 1;
    switch (currentAnim)
    {
    case ANIM_IDLE:  
        total = 1; 
        break;
    case ANIM_FLY:  
        total = 2; 
        break;
    case ANIM_HIT:  
        total = 3; 
        break;
    case ANIM_ANGRY: 
        total = 2; 
        break;
    case ANIM_SPIN: 
        total = 1;
        break;
    case ANIM_DIE: 
        total = 3;
        break;
    }

    if (animTimer >= animSpeed)
    {
        animTimer = 0.f;
        currentFrame++;
        if (currentAnim == ANIM_DIE)
        {
            if (currentFrame >= total) 
            {
                currentFrame = total - 1; 
                alive = false;
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
    case ANIM_FLY:  
        sprite->setTextureRect(flyFrames[currentFrame]);    
        break;
    case ANIM_HIT: 
        sprite->setTextureRect(addSnowFrames[currentFrame]);
        break;
    case ANIM_ANGRY: 
        sprite->setTextureRect(angryFrames[currentFrame]); 
        break;
    case ANIM_SPIN: 
        sprite->setTextureRect(spinFrame);      
        break;
    case ANIM_DIE: 
        sprite->setTextureRect(fffDieFrames[currentFrame]); 
        break;
    }
}

void FlyingFoogaFoog::setColor(int color)
{
    colorVariant = color;
    switch (color)
    {
    case COLOR_RED:   
        speedMultiplier = 1.0f;  
        snowMaxLevel = 3; 
        break;
    case COLOR_ORANGE:
        speedMultiplier = 1.1f;
        snowMaxLevel = 4; 
        break;
    case COLOR_BLUE:   
        speedMultiplier = 1.2f; 
        snowMaxLevel = 5;
        break;
    }
    speed = 120.f * speedMultiplier;  
    loadSprite();
}

void FlyingFoogaFoog::draw(RenderWindow& window)
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
}