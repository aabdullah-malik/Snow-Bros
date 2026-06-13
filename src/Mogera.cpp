#include "Mogera.h"
#include "MogeraChild.h"

Mogera::Mogera(float x, float y, float wW, float wH)
       : Enemy(x, y, wW, wH, 280.f, 320.f)
{
    MogeraChild::preloadTexture();
    health = 35;
    maxHealth = 35;
    spawnTimer = 0.f; 
    spawnInterval = 3.f;

    jumpTimer = 0.f; 
    jumpInterval = 2.f;
    jumpPhaseTimer = 0.f;  // controls how often jumpFrameIndex updates
    isJumping = false; 
    jumpFrameIndex = 0;
    isDying = false;
    deathFrameIndex = 0; 
    deathTimer = 0.f;
    deathAnimSpeed = 0.4f;

    currentPlatform = 0;   // 0 = upper platform, 1 = floor
    targetPlatform = 1;    // first jump goes down to floor

    shouldRoar = false;
    hasRoared = false;

    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        children[i] = nullptr;
    }

    healthBarBg.setFillColor(Color(100, 0, 0));  // dark red
    healthBarFill.setFillColor(Color(0, 200, 0));  // green

    bodyState = 0;
    bodyStateTimer = 0.f;

    // scoreReward starts at 0 -- set to 5000 when health hits 0
    scoreReward = 0;

    loadSprite();
    setupFrames();
}

void Mogera::setColor(int color) {}

void Mogera::loadSprite()
{
    texture.loadFromFile("SnowBrosAssets/Images/Mogera.png");
    sprite = new Sprite(texture);
    legsSprite = new Sprite(texture);
}

void Mogera::setupFrames()
{
    bodySitting = IntRect({ 40, 0 }, { 542, 466 });
    bodyMouthOpen = IntRect({ 581, 0 }, { 545, 465 });
    bodyRoaring = IntRect({ 1157, 0 }, { 557, 475 });
    bodyDeathFrames[0] = IntRect({ 169, 520 }, { 686, 593 });
    bodyDeathFrames[1] = IntRect({ 1005, 752 }, { 784, 343 });

    legFrames[0] = IntRect({ 1977, 0 }, { 483, 112 });
    legFrames[1] = IntRect({ 1991, 206 }, { 476, 230 });
    legFrames[2] = IntRect({ 1972, 435 }, { 496, 338 });
}

void Mogera::startJump()
{
    isJumping = true;
    jumpFrameIndex = 1;
    jumpPhaseTimer = 0.f;

    if (currentPlatform == 0)  // upper to floor
    {
        targetPlatform = 1;
        velY = 50.f;  // small value as gravity also exists
    }
    else  // floor to upper
    {
        targetPlatform = 0;
        velY = -1200.f;  // needs a strong upward push here
    }
}

void Mogera::updateJump(float dt)
{
    if (!isJumping)
    {
        return;
    }

    velY += gravity * dt;  // gravity always pulling down
    enemyY += velY * dt;   // moving vertically

    jumpPhaseTimer += dt;  // updating leg frame based on vertical speed
    if (jumpPhaseTimer >= 0.1f)  // updates the frame after 0.1 s
    {
        jumpPhaseTimer = 0.f;
        if (velY < -100.f)
        {
            jumpFrameIndex = 1;  // going up fast
        }
        else if (velY > 100.f)
        {
            jumpFrameIndex = 2;  // going down fast
        }
        else
        {
            jumpFrameIndex = 0;
        }
    }

    float upperY = windowH - 345.f;
    float floorY = windowH - 55.f;

    // Landing on floor
    if (enemyY + hitbox.getHeight() >= floorY)
    {
        enemyY = floorY - hitbox.getHeight();
        velY = 0;
        isJumping = false;
        jumpFrameIndex = 0;
        currentPlatform = 1;
        hitbox.update(enemyX, enemyY);
        return;
    }

    // Touching the ceiling
    if (enemyY < 0) 
    {
        enemyY = 0; velY = 0; 
    }

    // Landing on upper platform
    if (targetPlatform == 0)
    {
        float landY = upperY - hitbox.getHeight();

        // Reached platform on way up
        bool landingUp = (enemyY <= landY && velY < 0);
        // Fell back down onto platform from above
        bool landingDown = (enemyY <= landY + 20.f && enemyY >= landY - 60.f && velY >= 0);

        if (landingUp || landingDown)
        {
            enemyY = landY;
            velY = 0;
            isJumping = false;
            jumpFrameIndex = 0;
            currentPlatform = 0;
        }
    }

    hitbox.update(enemyX, enemyY);
}

void Mogera::spawnChildrenBurst(int count)
{
    shouldRoar = true;  // telling game to play roar sound

    bodyState = 1;        // start mouth open
    bodyStateTimer = 0.f;

    int spawned = 0;
    for (int i = 0; i < MAX_CHILDREN && spawned < count; i++)
    {
        if (children[i] == nullptr || !children[i]->isAlive())
        {
            delete children[i];  // clean up dead slot

            // strong upward velocity so they can reach platforms
            float vX = -(150.f + rand() % 50);
            float vY = -(700.f + rand() % 400);
            srand(time(0));
            if (rand() % 5 == 0) vX = -vX;  // occasionally thrown right

            // spwn on Mogera's left side and 30% down its body
            float spawnX = enemyX - 30;
            float spawnY = enemyY + hitbox.getHeight() * 0.3f;

            children[i] = new MogeraChild(spawnX, spawnY, windowW, windowH, vX, vY);
            spawned++;
        }
    }
}

void Mogera::updateChildren(float dt, Platform* platforms, int platformCount)
{
    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        if (children[i] != nullptr && children[i]->isAlive())
        {
            children[i]->update(dt, platforms, platformCount);
        }
    }
}

void Mogera::drawChildren(RenderWindow& window)
{
    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        if (children[i] != nullptr && children[i]->isAlive())
        {
            children[i]->draw(window);
        }
    }
}

void Mogera::update(float dt, Platform* platforms, int platformCount)
{
    if (!alive)
    {
        return;
    }
    if (snowCoated || snowRolling)
    {
        return; // bosses can't be coated
    }

    // Death
    if (isDying)
    {
        deathTimer += dt;
        sprite->setColor(Color(180, 180, 180));

        if (deathTimer >= deathAnimSpeed)  // every 0.4s
        {
            deathFrameIndex++;
            if (deathFrameIndex >= 2) 
            {
                alive = false; 
                return; 
            }
        }

        velY += gravity * dt;  // applying gravity so that body falls to floor
        enemyY += velY * dt;
        if (enemyY + hitbox.getHeight() > windowH - 70)
        {
            enemyY = windowH - 70 - hitbox.getHeight();
            velY = 0;
        }
        hitbox.update(enemyX, enemyY);

        float s = hitbox.getWidth() / (float)bodyDeathFrames[deathFrameIndex].size.x;
        sprite->setTextureRect(bodyDeathFrames[deathFrameIndex]);
        sprite->setScale({ s, s });
        sprite->setPosition({ enemyX, enemyY });
        legsSprite->setScale({ 0, 0 });  // as they are already in the dead frames so need for seperate legs
        return;
    }

    // Jumping
    jumpTimer += dt;
    if (jumpTimer >= jumpInterval && !isJumping)
    {
        jumpTimer = 0.f;
        startJump();
    }
    updateJump(dt);

    // Spawn children 
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval)
    {
        spawnTimer = 0.f;
        spawnChildrenBurst(2 + rand() % 2);  // randomly spawns 2 or 3 children
    }

    updateChildren(dt, platforms, platformCount);

    // Stay on position when idle
    if (!isJumping)
    {
        float upperY = windowH - 345.f;
        float floorY = windowH - 70.f;
        float platY = (currentPlatform == 0) ? upperY : floorY;
        enemyY = platY - hitbox.getHeight();
        hitbox.update(enemyX, enemyY);
    }

    // BODY + LEGS POSITIONING 
    float scale = hitbox.getWidth() / (float)bodySitting.size.x;
    float bodyH = bodySitting.size.y * scale;
    float legH = legFrames[jumpFrameIndex].size.y * scale;
    float overlap = 10.f;

    float totalH = bodyH + legH - overlap;
    if (totalH < hitbox.getHeight())  // checks if total fits inside hitbox height (320)
    {
        float extra = (hitbox.getHeight() - totalH) / 2.f;  // divided by 2 to equally distribute between legs and height
        bodyH += extra;
        legH += extra;
    }

    // BODY FRAME SELECTION
    if (bodyState == 1)   // mouth open
    {
        bodyStateTimer += dt;
        sprite->setTextureRect(bodyMouthOpen);
        sprite->setScale({ scale, bodyH / (float)bodyMouthOpen.size.y });
        sprite->setPosition({ enemyX, enemyY });
        if (bodyStateTimer >= 0.3f) 
        {
            bodyState = 2;   // transition to roaring
            bodyStateTimer = 0.f;
        }
    }
    else if (bodyState == 2) // roaring
    {
        bodyStateTimer += dt;
        sprite->setTextureRect(bodyRoaring);
        sprite->setScale({ scale, bodyH / (float)bodyRoaring.size.y });
        sprite->setPosition({ enemyX, enemyY });
        if (bodyStateTimer >= 0.6f) 
        {
            bodyState = 0;   // back to sitting
            bodyStateTimer = 0.f;
        }
    }
    else  // sitting
    {
        sprite->setTextureRect(bodySitting);
        sprite->setScale({ scale, bodyH / (float)bodySitting.size.y });
        sprite->setPosition({ enemyX, enemyY });
    }

    float legScaleY = legH / (float)legFrames[jumpFrameIndex].size.y;
    float legW = legFrames[jumpFrameIndex].size.x * scale;
    float legX = enemyX + (hitbox.getWidth() - legW) / 2.f;  // centrils legs horixontally inside hitbox
    float legY = enemyY + hitbox.getHeight() - legH;

    legsSprite->setTextureRect(legFrames[jumpFrameIndex]);
    legsSprite->setScale({ scale, legScaleY });
    legsSprite->setPosition({ legX, legY });

    // Health bar
    float pct = (float)health / maxHealth;
    healthBarBg.setSize({ hitbox.getWidth(), 10 });
    healthBarBg.setPosition({ enemyX, enemyY - 15 });
    healthBarFill.setSize({ hitbox.getWidth() * pct, 10 });  // shrinks as health decreases
    healthBarFill.setPosition({ enemyX, enemyY - 15 });

    for (int i = 0; i < 10; i++)   // so that when children die, new one can come at their place
    {
        if (children[i] != nullptr && !children[i]->isAlive()) 
        {
            delete children[i];
            children[i] = nullptr;
        }
    }
}

bool Mogera::getShouldRoar()
{
    bool r = shouldRoar;
    shouldRoar = false;
    return r;
}

void Mogera::die()
{
    if (health <= 0)
    {
        isDying = true;
    }
}

void Mogera::onHitByRollingSnowball() { takeDamage(2); }

void Mogera::draw(RenderWindow& window)
{
    if (!alive)
    {
        return;
    }
    if (!isDying)
    {
        window.draw(*legsSprite);  // legs sprite only drawn when mogera is alive
    }

    window.draw(*sprite);

    if (!isDying)
    {
        window.draw(healthBarBg);
        window.draw(healthBarFill);
    }
    drawChildren(window);
}

int Mogera::getCreatureCount() const
{
    int count = 0;
    for (int i = 0; i < 10; i++)
    {
        if (children[i] != nullptr && children[i]->isAlive())
        {
            count++;
        }
    }
    return count;
}

void Mogera::addSnow(int amount)
{
    snowLevel = 0;        // never let snow accumulate
    snowCoated = false;   // never let it get coated
    takeDamage(2);
    if (health <= 0 && scoreReward == 0)
        scoreReward = 5000;
}

MogeraChild* Mogera::getCreature(int index) const
{
    if (index < 0 || index >= 10)
    {
        return nullptr;
    }
    return children[index];
}

Mogera::~Mogera()
{
    delete legsSprite;
    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        delete children[i];
    }
}