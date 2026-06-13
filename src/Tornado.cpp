#include "Tornado.h"

Tornado::Tornado(float x, float y, float wW, float wH)
        : FlyingFoogaFoog(x, y, wW, wH)
{
    snowMaxLevel = 3;
    addSnowFrameCount = 3;
    removeSnowFrameCount = 3;

    knifeTimer = 0.f;  // counts time since last knife throw
    knifeInterval = 4.f;  // throws a knife after every 4 seconds
    currentKnife = nullptr; 
    flightSpeed = 180;

    isTornadoSpinning = false;
    tornadoSpinDone = false;
    spinFrameIndex = 0;
    spinTimer = 0.f;
    spinFrameDelay = 0.15f;  // advance frame after every 0.15 seconds
    spinCycleTimer = 0.f;  // used to trigger spinning
    isDying = false;

    health = 1;
    maxHealth = 1;

    currentAnim = ANIM_IDLE;

    knifeTexture.loadFromFile("SnowBrosAssets/Images/Tornado_Red.png");

    loadSprite();
    setupFrames();
}

void Tornado::setColor(int color)
{
    colorVariant = color;
    switch (color)
    {
    case COLOR_RED:  
        speedMultiplier = 1.0f;
        snowMaxLevel = 3;
        break;
    case COLOR_BLUE:
        speedMultiplier = 1.25f;
        snowMaxLevel = 4;
        break;
    }
    speed = 120.f * speedMultiplier;  
    loadSprite();
}

void Tornado::loadSprite()
{
    switch (colorVariant)
    {
    case COLOR_RED:
        texture.loadFromFile("SnowBrosAssets/Images/Tornado_Red.png");
        break;
    case COLOR_BLUE:
    default:
        texture.loadFromFile("SnowBrosAssets/Images/Tornado_Blue.png");
        break;
    }
    sprite = new Sprite(texture);
}

void Tornado::setupFrames()
{
    idleFrame = IntRect({ 15, 36 }, { 108, 125 });

    runFrames[0] = IntRect({ 141,  36 }, { 133, 125 });
    runFrames[1] = IntRect({ 280,  40 }, { 129, 121 });
    runFrames[2] = IntRect({ 418,  32 }, { 133, 129 });

    jumpFrame = IntRect({ 25,  189 }, { 100, 143 });
    fallFrame = IntRect({ 151, 202 }, { 114, 125 });
    landFrame = IntRect({ 289, 220 }, { 116, 103 });

    turnFrames[0] = IntRect({ 15,  707 }, { 121, 125 });
    turnFrames[1] = IntRect({ 145, 707 }, { 129, 125 });

    throwFrames[0] = IntRect({ 25,  880 }, { 114, 118 });
    throwFrames[1] = IntRect({ 141, 880 }, { 123, 114 });

    tornadoSpinFrames[0] = IntRect({ 19,  1043 }, { 100, 125 });
    tornadoSpinFrames[1] = IntRect({ 137, 1043 }, { 146, 125 });
    tornadoSpinFrames[2] = IntRect({ 300, 1043 }, { 100, 125 });
    tornadoSpinFrames[3] = IntRect({ 414, 1044 }, { 132, 117 });
    tornadoSpinFrames[4] = IntRect({ 554, 1047 }, { 86, 126 });
    tornadoSpinFrames[5] = IntRect({ 716, 1064 }, { 91, 116 });
    tornadoSpinFrames[6] = IntRect({ 884, 1047 }, { 91, 129 });
    tornadoSpinFrames[7] = IntRect({ 988, 1047 }, { 100, 108 });

    tornadoDieFrames[0] = IntRect({ 2, 1203 }, { 136, 132 });
    tornadoDieFrames[1] = IntRect({ 145, 1202 }, { 133, 144 });
    tornadoDieFrames[2] = IntRect({ 296, 1215 }, { 100, 125 });
    tornadoDieFrames[3] = IntRect({ 414, 1215 }, { 146, 125 });
    tornadoDieFrames[4] = IntRect({ 577, 1215 }, { 100, 125 });
    tornadoDieFrames[5] = IntRect({ 691, 1214 }, { 141, 120 });
    tornadoDieFrames[6] = IntRect({ 832, 1200 }, { 139, 128 });
    tornadoDieFrames[7] = IntRect({ 980, 1202 }, { 125, 133 });
    tornadoDieFrames[8] = IntRect({ 1110, 1202 }, { 137, 133 });

    addSnowFrames[0] = IntRect({ 154, 204 }, { 116, 129 });
    addSnowFrames[1] = IntRect({ 288, 220 }, { 121, 109 });

    removeSnowFrames[0] = IntRect({ 11,  372 }, { 125, 124 });
    removeSnowFrames[1] = IntRect({ 166, 372 }, { 108, 124 });
    removeSnowFrames[2] = IntRect({ 288, 372 }, { 104, 124 });

    sprite->setTextureRect(idleFrame);
    sprite->setPosition({ enemyX, enemyY });
    float scaleX = hitbox.getWidth() / idleFrame.size.x;
    float scaleY = hitbox.getHeight() / idleFrame.size.y;
    sprite->setScale({ scaleX, scaleY });
}

void Tornado::setState(int newState)
{
    if (currentAnim == newState)
    {
        return;
    }
    currentAnim = newState;
    currentFrame = 0;
    animTimer = 0.f;
    spinFrameIndex = 0;
    spinTimer = 0.f;
}

void Tornado::throwKnife()
{
    if (currentKnife != nullptr && currentKnife->isActive())
    {
        // only throws if previous knife is gone
        return;
    }
    delete currentKnife;

    float vX = facingRight ? 600.f : -600.f;
    currentKnife = new Knife( facingRight ? enemyX + hitbox.getWidth() : enemyX, enemyY + hitbox.getHeight() / 2, vX, 0.f, windowW, windowH, &knifeTexture);
}

void Tornado::updateKnife(float dt, Platform* platforms, int platformCount)
{
    if (currentKnife != nullptr && currentKnife->isActive())
    {
        currentKnife->update(dt, platforms, platformCount);
    }
}

void Tornado::die() 
{
    if (isDying || !alive)
    {
        return;
    }
    isDying = true;
    setState(ANIM_DIE);
    currentFrame = 0;
    animTimer = 0.f;
}

void Tornado::update(float dt, Platform* platforms, int platformCount)
{
    if (!alive)
    {
        return;
    }

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

    if (snowCoated && !snowRolling)
    {
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

    // Partially coated it can't spin or fly, it moves slowly on ground only
    if (snowLevel > 0 && !snowRolling) 
    {
        isTornadoSpinning = false;  // cancel spin
        isFlying = false;
        float slowSpeed = speed * 0.4f;
        velY += gravity * dt;
        enemyX += (facingRight ? slowSpeed : -slowSpeed) * dt;
        enemyY += velY * dt;
        onGround = false;
        hitbox.update(enemyX, enemyY);
        FloatRect er = hitbox.getRect();
        for (int i = 0; i < platformCount; i++) {
            FloatRect p = platforms[i].getRect();
            bool hO = er.position.x + er.size.x > p.position.x && er.position.x < p.position.x + p.size.x;
            bool vC = er.position.y + er.size.y >= p.position.y && er.position.y + er.size.y <= p.position.y + 40 && velY > 0;
            if (hO && vC)
            { 
                enemyY = p.position.y - hitbox.getHeight(); 
                velY = 0; 
                onGround = true; 
                hitbox.update(enemyX, enemyY);
            }
        }
        if (enemyX < 80) 
        {
            enemyX = 80; 
            facingRight = true; 
        }
        if (enemyX + hitbox.getWidth() > windowW - 80) 
        {
            enemyX = windowW - 80 - hitbox.getWidth(); 
            facingRight = false;
        }
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

    // Trigger spin every 5 seconds
    spinCycleTimer += dt;
    if (spinCycleTimer >= 5.f && !isTornadoSpinning)
    {
        isTornadoSpinning = true;
        spinFrameIndex = 0;
        spinTimer = 0.f;
        spinCycleTimer = 0.f;
        srand(time(0));
        int dir = rand() % 8;
        float dirs[8][2] = { {1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1} };
        flightDirX = dirs[dir][0];
        flightDirY = dirs[dir][1];
    }

    // Spinning, basically flying freely
    if (isTornadoSpinning)
    {
        spinCycleTimer += dt;

        spinTimer += dt;
        if (spinTimer >= spinFrameDelay)
        {
            spinTimer -= spinFrameDelay;  // takes leftover time for next frame
            spinFrameIndex = (spinFrameIndex + 1) % 8;
        }

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
        if (enemyY + hitbox.getHeight() > windowH - 55) 
        {
            enemyY = windowH - 55 - hitbox.getHeight();
            flightDirY = -flightDirY;
        }

        hitbox.update(enemyX, enemyY);

        IntRect frame = tornadoSpinFrames[spinFrameIndex];
        sprite->setTextureRect(frame);
        sprite->setScale({ hitbox.getWidth() / (float)frame.size.x,
                           hitbox.getHeight() / (float)frame.size.y });
        sprite->setPosition({ enemyX, enemyY });

        if (spinCycleTimer >= 3.f)  // spin lasts 3s
        {
            isTornadoSpinning = false;
            spinFrameIndex = 0;
            spinCycleTimer = 0.f;
        }
        return;
    }

    // Ground running with gravity
    velY += gravity * dt;
    enemyX += velX * dt;
    enemyY += velY * dt;

    onGround = false;
    hitbox.update(enemyX, enemyY);
    FloatRect enemyRect = hitbox.getRect();

    for (int i = 0; i < platformCount; i++)
    {
        FloatRect p = platforms[i].getRect();
        bool hOvlp = enemyRect.position.x + enemyRect.size.x > p.position.x &&
                     enemyRect.position.x < p.position.x + p.size.x;
        bool vCol = enemyRect.position.y + enemyRect.size.y >= p.position.y &&
                    enemyRect.position.y + enemyRect.size.y <= p.position.y + 40 &&
                    velY > 0;
        if (hOvlp && vCol)
        {
            enemyY = p.position.y - hitbox.getHeight();
            velY = 0;
            onGround = true;
            hitbox.update(enemyX, enemyY);
            enemyRect = hitbox.getRect();
        }
    }

    if (enemyX < 80) 
    { 
        enemyX = 80;            
        velX = speed; 
        facingRight = true; 
    }
    if (enemyX + hitbox.getWidth() > windowW - 80) 
    {
        enemyX = windowW - 80 - hitbox.getWidth(); 
        velX = -speed;
        facingRight = false; 
    }
    if (enemyY > windowH) 
    { 
        alive = false; 
        return;
    }

    updateSnowAnim(dt);

    if (snowLevel == snowMaxLevel && !isRolling)
    {
        velX = 0;
        sprite->setTextureRect(idleFrame);
        return;
    }

    if (snowLevel < snowMaxLevel)  // only throws if not fully coated
    {
        knifeTimer += dt;
        if (knifeTimer >= knifeInterval) 
        {
            knifeTimer = 0.f; 
            throwKnife();
        }
    }

    if (onGround)
    {
        velX = facingRight ? speed : -speed;
    }

    updateKnife(dt, platforms, platformCount);

    setState(onGround && velX != 0 ? ANIM_RUN : ANIM_IDLE);
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

void Tornado::updateAnimation(float dt)
{
    animTimer += dt;

    int total = 1;
    switch (currentAnim)
    {
    case ANIM_IDLE:
        total = 1;
        break;
    case ANIM_RUN:  
        total = 3;
        break;
    case ANIM_JUMP: 
        total = 1; 
        break;
    case ANIM_FALL:
        total = 1;
        break;
    case ANIM_LAND: 
        total = 1; 
        break;
    case ANIM_TURN: 
        total = 2; 
        break;
    case ANIM_THROW:
        total = 2; 
        break;
    case ANIM_SPIN: 
        total = 8;
        break;
    case ANIM_DIE:  
        total = 9; 
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
                currentFrame = total - 1; alive = false;
            }
        }
        else if (currentFrame >= total)
        {
            currentFrame = 0;
        }
    }

    switch (currentAnim)
    {
    case ANIM_IDLE: 
        sprite->setTextureRect(idleFrame);      
        break;
    case ANIM_RUN:   
        sprite->setTextureRect(runFrames[currentFrame]);      
        break;
    case ANIM_JUMP:  
        sprite->setTextureRect(jumpFrame);        
        break;
    case ANIM_FALL: 
        sprite->setTextureRect(fallFrame);       
        break;
    case ANIM_LAND: 
        sprite->setTextureRect(landFrame);             
        break;
    case ANIM_TURN:  
        sprite->setTextureRect(turnFrames[currentFrame]);     
        break;
    case ANIM_SPIN: 
        sprite->setTextureRect(tornadoSpinFrames[currentFrame]);   
        break;
    case ANIM_THROW: 
        sprite->setTextureRect(throwFrames[currentFrame]);    
        break;
    case ANIM_DIE:  
        sprite->setTextureRect(tornadoDieFrames[currentFrame]);    
        break;
    }
}

void Tornado::draw(RenderWindow& window)
{
    if (!alive) return;

    // Draw snow overlay if coated, else draw sprite + partial snow on top
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

    // Always draw knife on top
    if (currentKnife != nullptr && currentKnife->isActive())
    {
        currentKnife->draw(window);
    }
}

Tornado::~Tornado() 
{
    delete currentKnife; 
}