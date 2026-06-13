#include "Gamakichi.h"
#include "GamakichiBomb.h"
#include "GamakichiCreature.h"

Gamakichi::Gamakichi(float wW, float wH) : Enemy(wW / 2.f - 150.f, wH + 100.f, wW, wH, 300.f, 280.f)
{
    GamakichiCreature::preloadTexture();
    GamakichiBomb::preloadTexture();
    
    state = STATE_ENTERING;
    stateTimer = 0.f;
    fireFrame = 0;
    fireAnimTimer = 0.f;
    firePosFrame = 0;

    lightningTimer = 0.f;  // used to alternate between the 2 lightning frames rapidly
     
    positionTop = windowH * 0.25f;      // upper position
    positionBottom = windowH - 380.f;  // lower position (current targetY)
    moveSpeed = 200.f;   // pixels per second
    dwellTime = 3.f;     // seconds at each position

    health = 50;
    maxHealth = 50;
    isDying = false;

    shouldRoar = true;
    hasRoared = false;

    damageCooldown = 0.f;
    damageCooldownMax = 1.5f;

    for (int i = 0; i < MAX_ARMS; i++) 
    {
        armActive[i] = false;
        lightningSprites[i] = nullptr;
    }

    for (int i = 0; i < MAX_CREATURES; i++)
    {
        creatures[i] = nullptr;
    }
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        bombs[i] = nullptr;
    }

    healthBarBg.setFillColor(Color(100, 0, 0));  // dark red
    healthBarFill.setFillColor(Color(0, 200, 0));  // green

    // scoreReward starts at 0 -- set to 10000 when health hits 0
    scoreReward = 0;

    loadSprite();
    setupFrames();
}

void Gamakichi::setColor(int color) {}

void Gamakichi::loadSprite()
{
    texture.loadFromFile("SnowBrosAssets/Images/Gamakichi.png");
    sprite = new Sprite(texture);
    for (int i = 0; i < 4; i++)
    {
        fireSprites[i] = new Sprite(texture);
    }
    for (int i = 0; i < MAX_ARMS; i++)
    {
        lightningSprites[i] = new Sprite(texture);
    }
}

void Gamakichi::setupFrames()
{
    idleFrame = IntRect({ 121,  0 }, { 1107, 640 });
    attackFrame = IntRect({ 1433, 6 }, { 1107, 644 });

    fireFrames[0] = IntRect({ 2741, 631 }, { 332, 265 });
    fireFrames[1] = IntRect({ 3072, 631 }, { 327, 269 });
    fireFrames[2] = IntRect({ 3403, 611 }, { 324, 285 });
    fireFrames[3] = IntRect({ 3727, 618 }, { 326, 278 });

    lightningFrames[0] = IntRect({ 54,  652 }, { 239, 297 });
    lightningFrames[1] = IntRect({ 362, 662 }, { 287, 326 });

    armOffsets[0] = { -50.f, 30.f };
    armOffsets[1] = { -60.f, 80.f };
    armOffsets[2] = { -45.f, 120.f };
    armOffsets[3] = { hitbox.getWidth() - 10.f, 30.f };
    armOffsets[4] = { hitbox.getWidth(), 80.f };
    armOffsets[5] = { hitbox.getWidth() - 15.f, 120.f };

    sprite->setTextureRect(idleFrame);
    float sx = hitbox.getWidth() / (float)idleFrame.size.x;
    float sy = hitbox.getHeight() / (float)idleFrame.size.y;
    sprite->setScale({ sx, sy });

    float fireW = 100.f, fireH = 100.f;
    for (int i = 0; i < 4; i++) 
    {
        fireSprites[i]->setTextureRect(fireFrames[i]);
        fireSprites[i]->setScale({ fireW / (float)fireFrames[i].size.x,
                                   fireH / (float)fireFrames[i].size.y });
    }

    for (int i = 0; i < MAX_ARMS; i++) {
        lightningSprites[i]->setTextureRect(lightningFrames[0]);
        float ls = 50.f / lightningFrames[0].size.x;
        lightningSprites[i]->setScale({ ls, ls });
    }
}


// ATTACK
void Gamakichi::startAttack()
{
    shouldRoar = true;
    state = STATE_ATTACKING;
    stateTimer = 0.f;

    int numArms = (rand() % 2 == 0) ? 4 : 6;  // randomly 4 or 6 arms
    for (int i = 0; i < MAX_ARMS; i++)
    {
        armActive[i] = false;
    }
    for (int i = 0; i < numArms; i++)
    {
        armActive[i] = true;
    }
    for (int i = 0; i < numArms; i++)
    {
        spawnCreatureFromArm(i);
    }
    if (rand() % 3 == 0)
    {
        spawnBomb();  // 1-in-3 chance of a bomb
    }
}

void Gamakichi::spawnCreatureFromArm(int armIndex)
{
    for (int i = 0; i < MAX_CREATURES; i++) 
    {
        if (creatures[i] == nullptr || !creatures[i]->isAlive())
        {
            delete creatures[i];
            float sx = enemyX + armOffsets[armIndex].x;  // arm's position on screen
            float sy = enemyY + armOffsets[armIndex].y;  // arm's position on screen
            bool goLeft = (armIndex < 3);   // 0,1,2 on left and 3,4,5 on right
            creatures[i] = new GamakichiCreature(sx, sy, windowW, windowH, goLeft);
            return;
        }
    }
}

void Gamakichi::spawnBomb()
{
    for (int i = 0; i < MAX_BOMBS; i++) {

        if (bombs[i] == nullptr || !bombs[i]->isAlive()) 
        {
            delete bombs[i];
            float bx = enemyX + (rand() % 2 == 0 ? -100.f : hitbox.getWidth() + 100.f);
            float by = enemyY - 300.f;
            bombs[i] = new GamakichiBomb(bx, by, windowW, windowH);
            return;
        }
    }
}


// Updating gamakichicreature and bombs
void Gamakichi::updateProjectiles(float dt, Platform* platforms, int platformCount)
{
    for (int i = 0; i < MAX_CREATURES; i++)
    {
        if (creatures[i] != nullptr && creatures[i]->isAlive())
        {
            creatures[i]->update(dt, platforms, platformCount);
        }
    }
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i] != nullptr && bombs[i]->isAlive())
        {
            bombs[i]->update(dt, platforms, platformCount);
        }
    }
    // bomb destroys creatures on contact
    for (int b = 0; b < MAX_BOMBS; b++) 
    {
        if (bombs[b] == nullptr || !bombs[b]->isAlive())
        {
            continue;
        }
        FloatRect br = bombs[b]->getHitbox().getRect();
        for (int c = 0; c < MAX_CREATURES; c++) 
        {
            if (creatures[c] == nullptr || !creatures[c]->isAlive())
            {
                continue;
            }
            if (br.findIntersection(creatures[c]->getHitbox().getRect()).has_value())  // has value returns true if they overlap
            {
                creatures[c]->dieByBomb();
                bombs[b]->die();
                break;
            }
        }
    }
}

void Gamakichi::drawProjectiles(RenderWindow& window)
{
    for (int i = 0; i < MAX_CREATURES; i++)
    {
        if (creatures[i] != nullptr && creatures[i]->isAlive())
        {
            creatures[i]->draw(window);
        }
    }
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i] != nullptr && bombs[i]->isAlive())
        {
            bombs[i]->draw(window);
        }
    }
}


// UPDATE
void Gamakichi::update(float dt, Platform* platforms, int platformCount)
{
    if (damageCooldown > 0.f)
    {
        damageCooldown -= dt;
    }

    if (!alive)
    {
        return;
    }
    if (snowCoated || snowRolling)
    {
        return;
    }

    // DEATH 
    if (isDying)
    {
        stateTimer += dt;
        sprite->setColor(Color(180, 100, 255));

        // after 1.5s flash, start sinking off screen
        if (stateTimer >= 1.5f)
        {
            enemyY += 150.f * dt;
            if (enemyY > windowH + hitbox.getHeight())
            {
                alive = false;
                return;
            }
        }

        hitbox.update(enemyX, enemyY);
        sprite->setPosition({ enemyX, enemyY });

        // fire sinks with Gamakichi
        float slotStep = 60.f;  //  each fire sprite is 60 pixels apart from the next
        float wallStartX = enemyX + (hitbox.getWidth() / 2.f) - (slotStep * 1.5f) - 50.f;  // calculates the leftmost fire position so all 4 fire sprites together are centered under Gamakichi
        float fireYPos = enemyY + hitbox.getHeight() - 40.f;  // places the fire just below Gamakichi's feet 
        for (int i = 0; i < 4; i++) 
        {
            int slot = (i + firePosFrame) % 4;  // firePosFrame cycles 0→1→2→3→0 over time which creates a firy animation
            fireSprites[i]->setPosition({ wallStartX + slot * slotStep, fireYPos });
        }

        updateProjectiles(dt, platforms, platformCount);
        return;
    }

    // NORMAL UPDATE 
    stateTimer += dt;   // counts time for the current state
    fireAnimTimer += dt;  // counts time for the fire animation

    if (fireAnimTimer >= 0.1f) {
        fireAnimTimer = 0.f;
        firePosFrame = (firePosFrame + 1) % 4;  // every 0.1 seconds advance firePosFrame by 1
    }

    switch (state)
    {
    case STATE_ENTERING:
    {
        // rise from below screen to bottom position
        enemyY -= moveSpeed * dt;
        if (enemyY <= positionBottom)
        {
            enemyY = positionBottom;
            state = STATE_AT_BOTTOM;
            stateTimer = 0.f;
        }
        break;
    }

    case STATE_AT_BOTTOM:
    {
        enemyY = positionBottom;  // lock position
        if (stateTimer >= dwellTime)  // after dwellTime (3 seconds) of waiting trigger an attack and reset the timer
        {
            startAttack();        // attack while at bottom
            stateTimer = 0.f;
        }
        sprite->setTextureRect(idleFrame);
        break;
    }

    case STATE_RISING:
    {
        enemyY -= moveSpeed * dt;  // move upward
        if (enemyY <= positionTop)
        {
            enemyY = positionTop;
            state = STATE_AT_TOP;
            stateTimer = 0.f;
        }
        sprite->setTextureRect(idleFrame);
        break;
    }

    case STATE_AT_TOP:
    {
        enemyY = positionTop;   // lock position
        if (stateTimer >= dwellTime)
        {
            startAttack();       // attack while at top
            stateTimer = 0.f;
        }
        sprite->setTextureRect(idleFrame);
        break;
    }

    case STATE_DESCENDING:
    {
        enemyY += moveSpeed * dt;  // move downward
        if (enemyY >= positionBottom)
        {
            enemyY = positionBottom;
            state = STATE_AT_BOTTOM;
            stateTimer = 0.f;
        }
        sprite->setTextureRect(idleFrame);
        break;
    }

    case STATE_ATTACKING:
    {
        sprite->setTextureRect(attackFrame);

        lightningTimer += dt;
        int li = ((int)(lightningTimer * 15.f)) % 2;  // multiply by 15 and cast to int means it increments 15 times per second
                                                      // % 2 makes it alternate between 0 and 1 so li flickers between 0 and 1 fifteen times per second
        for (int i = 0; i < MAX_ARMS; i++)
        {
            if (!armActive[i])
            {
                continue;
            }
            lightningSprites[i]->setTextureRect(lightningFrames[li]);
            float lx = enemyX + armOffsets[i].x;
            float ly = enemyY + armOffsets[i].y;
            float lsx = lightningSprites[i]->getScale().x;  /* The reason we need to read the current scale first is because the next line 
                                       uses fabs(lsx) to strip its sign, then reapplies it as either positive or negative depending on which side the arm is on*/
            lightningSprites[i]->setScale({ (i < 3) ? -fabs(lsx) : fabs(lsx), fabs(lsx) });  // fabs() gives the absolute valu
            lightningSprites[i]->setPosition({ lx, ly });
        }

        if (stateTimer >= 1.0f)  // after 1 second of attacking, deactivate all arms and the lightning disappears
        {
            for (int i = 0; i < MAX_ARMS; i++)
            {
                armActive[i] = false;
            }

            if (enemyY >= positionBottom - 10.f)
            {
                state = STATE_RISING;
            }

            else
            {
                state = STATE_DESCENDING;
            }

            stateTimer = 0.f;
        }
        break;
    }
    }

    hitbox.update(enemyX, enemyY);
    sprite->setPosition({ enemyX, enemyY });

    // fire position
    float slotStep = 60.f;
    float wallStartX = enemyX + (hitbox.getWidth() / 2.f) - (slotStep * 1.5f) - 50.f;
    float fireYPos = enemyY + hitbox.getHeight() - 40.f;
    for (int i = 0; i < 4; i++) 
    {
        int slot = (i + firePosFrame) % 4;
        fireSprites[i]->setPosition({ wallStartX + slot * slotStep, fireYPos });
    }

    // health bar
    float pct = (float)health / maxHealth;
    healthBarBg.setSize({ hitbox.getWidth(), 10 });
    healthBarBg.setPosition({ enemyX, enemyY - 15 });
    healthBarFill.setSize({ hitbox.getWidth() * pct, 10 });
    healthBarFill.setPosition({ enemyX, enemyY - 15 });

    // cleaning up dead creatures
    for (int i = 0; i < MAX_CREATURES; i++)
    {
        if (creatures[i] != nullptr && !creatures[i]->isAlive())
        {
            delete creatures[i];
            creatures[i] = nullptr;
        }
    }

    updateProjectiles(dt, platforms, platformCount);
}


// DRAW
void Gamakichi::draw(RenderWindow& window)
{
    if (!alive)
    {
        return;
    }

    window.draw(*sprite);

    if (state == STATE_ATTACKING && !isDying)
    {
        for (int i = 0; i < MAX_ARMS; i++)
        {
            if (armActive[i])
            {
                window.draw(*lightningSprites[i]);
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        window.draw(*fireSprites[i]);
    }

    if (!isDying)
    {
        window.draw(healthBarBg);
        window.draw(healthBarFill);
    }
    drawProjectiles(window);
}


// BOSS INTERFACE
bool Gamakichi::getShouldRoar()
{
    bool r = shouldRoar;
    shouldRoar = false;
    return r;
}

void Gamakichi::addSnow(int amount) 
{
    // Don't take damage if already dying or dead
    if (state == STATE_DYING || !alive)
    {
        return;
    }

    if (damageCooldown <= 0.f)        
    {
        takeDamage(2);
        damageCooldown = damageCooldownMax;  
    }

    if (health <= 0 && scoreReward == 0)
    {
        scoreReward = 10000;  // Gamakichi gives 10000 pts fixed
        
    }
}

void Gamakichi::die()
{
    if (isDying || !alive)
    {
        return;
    }
    if (health <= 0)
    {
        isDying = true;
        stateTimer = 0.f;
    }
}

void Gamakichi::takeDamage(int amount)
{
    if (isDying || !alive)
    {
        return;
    }
    health -= amount;
    if (health <= 0)
    {
        health = 0;
        isDying = true;
        stateTimer = 0.f;
    }
}

void Gamakichi::onHitByRollingSnowball()
{
    if (damageCooldown <= 0.f)
    {
        takeDamage(2);
        damageCooldown = damageCooldownMax;
    }
}

int Gamakichi::getCreatureCount() const
{
    int count = 0;
    for (int i = 0; i < MAX_CREATURES; i++)
    {
        if (creatures[i] != nullptr && creatures[i]->isAlive())
        {
            count++;
        }
    }
    return count;
}

GamakichiCreature* Gamakichi::getCreature(int index) const
{
    if (index < 0 || index >= MAX_CREATURES)
    {
        return nullptr;
    }
    return creatures[index];
}

GamakichiBomb* Gamakichi::getBomb(int index) const
{
    if (index < 0 || index >= MAX_BOMBS)
    {
        return nullptr;
    }
    return bombs[index];
}


// DESTRUCTOR
Gamakichi::~Gamakichi()
{
    for (int i = 0; i < 4; i++)
    {
        delete fireSprites[i];
    }
    for (int i = 0; i < MAX_ARMS; i++)
    {
        delete lightningSprites[i];
    }
    for (int i = 0; i < MAX_CREATURES; i++)
    {
        delete creatures[i];
    }
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        delete bombs[i];
    }
}