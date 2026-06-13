#include "Botom.h"

Botom::Botom(float x, float y, float wW, float wH) : Enemy(x, y, wW, wH, 60.f, 80.f)
{
    snowMaxLevel = 2;   // encased by 2 hits
    addSnowFrameCount = 2;   // 2 getting trapped frames
    removeSnowFrameCount = 3;   // 3 unleashing frames
    currentAnim = ANIM_IDLE;
    isDying = false;
    dirChangeTimer = 0.f;
    dirChangeInterval = 2.f;  // change direction every 2 seconds
    health = 1;
    maxHealth = 1;

    loadSprite();
    setupFrames();

    speed = 90.f;   
    velX = -speed;  // start moving left
}

void Botom::loadSprite()
{
    // loads color variant file
    switch (colorVariant)
    {
    case COLOR_RED:
        texture.loadFromFile("SnowBrosAssets/Images/Botom_Pink.png");
        break;
    case COLOR_ORANGE:
        texture.loadFromFile("SnowBrosAssets/Images/Botom_Orange.png");
        break;
    case COLOR_BLUE:
        texture.loadFromFile("SnowBrosAssets/Images/Botom_Blue.png");
        break;
    default:
        texture.loadFromFile("SnowBrosAssets/Images/Botom_Pink.png");
        break;
    }
    sprite = new Sprite(texture);
}

void Botom::setupFrames()
{
    idleFrame = IntRect({ 9, 138 }, { 85, 81 });

    chargeFrames[0] = IntRect({ 0, 250 }, { 95, 86 });
    chargeFrames[1] = IntRect({ 97, 254 }, { 93, 79 });

    walkFrames[0] = IntRect({ 1, 367 }, { 94, 80 });
    walkFrames[1] = IntRect({ 96, 363 }, { 90, 84 });
    walkFrames[2] = IntRect({ 188, 366 }, { 92, 81 });

    jumpFrame = IntRect({ 6, 479 }, { 86, 92 });
    fallFrame = IntRect({ 5, 594 }, { 85, 92 });
    stareFrame = IntRect({ 1, 937 }, { 93, 80 });
    aaFrame = IntRect({ 7, 1163 }, { 87, 84 });

    // snow coat frames
    addSnowFrames[0] = IntRect({ 6, 712 }, { 86, 76 });
    addSnowFrames[1] = IntRect({ 99, 709 }, { 89, 79 });

    removeSnowFrames[0] = IntRect({ 1, 821 }, { 88, 83 });
    removeSnowFrames[1] = IntRect({ 99, 823 }, { 87, 82 });
    removeSnowFrames[2] = IntRect({ 194, 821 }, { 87, 83 });

    // die frames
    dieFrames[0] = IntRect({ 1, 1048 }, { 92, 94 });
    dieFrames[1] = IntRect({ 99, 1048 }, { 85, 81 });
    dieFrames[2] = IntRect({ 190, 1050 }, { 85, 91 });
    dieFrames[3] = IntRect({ 287, 1050 }, { 91, 86 });
    dieFrames[4] = IntRect({ 387, 1053 }, { 85, 91 });
    dieFrames[5] = IntRect({ 473, 1059 }, { 91, 85 });
    dieFrames[6] = IntRect({ 571, 1050 }, { 87, 84 });
    dieFrames[7] = IntRect({ 664, 1069 }, { 90, 66 });
    dieFrames[8] = IntRect({ 755, 1091 }, { 92, 41 });

    // initialize sprite
    sprite->setTextureRect(idleFrame);
    sprite->setPosition({ enemyX, enemyY });

    float scaleX = hitbox.getWidth() / idleFrame.size.x;
    float scaleY = hitbox.getHeight() / idleFrame.size.y;
    sprite->setScale({ scaleX, scaleY });
}

void Botom::setState(int newState)
{
    if (currentAnim == newState)
    {
        return;
    }
    currentAnim = newState;
    currentFrame = 0;
    animTimer = 0.f;
}

void Botom::die() {
    if (isDying || !alive)  // if already dying then its not played
    {
        return;
    }
    isDying = true;
    setState(ANIM_DIE);
    currentFrame = 0;
    animTimer = 0.f;
}

void Botom::update(float dt, Platform* platforms, int platformCount)
{
    if (!alive) return;

    if (isDying)
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
        return;
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

    // Partially coated: stop moving, wait for snow to clear
    if (snowLevel > 0 && !snowRolling) {
        velX = 0;
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

    if (snowRolling) {
        updateSnow(dt, platforms, platformCount);  // rolling physics only
        return;
    }

    applyGravityAndMove(dt, platforms, platformCount);
    updateSnowAnim(dt);

    if (isDying)
    {
        setState(ANIM_DIE);
        updateAnimation(dt);
        return;
    }

    if (isRolling)
    {
        setState(ANIM_AA);
        updateAnimation(dt);
        return;
    }

    if (snowLevel == snowMaxLevel)
    {
        velX = 0;
        return;
    }

    if (onGround)
    {
        dirChangeTimer += dt;
        if (dirChangeTimer >= dirChangeInterval)
        {
            dirChangeTimer = 0.f;
            dirChangeInterval = 1.f + (rand() % 3);
            facingRight = !facingRight;

            // Random chance to jump 
            if (rand() % 4 == 0)
            {
                velY = -900.f;
                onGround = false;
            }
        }

        velX = facingRight ? speed : -speed;
        setState(ANIM_WALK);
    }
    else
    {
        if (velY < 0)
        {
            setState(ANIM_JUMP);
        }
        else
        {
            setState(ANIM_FALL);
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

void Botom::setColor(int color)
{
    colorVariant = color;

    switch (color)
    {
    case COLOR_RED:
        speedMultiplier = 1.0f;
        snowMaxLevel = 2;
        break;

    case COLOR_ORANGE:
        speedMultiplier = 1.1f;
        snowMaxLevel = 3;
        break;

    case COLOR_BLUE:
        speedMultiplier = 1.2f;
        snowMaxLevel = 4;
        break;
    }

    speed = 90.f * speedMultiplier;   

    loadSprite();  // reload texture for this color
}

void Botom::updateAnimation(float dt)
{
    animTimer += dt;

    int totalFrames = 1;
    switch (currentAnim)
    {
    case ANIM_IDLE: 
        totalFrames = 1; 
        break;
    case ANIM_CHARGE:
        totalFrames = 2; 
        break;
    case ANIM_WALK:  
        totalFrames = 3; 
        break;
    case ANIM_JUMP:   
        totalFrames = 1; 
        break;
    case ANIM_FALL:   
        totalFrames = 1;
        break;
    case ANIM_DIE:   
        totalFrames = 9;
        break;
    case ANIM_AA:    
        totalFrames = 1; 
        break;
    case ANIM_STARE:  
        totalFrames = 1; 
        break;
    }

    if (animTimer >= animSpeed)
    {
        animTimer = 0.f;
        currentFrame++;

        if (currentAnim == ANIM_DIE)
        {
            if (currentFrame >= totalFrames)
            {
                currentFrame = totalFrames - 1;
                alive = false;  // fully dead after animation
            }
        }
        else
        {
            if (currentFrame >= totalFrames)
                currentFrame = 0;
        }
    }

    switch (currentAnim)
    {
    case ANIM_IDLE:   
        sprite->setTextureRect(idleFrame);     
        break;
    case ANIM_CHARGE:
        sprite->setTextureRect(chargeFrames[currentFrame]); 
        break;
    case ANIM_WALK:  
        sprite->setTextureRect(walkFrames[currentFrame]);  
        break;
    case ANIM_JUMP:  
        sprite->setTextureRect(jumpFrame);   
        break;
    case ANIM_FALL: 
        sprite->setTextureRect(fallFrame); 
        break;
    case ANIM_DIE:    
        sprite->setTextureRect(dieFrames[currentFrame]);  
        break;
    case ANIM_AA:    
        sprite->setTextureRect(aaFrame);      
        break;
    case ANIM_STARE:
        sprite->setTextureRect(stareFrame);  
        break;
    }
}

void Botom::draw(RenderWindow& window)
{
    if (!alive)
    {
        return;
    }
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