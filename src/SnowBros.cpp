/* Muhammad Abdullah Malik  25I-3006
   Muhammad Habib Ashfaq    25I-3025  SE-D */

#include "SnowBros.h"
#include "MogeraChild.h"
#include "GamakichiCreature.h"
#include "GamakichiBomb.h"
#include "Powers.h"
#include <cmath>
#include <fstream>
#include <cstdlib>

static string getCurrentDate()
{
    time_t now = time(nullptr);
    // localtime_s is the MSVC-safe version -- fills our own tm struct.
    // Argument order is reversed vs localtime: result first, time second.
    tm t{};
    localtime_s(&t, &now);
    int year = t.tm_year + 1900;
    int month = t.tm_mon + 1;
    int day = t.tm_mday;
    string d = "";
    d += char('0' + year / 1000);
    d += char('0' + (year / 100) % 10);
    d += char('0' + (year / 10) % 10);
    d += char('0' + year % 10);
    d += '-';
    d += char('0' + month / 10);
    d += char('0' + month % 10);
    d += '-';
    d += char('0' + day / 10);
    d += char('0' + day % 10);
    return d;
}

SnowBros::SnowBros()
{
    VideoMode desktop = VideoMode::getDesktopMode();
    windowW = (float)desktop.size.x;
    windowH = (float)desktop.size.y;
    window.create(desktop, "Snow Bros", State::Fullscreen);
    window.setFramerateLimit(60);

    font.openFromFile("SnowBrosAssets/Fonts/snow-bros.ttf");
    music.openFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
    music.setLooping(true);

    if (!shootBuf.loadFromFile("SnowBrosAssets/Sounds/player-shoot.ogg"))
    {
        shootBuf.loadFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
    }
    if (!dieBuf.loadFromFile("SnowBrosAssets/Sounds/player-die.ogg"))
    {
        dieBuf.loadFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
    }
    if (!jumpBuf.loadFromFile("SnowBrosAssets/Sounds/player-jump.ogg"))
    {
        jumpBuf.loadFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
    }
    if (!enemyDieBuf.loadFromFile("SnowBrosAssets/Sounds/enemy-die.ogg"))
    {
        enemyDieBuf.loadFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
    }
    if (!roarBuf.loadFromFile("SnowBrosAssets/Sounds/boss-roar.ogg"))
    {
        roarBuf.loadFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
    }
    shootSnd = new Sound(shootBuf);
    dieSnd = new Sound(dieBuf);
    jumpSnd = new Sound(jumpBuf);
    enemyDieSnd = new Sound(enemyDieBuf);
    roarSnd = new Sound(roarBuf);

    bossMusic.openFromFile("SnowBrosAssets/Sounds/final_boss_snow_bros.ogg");
    bossMusic.setLooping(true);
    isBossLevel = false;

    p1WasThrowing = false;
    p2WasThrowing = false;
    mogeraWasRoaring = false;
    gamakichiWasRoaring = false;

    auth = new Auth(font, windowW, windowH);
    mainMenu = new MainMenu(font, windowW, windowH, nullptr);
    shop = new Shop(font, windowW, windowH, nullptr);
    modeSelect = new ModeSelect(font, windowW, windowH, nullptr);
    levelSelect = new LevelSelect(font, windowW, windowH, nullptr);
    pauseMenu = new PauseMenu(font, windowW, windowH, nullptr);
    leaderboard = new Leaderboard(font, windowW, windowH);
    helpScreen = new HelpScreen(font, windowW, windowH);
    settings = new Settings(font, windowW, windowH);
    // LevelComplete is a value member (not a pointer) so we call init here.
    // It needs the font and window size to center and draw all its text.
    levelComplete = LevelComplete(font, windowW, windowH);

    selectedIndex = 0;
    currentScreen = 0;
    levelSelectReturnScreen = 4;
    isPaused = false;

    player1 = nullptr;
    player2 = nullptr;
    twoPlayerMode = false;
    currentLevel = nullptr;
    currentLevelNum = 1;
    isNewGame = true;
    showHitBoxes = false;
    enemyCount = 0;
    score = 0;
    levelComplete.reset();  // reset typewriter + done flag for new level

    // Star power-up state -- starPowerUp created in startGameplay() when level 7 loads
    starPowerUp = nullptr;
    starPanelOpen = false;
    starPanelSelected = 0;
    starPanelOptions[0] = starPanelOptions[1] = starPanelOptions[2] = 0;
    starPowerUp2 = nullptr;
    starPanelOpen2 = false;
    starPanelSelected2 = 0;
    starPanelOptions2[0] = starPanelOptions2[1] = starPanelOptions2[2] = 0;

    gameOverSelectedIndex = 0;
    gameOverEnterPressed = false;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i] = nullptr;
    }
    for (int i = 0; i < MAX_GEMS; i++)
    {
        gems[i] = nullptr;
    }
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        powerUps[i] = nullptr;
    }
    bossRewardGiven = false;
    bonusRainActive = false;
    bonusTimer = 0.f;
    clearFallingGems();

    music.play();
}


// DESTRUCTOR


SnowBros::~SnowBros()
{
    music.stop();
    bossMusic.stop();

    delete shootSnd;
    delete dieSnd;
    delete jumpSnd;
    delete enemyDieSnd;
    delete roarSnd;

    delete auth;
    delete mainMenu;
    delete shop;
    delete modeSelect;
    delete levelSelect;
    delete pauseMenu;
    delete leaderboard;
    delete player1;
    delete player2;
    delete currentLevel;
    delete helpScreen;
    delete settings;
    clearEnemies();
    clearGems();
    clearPowerUps();
    clearFallingGems();
}

// ENEMY HELPERS
void SnowBros::clearEnemies()
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        delete enemies[i];
        enemies[i] = nullptr;
    }
    enemyCount = 0;
    clearGems();
    clearPowerUps();
}

void SnowBros::spawnEnemiesForLevel(int lvl)
{
    clearEnemies();  // clear all memory then take a fresh start

    if (lvl == 5)
    {
        enemies[0] = new Mogera(windowW - 500, 200, windowW, windowH);
        enemyCount = 1;
        return;
    }
    if (lvl == 10)
    {
        enemies[0] = new Gamakichi(windowW, windowH);
        enemyCount = 1;
        return;
    }

    int count = 2 + lvl;
    if (count > MAX_ENEMIES)
    {
        count = MAX_ENEMIES;
    }

    float spacing = (windowW - 350.f) / (count + 1);  // gap between enemies

    for (int i = 0; i < count; i++)
    {
        float  ex = 170.f + spacing * (i + 1);
        float  ey = 100.f;
        Enemy* e = nullptr;

        if (lvl <= 2)
        {
            Botom* b = new Botom(ex, ey, windowW, windowH);
            if (lvl >= 2 && i % 3 == 0)  // 1 enemy orange on level 2
            {
                b->setColor(Botom::getColorOrange());
            }
            e = b;
        }
        else if (lvl <= 4)
        {
            if (i % 2 == 0)
            {
                FlyingFoogaFoog* f = new FlyingFoogaFoog(ex, ey, windowW, windowH);
                if (lvl == 4 && i % 3 == 0)
                {
                    f->setColor(FlyingFoogaFoog::getColorRed());
                }
                e = f;
            }
            else
            {
                Botom* b = new Botom(ex, ey, windowW, windowH);
                if (lvl == 4 || i % 3 == 0)
                {
                    b->setColor(Botom::getColorOrange());
                }
                e = b;
            }
        }
        else if (lvl <= 7)
        {
            int type = i % 3;
            if (type == 0)
            {
                Botom* b = new Botom(ex, ey, windowW, windowH);
                b->setColor(lvl >= 7 ? Botom::getColorBlue() : Botom::getColorOrange());
                e = b;
            }
            else if (type == 1)
            {
                FlyingFoogaFoog* f = new FlyingFoogaFoog(ex, ey, windowW, windowH);
                f->setColor(lvl >= 7 ? FlyingFoogaFoog::getColorBlue() : FlyingFoogaFoog::getColorOrange());
                e = f;
            }
            else
            {
                Tornado* t = new Tornado(ex, ey, windowW, windowH);
                t->setColor(Tornado::getColorRed());
                e = t;
            }
        }
        else
        {
            int type = i % 3;
            if (type == 0)
            {
                Botom* b = new Botom(ex, ey, windowW, windowH);
                b->setColor(Botom::getColorBlue());
                e = b;
            }
            else if (type == 1)
            {
                FlyingFoogaFoog* f = new FlyingFoogaFoog(ex, ey, windowW, windowH);
                f->setColor(FlyingFoogaFoog::getColorBlue());
                e = f;
            }
            else
            {
                Tornado* t = new Tornado(ex, ey, windowW, windowH);
                t->setColor(Tornado::getColorBlue());
                e = t;
            }
        }

        enemies[i] = e;
        enemyCount++;
    }
}

// START GAMEPLAY
void SnowBros::startGameplay(bool multiPlayer, bool newGame)
{
    twoPlayerMode = multiPlayer;
    isNewGame = newGame;

    mogeraWasRoaring = false;
    gamakichiWasRoaring = false;

    delete player1;
    player1 = nullptr;
    delete player2;
    player2 = nullptr;
    delete currentLevel;
    currentLevel = nullptr;

    isBonusLevel = (currentLevelNum == 4 || currentLevelNum == 9);
    bonusRainActive = false;
    bonusTimer = 0.f;
    clearFallingGems();

    if (currentLevelNum < 1)
    {
        currentLevelNum = 1;
    }
    if (currentLevelNum > 10)
    {
        currentLevelNum = 10;
    }

    score = 0;
    levelComplete.reset();  // reset typewriter + done flag for new level
    bossRewardGiven = false;
    clearGems();
    clearPowerUps();

    currentLevel = new Level(currentLevelNum, windowW, windowH);
    player1 = new Nick(200, 200, windowW, windowH);
    if (twoPlayerMode)
    {
        player2 = new Tom(windowW - 300, 200, windowW, windowH);
    }

    music.stop();
    bossMusic.stop();

    if (currentLevelNum == 5 || currentLevelNum == 10)
    {
        isBossLevel = true;
        bossMusic.play();
    }
    else
    {
        isBossLevel = false;
        music.openFromFile("SnowBrosAssets/Sounds/snow_bros_level.ogg");
        music.setLooping(true);
        music.play();
    }

    spawnEnemiesForLevel(currentLevelNum);
    pauseMenu->init(windowW, windowH, &currentPlayer);
    isPaused = false;

    // Star power-up: only active on STAR_LEVEL (level 7)
    // If entering level 7, create a fresh StarPowerUp at screen center
    // If leaving level 7, delete the object to free memory
    // reset() is called (not new) when player dies and restarts level 7
    // that is handled in the player death block in updateGameplay()
    delete starPowerUp;
    starPowerUp = nullptr;
    starPanelOpen = false;
    delete starPowerUp2;
    starPowerUp2 = nullptr;
    starPanelOpen2 = false;
    if (currentLevelNum == STAR_LEVEL)
    {
        // Player1 star: left-center of screen
        starPowerUp = new StarPowerUp(windowW, windowH);
        if (twoPlayerMode)
        {
            starPowerUp2 = new StarPowerUp(windowW, windowH);
        }
    }
    currentScreen = 6;
}


// ADVANCE TO NEXT LEVEL


void SnowBros::advanceLevel()
{
    currentLevelNum++;

    isBonusLevel = (currentLevelNum == 4 || currentLevelNum == 9);
    bonusRainActive = false;
    bonusTimer = 0.f;
    clearFallingGems();

    if (currentLevelNum > 10)
    {
        music.stop();
        bossMusic.stop();
        music.openFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
        music.setLooping(true);
        music.play();

        saveHighScore();
        currentPlayer.currentLevel = 1;
        saveProgress();
        currentScreen = 2;
        mainMenu->init(windowW, windowH, &currentPlayer);
        mainMenu->reset();
        return;
    }

    currentPlayer.currentLevel = currentLevelNum;
    saveProgress();

    delete currentLevel;
    currentLevel = new Level(currentLevelNum, windowW, windowH);

    if (player1)
    {
        delete player1;
        player1 = new Nick(200, 200, windowW, windowH);
    }
    if (twoPlayerMode && player2)
    {
        delete player2;
        player2 = new Tom(windowW - 300, 200, windowW, windowH);
    }

    bonusRainActive = false;
    bonusTimer = 0.f;
    clearFallingGems();

    spawnEnemiesForLevel(currentLevelNum);

    // Switch music for boss levels vs normal levels
    music.stop();
    bossMusic.stop();

    if (currentLevelNum == 5 || currentLevelNum == 10)
    {
        isBossLevel = true;
        bossMusic.play();
    }
    else
    {
        isBossLevel = false;
        music.openFromFile("SnowBrosAssets/Sounds/snow_bros_level.ogg");
        music.setLooping(true);
        music.play();
    }

    levelComplete.reset();
    bossRewardGiven = false;
    clearGems();
    clearPowerUps();
    currentScreen = 6;
}

// APPLY ROLL HIT
void SnowBros::applyRollHit(Enemy* roller, Enemy* victim)
{
    victim->onHitByRollingSnowball();
    enemyDieSnd->play();
    roller->addRollKill();  // used for increasing snowball size

    // Per-enemy-type score ranges
    int typeId = victim->getEnemyType();
    int minScore = 100, maxScore = 500;  // default  (Botom range)
    if (typeId == 1) { minScore = 200;  maxScore = 800; }  // FlyingFoogaFoog
    else if (typeId == 2) { minScore = 300;  maxScore = 1200; }  // Tornado
    else if (typeId == 6) { minScore = 200;  maxScore = 800; }  // GamakichiCreature

    int baseScore = minScore + rand() % (maxScore - minScore + 1);

    // Chain kill bonus: +10% of baseScore per enemy already in this chain.
    // getRollKillCount() returns kills so far on this rolling snowball.
    // So 1st kill = +0%, 2nd kill = +10%, 3rd kill = +20%, etc.
    int bonus = (int)(baseScore * roller->getRollKillCount() * 0.1f);
    if (player1)
    {
        player1->addScore(baseScore + bonus);
    }
    score += baseScore + bonus;
    // Real-time leaderboard update 
    saveHighScore();

    // Spawn gems where victim died
    int gemVal = 1 + (rand() % 5);
    spawnGem(victim->getX(), victim->getY(), gemVal, false);

    // Spawn power-up where victim died (respects level limit)
    spawnPowerUp(victim->getX(), victim->getY());

}


// COLLISION: SNOWBALL vs ENEMY 
void SnowBros::checkSnowballEnemyCollision()
{
    for (int playerNum = 1; playerNum <= 2; playerNum++)
    {
        if (playerNum == 2 && !twoPlayerMode)
        {
            continue;
        }
        if (playerNum == 2 && player2 == nullptr)
        {
            continue;
        }

        Player* shooter = (playerNum == 1) ? player1 : player2;

        for (int s = 0; s < SnowballLauncher::getMaxProjectiles(); s++)
        {
            Snowball* sb = shooter->getLauncher().getProjectile(s);
            if (sb == nullptr || !sb->isActive())  // if snowball is not active
            {
                continue;
            }

            FloatRect sbRect = sb->getRect();
            bool consumed = false;  // consumed means once a snowball hits something, stop checking

            for (int e = 0; e < MAX_ENEMIES && !consumed; e++)
            {
                if (enemies[e] == nullptr || !enemies[e]->isAlive())
                {
                    continue;
                }

                // Direct hit on main enemy (including boss bodies) 
                {
                    FloatRect er = enemies[e]->getRect();
                    bool hit = sbRect.position.x + sbRect.size.x > er.position.x &&
                        sbRect.position.x < er.position.x + er.size.x &&
                        sbRect.position.y + sbRect.size.y > er.position.y &&
                        sbRect.position.y < er.position.y + er.size.y;
                    if (hit)
                    {
                        int etype = enemies[e]->getEnemyType();
                        bool isBossHit = (etype == 3 || etype == 4);
                        if (isBossHit)
                        {
                            enemies[e]->addSnow();
                        }
                        else
                        {
                            if (shooter->getSnowballPowerActive())  // If the player has snowball power-up then instantly fully coat the enemy
                            {
                                enemies[e]->addSnow(enemies[e]->getsnowMaxLevel());
                            }
                            else
                            {
                                enemies[e]->addSnow();
                            }
                        }
                        sb->deactivate();
                        consumed = true;
                        continue; // try next enemy slot (consumed=true breaks outer)
                    }
                }

                // Mogera children
                if (enemies[e]->getEnemyType() == 3)
                {
                    Mogera* mog = (Mogera*)enemies[e];
                    for (int c = 0; c < 10 && !consumed; c++)
                    {
                        MogeraChild* child = mog->getCreature(c);
                        if (child == nullptr || !child->isAlive())
                        {
                            continue;
                        }

                        FloatRect cr = child->getRect();
                        bool chit = sbRect.position.x + sbRect.size.x > cr.position.x &&
                            sbRect.position.x < cr.position.x + cr.size.x &&
                            sbRect.position.y + sbRect.size.y > cr.position.y &&
                            sbRect.position.y < cr.position.y + cr.size.y;
                        if (chit)
                        {
                            if (shooter->getSnowballPowerActive())
                            {
                                child->addSnow(child->getsnowMaxLevel());
                            }
                            else
                            {
                                child->addSnow();
                            }
                            sb->deactivate();
                            consumed = true;
                        }
                    }
                }

                // Gamakichi creatures 
                if (enemies[e]->getEnemyType() == 4)
                {
                    Gamakichi* gama = (Gamakichi*)enemies[e];
                    for (int c = 0; c < 12 && !consumed; c++)
                    {
                        GamakichiCreature* cre = gama->getCreature(c);
                        if (cre == nullptr || !cre->isAlive())
                        {
                            continue;
                        }

                        FloatRect cr = cre->getRect();
                        bool chit = sbRect.position.x + sbRect.size.x > cr.position.x &&
                            sbRect.position.x < cr.position.x + cr.size.x &&
                            sbRect.position.y + sbRect.size.y > cr.position.y &&
                            sbRect.position.y < cr.position.y + cr.size.y;
                        if (chit)
                        {
                            if (shooter->getSnowballPowerActive())
                            {
                                cre->addSnow(cre->getsnowMaxLevel());
                            }
                            else
                            {
                                cre->addSnow();
                            }
                            sb->deactivate();
                            consumed = true;
                        }
                    }
                }
            }
        }
    }
}


// checkRollingSnowballChain
void SnowBros::checkRollingSnowballChain()
{
    // enemies on screen vs everything 
    for (int r = 0; r < MAX_ENEMIES; r++)
    {
        if (enemies[r] == nullptr || !enemies[r]->isAlive())
        {
            continue;
        }
        if (!enemies[r]->isRollingSnowball())  // skip enemies that are not rolling
        {
            continue;
        }
        FloatRect rr = enemies[r]->getHitboxRect();  // get the rolling enemy's hitbox rectangle so we can check collisions

        bool rollerConsumed = false;  // it is added to create proper rolling chain animation

        // vs main enemies
        for (int e = 0; e < MAX_ENEMIES && !rollerConsumed; e++)  // to track if this roller has already hit something
        {
            if (e == r)  // dont check the roller against its own self
            {
                continue;
            }

            if (enemies[e] == nullptr || !enemies[e]->isAlive())  // skip empty/dead enemies
            {
                continue;
            }
            if (enemies[e]->isRollingSnowball()) // skip other rollers
            {
                continue;
            }

            FloatRect er = enemies[e]->getHitboxRect();
            bool hit = rr.position.x + rr.size.x > er.position.x &&
                rr.position.x < er.position.x + er.size.x &&
                rr.position.y + rr.size.y > er.position.y &&
                rr.position.y < er.position.y + er.size.y;
            if (!hit)
            {
                continue;
            }
            applyRollHit(enemies[r], enemies[e]);
        }
        if (rollerConsumed)
        {
            continue;
        }
    }

    // Rolling Mogera children vs Mogera boss 
    for (int e = 0; e < MAX_ENEMIES; e++)
    {
        if (enemies[e] == nullptr || !enemies[e]->isAlive())
        {
            continue;
        }
        if (enemies[e]->getEnemyType() != 3)
        {
            continue;
        }
        Mogera* mog = (Mogera*)enemies[e];

        for (int rc = 0; rc < 10; rc++)
        {
            MogeraChild* roller = mog->getCreature(rc);
            if (!roller || !roller->isAlive() || !roller->isRollingSnowball())
            {
                continue;
            }
            FloatRect rr = roller->getHitboxRect();
            bool rollerConsumed = false;

            // vs Mogera boss body
            FloatRect er = enemies[e]->getHitboxRect();
            bool hit = rr.position.x + rr.size.x > er.position.x &&
                rr.position.x < er.position.x + er.size.x &&
                rr.position.y + rr.size.y > er.position.y &&
                rr.position.y < er.position.y + er.size.y;

            if (hit && !roller->getIsDying())
            {
                mog->onHitByRollingSnowball();
                roller->die();
                rollerConsumed = true;
            }

            if (rollerConsumed)
            {
                continue;
            }

            // vs sibling Mogera children
            for (int vc = 0; vc < 10; vc++)
            {
                if (vc == rc)
                {
                    continue;
                }
                MogeraChild* victim = mog->getCreature(vc);
                if (!victim || !victim->isAlive() || victim->isRollingSnowball())
                {
                    continue;
                }
                FloatRect cr = victim->getHitboxRect();
                bool vhit = rr.position.x + rr.size.x > cr.position.x &&
                    rr.position.x < cr.position.x + cr.size.x &&
                    rr.position.y + rr.size.y > cr.position.y &&
                    rr.position.y < cr.position.y + cr.size.y;

                if (vhit)
                {
                    applyRollHit(roller, victim);
                    break;
                }
            }
        }
    }

    // Rolling Gamakichi creatures vs Gamakichi boss 
    for (int e = 0; e < MAX_ENEMIES; e++)
    {
        if (enemies[e] == nullptr || !enemies[e]->isAlive())
        {
            continue;
        }
        if (enemies[e]->getEnemyType() != 4)
        {
            continue;
        }
        Gamakichi* gama = (Gamakichi*)enemies[e];

        for (int rc = 0; rc < 12; rc++)
        {
            GamakichiCreature* roller = gama->getCreature(rc);
            if (!roller || !roller->isAlive() || !roller->isRollingSnowball())
            {
                continue;
            }
            FloatRect rr = roller->getHitboxRect();
            bool rollerConsumed = false;

            // vs Gamakichi boss body
            FloatRect er = enemies[e]->getHitboxRect();
            bool hit = rr.position.x + rr.size.x > er.position.x &&
                rr.position.x < er.position.x + er.size.x &&
                rr.position.y + rr.size.y > er.position.y &&
                rr.position.y < er.position.y + er.size.y;

            if (hit && !roller->getIsDying())
            {
                roller->die();
                rollerConsumed = true;
                gama->onHitByRollingSnowball();
            }

            if (rollerConsumed)
            {
                continue;
            }
            // vs sibling Gamakichi creatures
            for (int vc = 0; vc < 12; vc++)
            {
                if (vc == rc)
                {
                    continue;
                }
                GamakichiCreature* victim = gama->getCreature(vc);
                if (!victim || !victim->isAlive() || victim->isRollingSnowball())
                {
                    continue;
                }
                FloatRect cr = victim->getHitboxRect();
                bool vhit = rr.position.x + rr.size.x > cr.position.x &&
                    rr.position.x < cr.position.x + cr.size.x &&
                    rr.position.y + rr.size.y > cr.position.y &&
                    rr.position.y < cr.position.y + cr.size.y;

                if (vhit)
                {
                    applyRollHit(roller, victim);
                    break;
                }
            }
        }
    }
}

// PLAYER vs ENEMY collision
void SnowBros::checkPlayerEnemyCollision()
{
    for (int playerNum = 1; playerNum <= 2; playerNum++)
    {
        if (playerNum == 2 && !twoPlayerMode)
        {
            continue;
        }
        if (playerNum == 2 && player2 == nullptr)
        {
            continue;
        }

        Player* p = (playerNum == 1) ? player1 : player2;
        if (p == nullptr || p->getIsDead())
        {
            continue;
        }

        FloatRect pr = p->getRect();
        bool inBalloon = p->getBalloonModeActive();
        bool playerHit = false;

        // Main enemies
        for (int e = 0; e < MAX_ENEMIES && !playerHit; e++)
        {
            if (enemies[e] == nullptr || !enemies[e]->isAlive())
            {
                continue;
            }
            if (enemies[e]->isFullyCoated())
            {
                continue;
            }
            bool isBoss = (enemies[e]->getEnemyType() == 3 || enemies[e]->getEnemyType() == 4);

            FloatRect er = enemies[e]->getRect();
            bool touch = pr.position.x + pr.size.x > er.position.x &&
                pr.position.x < er.position.x + er.size.x &&
                pr.position.y + pr.size.y > er.position.y &&
                pr.position.y < er.position.y + er.size.y;

            if (touch)
            {
                if (inBalloon && !isBoss)  // bosses are not damaged in balloon mode
                {
                    applyRollHit(enemies[e], enemies[e]);
                }
                else if (!inBalloon)
                {
                    p->loseLife();
                    playerHit = true;
                }
            }
        }

        // Knife projectiles from Tornado
        if (!inBalloon && !playerHit)
        {
            for (int e = 0; e < MAX_ENEMIES && !playerHit; e++)
            {
                if (enemies[e] == nullptr || !enemies[e]->isAlive())
                {
                    continue;
                }
                if (enemies[e]->getEnemyType() != 2)
                {
                    continue;
                }

                Tornado* t = (Tornado*)enemies[e];
                Knife* k = t->getKnife();
                if (k == nullptr || !k->isActive())
                {
                    continue;
                }
                FloatRect kr = k->getRect();
                bool kHit = pr.position.x + pr.size.x > kr.position.x &&
                    pr.position.x < kr.position.x + kr.size.x &&
                    pr.position.y + pr.size.y > kr.position.y &&
                    pr.position.y < kr.position.y + kr.size.y;

                if (kHit)
                {
                    p->loseLife();
                    k->deactivate();
                    playerHit = true;
                }
            }
        }

        // Mogera children
        for (int e = 0; e < MAX_ENEMIES && !playerHit; e++)
        {
            if (enemies[e] == nullptr || !enemies[e]->isAlive())
            {
                continue;
            }
            if (enemies[e]->getEnemyType() != 3)
            {
                continue;
            }
            Mogera* boss = (Mogera*)enemies[e];

            for (int c = 0; c < 10 && !playerHit; c++)
            {
                MogeraChild* child = boss->getCreature(c);
                if (child == nullptr || !child->isAlive())
                {
                    continue;
                }
                if (child->isFullyCoated())
                {
                    continue;
                }

                FloatRect cr = child->getRect();
                bool hit = pr.position.x + pr.size.x > cr.position.x &&
                    pr.position.x < cr.position.x + cr.size.x &&
                    pr.position.y + pr.size.y > cr.position.y &&
                    pr.position.y < cr.position.y + cr.size.y;

                if (hit)
                {
                    if (inBalloon)
                    {
                        applyRollHit(child, child);
                    }
                    else
                    {
                        p->loseLife();
                        playerHit = true;
                    }
                }
            }
        }

        // Gamakichi creatures
        for (int e = 0; e < MAX_ENEMIES && !playerHit; e++)
        {
            if (enemies[e] == nullptr || !enemies[e]->isAlive())
            {
                continue;
            }
            if (enemies[e]->getEnemyType() != 4)
            {
                continue;
            }

            Gamakichi* boss = (Gamakichi*)enemies[e];

            for (int c = 0; c < 12 && !playerHit; c++)
            {
                GamakichiCreature* creature = boss->getCreature(c);
                if (creature == nullptr || !creature->isAlive()) continue;
                if (creature->isFullyCoated()) continue;

                FloatRect cr = creature->getRect();
                bool hit = pr.position.x + pr.size.x > cr.position.x &&
                    pr.position.x < cr.position.x + cr.size.x &&
                    pr.position.y + pr.size.y > cr.position.y &&
                    pr.position.y < cr.position.y + cr.size.y;

                if (hit)
                {
                    if (inBalloon)
                    {
                        applyRollHit(creature, creature);
                    }
                    else
                    {
                        p->loseLife();
                        playerHit = true;
                    }
                }
            }
        }

        // Gamakichi bombs
        if (!inBalloon && !playerHit)
        {
            for (int e = 0; e < MAX_ENEMIES && !playerHit; e++)
            {
                if (enemies[e] == nullptr || !enemies[e]->isAlive())
                {
                    continue;
                }
                if (enemies[e]->getEnemyType() != 4)
                {
                    continue;
                }
                Gamakichi* boss = (Gamakichi*)enemies[e];

                for (int b = 0; b < 5 && !playerHit; b++)
                {
                    GamakichiBomb* bomb = boss->getBomb(b);
                    if (bomb == nullptr || !bomb->isAlive())
                    {
                        continue;
                    }

                    FloatRect br = bomb->getRect();
                    bool hit = pr.position.x + pr.size.x > br.position.x &&
                        pr.position.x < br.position.x + br.size.x &&
                        pr.position.y + pr.size.y > br.position.y &&
                        pr.position.y < br.position.y + br.size.y;

                    if (hit)
                    {
                        p->loseLife();
                        bomb->die();
                        playerHit = true;
                    }
                }
            }
        }
    }
}

// Player push snowball
void SnowBros::checkPlayerPushSnowball()
{
    for (int playerNum = 1; playerNum <= 2; playerNum++)
    {
        if (playerNum == 2 && !twoPlayerMode)
        {
            continue;
        }
        if (playerNum == 2 && player2 == nullptr)
        {
            continue;
        }

        Player* p = (playerNum == 1) ? player1 : player2;
        if (p == nullptr || p->getIsDead())
        {
            continue;
        }
        if (!p->getPushKeyPressed())  // if push key is not pressed skip this all entirely
        {
            continue;
        }
        p->clearPushKeyPressed();  // clear the flag so it doesn't trigger again next frame

        FloatRect pr = p->getRect();
        float reach = 50.f;  // they can push anything within 60 pixels in front of them
        FloatRect extPr = pr;  // copying player's rectangle to extended one for pushing

        if (p->getFacingRight())
        {
            extPr.size.x += reach;
        }
        else
        {
            extPr.position.x -= reach;
            extPr.size.x += reach;
        }

        bool pushed = false;

        // Main enemies
        for (int e = 0; e < MAX_ENEMIES && !pushed; e++)
        {
            if (enemies[e] == nullptr || !enemies[e]->isAlive())
            {
                continue;
            }
            if (!enemies[e]->isFullyCoated() || enemies[e]->isRollingSnowball())  // if not fully coated or already rolling
            {
                continue;
            }

            FloatRect er = enemies[e]->getRect();
            bool touch = extPr.position.x + extPr.size.x > er.position.x &&
                extPr.position.x < er.position.x + er.size.x &&
                extPr.position.y + extPr.size.y > er.position.y &&
                extPr.position.y < er.position.y + er.size.y;
            if (touch)
            {
                p->startPush();
                enemies[e]->pushSnowball(p->getFacingRight() ? 1.f : -1.f);
                pushed = true;
            }
        }

        // Mogera children
        for (int e = 0; e < MAX_ENEMIES && !pushed; e++)
        {
            if (enemies[e] == nullptr || !enemies[e]->isAlive())
            {
                continue;
            }
            if (enemies[e]->getEnemyType() != 3)
            {
                continue;
            }

            Mogera* boss = (Mogera*)enemies[e];
            for (int c = 0; c < 10 && !pushed; c++)
            {
                MogeraChild* child = boss->getCreature(c);
                if (!child || !child->isAlive())
                {
                    continue;
                }
                if (!child->isFullyCoated() || child->isRollingSnowball())
                {
                    continue;
                }

                FloatRect cr = child->getRect();
                bool touch = extPr.position.x + extPr.size.x > cr.position.x &&
                    extPr.position.x < cr.position.x + cr.size.x &&
                    extPr.position.y + extPr.size.y > cr.position.y &&
                    extPr.position.y < cr.position.y + cr.size.y;
                if (touch)
                {
                    p->startPush();
                    child->pushSnowball(p->getFacingRight() ? 1.f : -1.f);  // 1 and -1 for pushing direction as needed in pushSnowball
                    pushed = true;
                }
            }
        }

        // Gamakichi creatures
        for (int e = 0; e < MAX_ENEMIES && !pushed; e++)
        {
            if (enemies[e] == nullptr || !enemies[e]->isAlive())
            {
                continue;
            }
            if (enemies[e]->getEnemyType() != 4)
            {
                continue;
            }

            Gamakichi* boss = (Gamakichi*)enemies[e];
            for (int c = 0; c < 12 && !pushed; c++)
            {
                GamakichiCreature* cre = boss->getCreature(c);
                if (!cre || !cre->isAlive())
                {
                    continue;
                }
                if (!cre->isFullyCoated() || cre->isRollingSnowball())
                {
                    continue;
                }
                FloatRect cr = cre->getRect();
                bool touch = extPr.position.x + extPr.size.x > cr.position.x &&
                    extPr.position.x < cr.position.x + cr.size.x &&
                    extPr.position.y + extPr.size.y > cr.position.y &&
                    extPr.position.y < cr.position.y + cr.size.y;
                if (touch)
                {
                    p->startPush();
                    cre->pushSnowball(p->getFacingRight() ? 1.f : -1.f);
                    pushed = true;
                }
            }
        }
    }
}


// GAMEPLAY UPDATE
void SnowBros::updateGameplay(float dt)
{
    Platform* plats = currentLevel->getPlatforms();
    int count = currentLevel->getPlatformCount();

    player1->update(dt, plats, count);
    bool p1NowThrowing = player1->getLauncher().getIsThrowing();
    if (p1NowThrowing && !p1WasThrowing)  // so that shoot sound is played exactly once
    {
        shootSnd->play();
    }
    p1WasThrowing = p1NowThrowing;

    if (twoPlayerMode && player2)
    {
        player2->update(dt, plats, count);
        bool p2NowThrowing = player2->getLauncher().getIsThrowing();
        if (p2NowThrowing && !p2WasThrowing)
        {
            shootSnd->play();
        }
        p2WasThrowing = p2NowThrowing;
    }

    // ── Star Power-Up update + collection check 
    // Only runs on level 7. starPowerUp is nullptr on all other levels
    // When the panel is open the game is effectively paused for gameplay
    // but we still need to handle input -- done in event block in run()
    // --- Player1 star ---
    if (starPowerUp != nullptr && starPowerUp->isActive() && !starPanelOpen)
    {
        starPowerUp->update(dt);
        FloatRect sr = starPowerUp->getRect();
        FloatRect pr = player1->getRect();
        bool p1Hit = pr.position.x < sr.position.x + sr.size.x &&
            pr.position.x + pr.size.x > sr.position.x &&
            pr.position.y < sr.position.y + sr.size.y &&
            pr.position.y + pr.size.y > sr.position.y;
        if (p1Hit)
        {
            starPowerUp->collect();
            int pool[4] = { 0, 1, 2, 3 };
            for (int k = 3; k > 0; k--)
            {
                int j = rand() % (k + 1); int tmp = pool[k]; pool[k] = pool[j]; pool[j] = tmp;
            }
            starPanelOptions[0] = pool[0];
            starPanelOptions[1] = pool[1];
            starPanelOptions[2] = pool[2];
            starPanelSelected = 0;
            starPanelOpen = true;
        }
    }

    // --- Player2 star (multiplayer only, fully independent) ---
    if (twoPlayerMode && player2 && starPowerUp2 != nullptr && starPowerUp2->isActive() && !starPanelOpen2)
    {
        starPowerUp2->update(dt);
        FloatRect sr2 = starPowerUp2->getRect();
        FloatRect p2r = player2->getRect();
        bool p2Hit = p2r.position.x < sr2.position.x + sr2.size.x &&
            p2r.position.x + p2r.size.x > sr2.position.x &&
            p2r.position.y < sr2.position.y + sr2.size.y &&
            p2r.position.y + p2r.size.y > sr2.position.y;
        if (p2Hit)
        {
            starPowerUp2->collect();
            int pool[4] = { 0, 1, 2, 3 };
            for (int k = 3; k > 0; k--)
            {
                int j = rand() % (k + 1); int tmp = pool[k]; pool[k] = pool[j]; pool[j] = tmp;
            }
            starPanelOptions2[0] = pool[0];
            starPanelOptions2[1] = pool[1];
            starPanelOptions2[2] = pool[2];
            starPanelSelected2 = 0;
            starPanelOpen2 = true;
        }
    }

    if (player1->getJustJumped())
    {
        jumpSnd->play();
    }
    if (twoPlayerMode && player2 && player2->getJustJumped())
    {
        jumpSnd->play();
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i] != nullptr && enemies[i]->isAlive())
        {
            enemies[i]->update(dt, plats, count);  // udate every living enemy's movement, animations
        }
    }

    checkSnowballEnemyCollision();
    checkPlayerEnemyCollision();
    checkPlayerPushSnowball();
    checkRollingSnowballChain();
    //------
    updateGems(dt);
    checkGemCollection();
    updatePowerUps(dt);
    checkPowerUpCollection();
    if (bonusRainActive)
    {
        updateBonusRain(dt); return;
    }

    // Boss roar sounds
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i] == nullptr || !enemies[i]->isAlive())
        {
            continue;
        }

        if (enemies[i]->getEnemyType() == 3) // 3 = Mogera
        {
            Mogera* mog = (Mogera*)enemies[i];
            bool now = mog->getShouldRoar();
            if (now && !mogeraWasRoaring)
            {
                roarSnd->play();
            }
            mogeraWasRoaring = now;
        }

        if (enemies[i]->getEnemyType() == 4) // 4 = Gamakichi
        {
            Gamakichi* gama = (Gamakichi*)enemies[i];
            bool now = gama->getShouldRoar();
            if (now && !gamakichiWasRoaring)
            {
                roarSnd->play();
            }
            gamakichiWasRoaring = now;
        }
    }

    // Check player death
    if (player1->getIsDead() && player1->getDeathDone())
    {
        if (player1->getLives() <= 0)
        {
            // Only game over if player 2 is also dead or doesn't exist
            if (!twoPlayerMode || player2 == nullptr || (player2->getIsDead() && player2->getLives() <= 0))
            {
                dieSnd->play();
                saveHighScore();
                currentScreen = 7;
                return;
            }
            // else player 2 still alive, p1 just stays dead
        }
        else
        {
            dieSnd->play();
            player1->revive();
            if (currentLevelNum == STAR_LEVEL && starPowerUp != nullptr && !starPowerUp->wasCollected())
                starPowerUp->reset(windowW, windowH);
        }
    }

    if (twoPlayerMode && player2 && player2->getIsDead() && player2->getDeathDone())
    {
        if (player2->getLives() <= 0)
        {
            dieSnd->play();
            if (player1->getLives() <= 0)
            {
                saveHighScore();
                currentScreen = 7;
                return;
            }
        }
        else
        {
            player2->revive();
            dieSnd->play();
            if (currentLevelNum == STAR_LEVEL && starPowerUp2 != nullptr && !starPowerUp2->wasCollected())
                starPowerUp2->reset(windowW, windowH);
        }
    }

    if (twoPlayerMode && player2 && player2->getIsDead() && player2->getDeathDone())
    {
        if (player2->getLives() <= 0)
        {
            if (player1->getLives() <= 0)
            {
                dieSnd->play();
                saveHighScore();
                currentScreen = 7;
                return;
            }
        }
        else player2->revive();
    }

    // Check level clear
    {
        // For normal enemies: fully coated ones count as defeated
        // For bosses: only count them as defeated once alive == false
        bool allDefeated = true;
        int  aliveCount = 0;

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i] == nullptr || !enemies[i]->isAlive()) continue;
            aliveCount++;

            int etype = enemies[i]->getEnemyType();
            bool isBoss = (etype == 3 || etype == 4);

            if (isBoss)
            {
                // Boss counts as playing until alive == false
                allDefeated = false;
            }
            else
            {
                // Normal enemy: must be coated or dying to be "done"
                if (!enemies[i]->isFullyCoated() && !enemies[i]->getIsDying())
                {
                    allDefeated = false;
                }
            }
        }

        // Finish off any coated normal enemies
        if (allDefeated && aliveCount > 0)
        {
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i] == nullptr || !enemies[i]->isAlive())
                {
                    continue;
                }
                int etype = enemies[i]->getEnemyType();
                bool isBoss = (etype == 3 || etype == 4);
                if (!isBoss && enemies[i]->isFullyCoated())
                {
                    enemies[i]->die();
                    enemyDieSnd->play();
                }
            }
        }

        // Checks if all dead
        bool allDead = true;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i] != nullptr && enemies[i]->isAlive())
            {
                allDead = false;
                break;
            }
        }
        if (allDead)
        {
            // Boss gem rewards
            if (!bossRewardGiven)
            {
                if (currentLevelNum == 5)
                {
                    currentPlayer.gems += 200;
                    saveProgress();
                    bossRewardGiven = true;
                }
                if (currentLevelNum == 10)
                {
                    currentPlayer.gems += 500;
                    saveProgress();
                    bossRewardGiven = true;
                }
            }

            // Boss score rewards (spec: Mogera=5000, Gamakichi=10000 pts fixed)
            if (enemies[0] != nullptr)
            {
                int etype = enemies[0]->getEnemyType();
                if (etype == 3)  // Mogera
                {
                    Mogera* m = (Mogera*)enemies[0];
                    if (m->getScoreReward() > 0)
                    {
                        score += m->getScoreReward();
                        m->clearScoreReward();
                        // Real-time leaderboard update on boss kill
                        saveHighScore();
                    }
                }
                if (etype == 4)  // Gamakichi
                {
                    Gamakichi* g = (Gamakichi*)enemies[0];
                    if (g->getScoreReward() > 0)
                    {
                        score += g->getScoreReward();
                        g->clearScoreReward();
                        // Real-time leaderboard update on boss kill
                        saveHighScore();
                    }
                }
            }

            if (isBonusLevel && !bonusRainActive)
            {
                startBonusRain(); return;
            }
            if (isBonusLevel && bonusRainActive)
            {
                return;
            }

            levelComplete.reset();
            currentScreen = 8;      // switch to level complete screen immediately
        }
    }
}


// GAMEPLAY DRAW
void SnowBros::drawGameplay()
{
    window.clear(Color::Black);
    currentLevel->draw(window);

    MogeraChild::setShowHitbox(showHitBoxes);
    GamakichiCreature::setShowHitbox(showHitBoxes);
    GamakichiBomb::setShowHitbox(showHitBoxes);

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i] != nullptr && enemies[i]->isAlive())
        {
            enemies[i]->draw(window);
        }
    }

    player1->draw(window);
    if (twoPlayerMode && player2)
    {
        player2->draw(window);
    }

    if (showHitBoxes)
    {
        player1->drawDebug(window);
        if (twoPlayerMode && player2)
        {
            player2->drawDebug(window);
        }

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i] == nullptr || !enemies[i]->isAlive())  // dont draw for dead enemies
            {
                continue;
            }
            enemies[i]->drawDebug(window);

            // Draw Mogera children hitboxes
            if (enemies[i]->getEnemyType() == 3)
            {
                Mogera* mog = (Mogera*)enemies[i];
                for (int c = 0; c < 10; c++)
                {
                    MogeraChild* child = mog->getCreature(c);
                    if (child && child->isAlive())
                    {
                        child->drawDebug(window);
                    }
                }
            }

            // Draw Gamakichi creature hitboxes
            if (enemies[i]->getEnemyType() == 4)
            {
                Gamakichi* gama = (Gamakichi*)enemies[i];
                for (int c = 0; c < 12; c++)
                {
                    GamakichiCreature* cre = gama->getCreature(c);
                    if (cre && cre->isAlive())
                    {
                        cre->drawDebug(window);
                    }
                }

                // Draw Gamakichi bomb hitboxes
                for (int b = 0; b < 3; b++)
                {
                    GamakichiBomb* bomb = gama->getBomb(b);
                    if (bomb && bomb->isAlive())
                    {
                        bomb->getHitbox().draw(window, Color::Red);
                    }
                }
            }
        }

        Platform* plats = currentLevel->getPlatforms();
        int platCount = currentLevel->getPlatformCount();
        for (int i = 0; i < platCount; i++)
        {
            plats[i].drawDebug(window);
        }

        // player 1 snowball hitboxes (yellow)
        for (int s = 0; s < SnowballLauncher::getMaxProjectiles(); s++)
        {
            Snowball* sb = player1->getLauncher().getProjectile(s);
            if (sb != nullptr && sb->isActive())
            {
                sb->drawDebug(window);
            }
        }

        // player 2 snowball hitboxes (yellow)
        if (twoPlayerMode && player2)
        {
            for (int s = 0; s < SnowballLauncher::getMaxProjectiles(); s++)
            {
                Snowball* sb = player2->getLauncher().getProjectile(s);
                if (sb != nullptr && sb->isActive())
                {
                    sb->drawDebug(window);
                }
            }
        }

        // knife hitboxes from Tornado enemies (yellow)
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i] == nullptr || !enemies[i]->isAlive())
            {
                continue;
            }
            if (enemies[i]->getEnemyType() != 2)
            {
                continue;
            }
            Tornado* t = (Tornado*)enemies[i];
            Knife* k = t->getKnife();
            if (k != nullptr && k->isActive())
            {
                k->drawDebug(window);
            }
        }
    }

    drawHUD();
    drawGems();
    if (bonusRainActive) drawBonusRain();
    drawPowerUps();
    // Draw star if active on level 7
    if (starPowerUp != nullptr && starPowerUp->isActive())
        starPowerUp->draw(window);
    if (starPowerUp2 != nullptr && starPowerUp2->isActive())
        starPowerUp2->draw(window);
    // Draw selection panel on top of everything if open
    if (starPanelOpen)
        drawStarPanel();
    window.display();
}


// HUD

void SnowBros::drawHUD()
{
    // Score
    {
        int s = score;
        string sStr = (s == 0) ? "0" : "";
        while (s > 0) { sStr = char('0' + s % 10) + sStr; s /= 10; }
        Text t(font, "SCORE: " + sStr, 24);
        t.setFillColor(Color(255, 255, 255));
        t.setOutlineColor(Color::Black); t.setOutlineThickness(2);
        t.setPosition({ 90, 8 });
        window.draw(t);
    }

    // Lives - player 1
    // loseLife() fires immediately on hit, so add 1 back while death anim plays
    {
        int l = player1->getLives();
        if (player1->getIsDead() && !player1->getDeathDone()) l += 1;
        if (l < 0) l = 0;
        string ls = (l <= 0) ? "0" : "";
        while (l > 0) { ls = char('0' + l % 10) + ls; l /= 10; }
        Text t(font, "LIVES: " + ls, 24);
        t.setFillColor(Color(0, 220, 100));
        t.setOutlineColor(Color::Black); t.setOutlineThickness(2);
        t.setPosition({ 90, 36 });
        window.draw(t);
    }

    // Lives - player 2 (only in multiplayer)
    if (twoPlayerMode && player2)
    {
        int l = player2->getLives();
        if (player2->getIsDead() && !player2->getDeathDone()) l += 1;
        if (l < 0) l = 0;
        string ls = (l <= 0) ? "0" : "";
        while (l > 0) { ls = char('0' + l % 10) + ls; l /= 10; }
        Text t(font, "P2 LIVES: " + ls, 24);
        t.setFillColor(Color(255, 100, 100));  // red to distinguish from player 1
        t.setOutlineColor(Color::Black); t.setOutlineThickness(2);
        t.setPosition({ 90, 64 });  // sits below player 1 lives
        window.draw(t);
    }

    // Level indicator
    {
        int lvl = currentLevelNum;
        string ls = "";
        while (lvl > 0) { ls = char('0' + lvl % 10) + ls; lvl /= 10; }
        Text t(font, "LEVEL " + ls + " / 10", 24);
        FloatRect tb = t.getLocalBounds();
        t.setFillColor(Color(255, 220, 0));
        t.setOutlineColor(Color::Black); t.setOutlineThickness(2);
        t.setPosition({ (windowW - tb.size.x) / 2, 8 });
        window.draw(t);
    }

    // Gems
    {
        int g = currentPlayer.gems;
        string gs = (g == 0) ? "0" : "";
        while (g > 0) { gs = char('0' + g % 10) + gs; g /= 10; }
        Text t(font, "GEMS: " + gs, 24);
        FloatRect tb = t.getLocalBounds();
        t.setFillColor(Color(255, 215, 0));
        t.setOutlineColor(Color::Black); t.setOutlineThickness(2);
        t.setPosition({ windowW - tb.size.x - 90, 8 });
        window.draw(t);
    }

    // --- Power-Up HUD Status
    //  Bar animation technical explanation:
   //    Fill bar (bright color, width = barMaxW * fillRatio) -- the 'full' part
   //   fillRatio = currentTimer / maxDuration (0.0 to 1.0)
   //   Player::update() decrements the timer each frame via -= dt,
   //   so fillRatio shrinks automatically every frame 
    {
        // Bar visual constants
        const float barMaxW = 200.f;  // max width of the timer bar in pixels
        const float barH = 10.f;   // height of the bar
        const float rowH = 28.f;   // vertical space per power-up row
        // Start Y: above the bottom hint text (hint is at windowH-24, each row is 28px)
        // We count how many are active first so rows stack cleanly
        int activeCount = 0;
        if (player1->getSpeedBoostActive())      activeCount++;
        if (player1->getBalloonModeActive())     activeCount++;
        if (player1->getSnowballPowerActive())   activeCount++;
        if (player1->getDistanceIncreaseActive()) activeCount++;

        // rowY starts high enough that all rows fit above the hint
        // Each row is rowH pixels tall. We start from the topmost row.
        float rowY = windowH - 24.f - (activeCount * rowH) - 6.f;

        // Helper lambda-style block -- we repeat this pattern 4 times.
        // Each block: check if active, draw name + bar + timer, advance rowY.
        // We use a struct-like approach with local variables instead of lambda
        // (no lambdas allowed per project rules).

        // --- Speed Boost (timer: 0-15s, cyan) ---
        if (player1->getSpeedBoostActive())
        {
            float timer = player1->getSpeedBoostTimer();
            float maxDur = 15.f;  // Speed Boost lasts 15 seconds (spec)
            float fillRatio = timer / maxDur;
            if (fillRatio > 1.f) fillRatio = 1.f;
            if (fillRatio < 0.f) fillRatio = 0.f;
            Color barColor(0, 220, 255);       // cyan
            Color darkColor(0, 60, 80);        // dark cyan for background

            // Name label
            Text name(font, "SPEED BOOST", 16);
            name.setFillColor(barColor);
            name.setOutlineColor(Color::Black); name.setOutlineThickness(1);
            FloatRect nb = name.getLocalBounds();
            float blockW = nb.size.x + 8.f + barMaxW + 8.f + 40.f;
            float startX = (windowW - blockW) / 2.f;
            name.setPosition({ startX, rowY + 4.f });
            window.draw(name);

            // Background bar (dark, full width) -- the 'empty' portion
            float barX = startX + nb.size.x + 8.f;
            RectangleShape bgBar({ barMaxW, barH });
            bgBar.setFillColor(darkColor);
            bgBar.setPosition({ barX, rowY + 10.f });
            window.draw(bgBar);

            // Fill bar (bright, shrinking width) -- the 'remaining' portion
            // fillRatio * barMaxW gives the pixel width for this frame
            RectangleShape fillBar({ barMaxW * fillRatio, barH });
            fillBar.setFillColor(barColor);
            fillBar.setPosition({ barX, rowY + 10.f });
            window.draw(fillBar);

            // Remaining seconds -- convert float to string manually
            // Cast to int to show whole seconds (e.g. 8.3s -> "8s")
            int secs = (int)timer + 1;  // +1 so it shows 1s not 0s on last frame
            string secStr = "";
            if (secs <= 0) secStr = "0s";
            else { while (secs > 0) { secStr = char('0' + secs % 10) + secStr; secs /= 10; } secStr += "s"; }
            Text secText(font, secStr, 16);
            secText.setFillColor(barColor);
            secText.setPosition({ barX + barMaxW + 6.f, rowY + 4.f });
            window.draw(secText);

            rowY += rowH;  // advance to next row
        }

        // --- Balloon Mode (timer: 0-10s, pink) ---
        if (player1->getBalloonModeActive())
        {
            float timer = player1->getBalloonTimer();
            float maxDur = 15.f;  // Balloon Mode lasts 15 seconds (spec says 10, shop gives 30)
            float fillRatio = timer / maxDur;
            if (fillRatio > 1.f) fillRatio = 1.f;
            if (fillRatio < 0.f) fillRatio = 0.f;
            Color barColor(255, 100, 200);     // pink
            Color darkColor(80, 30, 60);       // dark pink for background

            Text name(font, "BALLOON MODE", 16);
            name.setFillColor(barColor);
            name.setOutlineColor(Color::Black); name.setOutlineThickness(1);
            FloatRect nb = name.getLocalBounds();
            float blockW = nb.size.x + 8.f + barMaxW + 8.f + 40.f;
            float startX = (windowW - blockW) / 2.f;
            name.setPosition({ startX, rowY + 4.f });
            window.draw(name);

            float barX = startX + nb.size.x + 8.f;
            RectangleShape bgBar({ barMaxW, barH });
            bgBar.setFillColor(darkColor);
            bgBar.setPosition({ barX, rowY + 10.f });
            window.draw(bgBar);

            RectangleShape fillBar({ barMaxW * fillRatio, barH });
            fillBar.setFillColor(barColor);
            fillBar.setPosition({ barX, rowY + 10.f });
            window.draw(fillBar);

            int secs = (int)timer + 1;
            string secStr = "";
            if (secs <= 0) secStr = "0s";
            else { while (secs > 0) { secStr = char('0' + secs % 10) + secStr; secs /= 10; } secStr += "s"; }
            Text secText(font, secStr, 16);
            secText.setFillColor(barColor);
            secText.setPosition({ barX + barMaxW + 6.f, rowY + 4.f });
            window.draw(secText);

            rowY += rowH;
        }

        // --- Snowball Power (permanent until level end, orange) ---
        // This lasts until level end so we show a full solid bar with ACTIVE text
        // instead of a shrinking timer -- there is no countdown to display.
        if (player1->getSnowballPowerActive())
        {
            Color barColor(255, 140, 0);       // orange
            Color darkColor(80, 44, 0);        // dark orange for background

            Text name(font, "SNOWBALL POWER", 16);
            name.setFillColor(barColor);
            name.setOutlineColor(Color::Black); name.setOutlineThickness(1);
            FloatRect nb = name.getLocalBounds();
            float blockW = nb.size.x + 8.f + barMaxW + 8.f + 50.f;
            float startX = (windowW - blockW) / 2.f;
            name.setPosition({ startX, rowY + 4.f });
            window.draw(name);

            float barX = startX + nb.size.x + 8.f;
            // Background (dark) bar
            RectangleShape bgBar({ barMaxW, barH });
            bgBar.setFillColor(darkColor);
            bgBar.setPosition({ barX, rowY + 10.f });
            window.draw(bgBar);
            // Fill bar -- full width because permanent
            RectangleShape fillBar({ barMaxW, barH });
            fillBar.setFillColor(barColor);
            fillBar.setPosition({ barX, rowY + 10.f });
            window.draw(fillBar);

            // Show ACTIVE instead of a timer
            Text secText(font, "ACTIVE", 16);
            secText.setFillColor(barColor);
            secText.setPosition({ barX + barMaxW + 6.f, rowY + 4.f });
            window.draw(secText);

            rowY += rowH;
        }

        // Distance Increase (permanent until level end, green)
        if (player1->getDistanceIncreaseActive())
        {
            Color barColor(0, 255, 100);       // green
            Color darkColor(0, 80, 30);        // dark green for background

            Text name(font, "DISTANCE UP", 16);
            name.setFillColor(barColor);
            name.setOutlineColor(Color::Black); name.setOutlineThickness(1);
            FloatRect nb = name.getLocalBounds();
            float blockW = nb.size.x + 8.f + barMaxW + 8.f + 50.f;
            float startX = (windowW - blockW) / 2.f;
            name.setPosition({ startX, rowY + 4.f });
            window.draw(name);

            float barX = startX + nb.size.x + 8.f;
            RectangleShape bgBar({ barMaxW, barH });
            bgBar.setFillColor(darkColor);
            bgBar.setPosition({ barX, rowY + 10.f });
            window.draw(bgBar);
            RectangleShape fillBar({ barMaxW, barH });
            fillBar.setFillColor(barColor);
            fillBar.setPosition({ barX, rowY + 10.f });
            window.draw(fillBar);

            Text secText(font, "ACTIVE", 16);
            secText.setFillColor(barColor);
            secText.setPosition({ barX + barMaxW + 6.f, rowY + 4.f });
            window.draw(secText);

            rowY += rowH;
        }
    }

    // Bottom hint
    {
        Text t(font, "F1: hitboxes   ESC: pause", 16);
        t.setFillColor(Color(80, 80, 80));
        FloatRect tb = t.getLocalBounds();
        t.setPosition({ (windowW - tb.size.x) / 2, windowH - 24 });
        window.draw(t);
    }
}


// Game Over Screen with Restart Option


void SnowBros::drawGameOver()
{
    window.clear(Color(10, 0, 20));
    float t = clock.getElapsedTime().asSeconds();

    Text title(font, "GAME OVER", 100);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 60, 60));
    title.setOutlineColor(Color(100, 0, 0)); title.setOutlineThickness(5);
    title.setPosition({ (windowW - tb.size.x) / 2, windowH / 4 });
    window.draw(title);

    int s = score;
    string sStr = (s == 0) ? "0" : "";
    while (s > 0) { sStr = char('0' + s % 10) + sStr; s /= 10; }
    Text scoreText(font, "Score: " + sStr, 40);
    FloatRect sb = scoreText.getLocalBounds();
    scoreText.setFillColor(Color(255, 220, 0));
    scoreText.setPosition({ (windowW - sb.size.x) / 2, windowH / 2 });
    window.draw(scoreText);

    int l = currentLevelNum;
    string lStr = "";
    while (l > 0) { lStr = char('0' + l % 10) + lStr; l /= 10; }
    Text levelText(font, "Level reached: " + lStr, 30);
    FloatRect lb = levelText.getLocalBounds();
    levelText.setFillColor(Color(200, 200, 200));
    levelText.setPosition({ (windowW - lb.size.x) / 2, windowH / 2 + 60 });
    window.draw(levelText);

    // NEW: Restart / Menu options 
    const char* options[2] = { "Restart Level", "Main Menu" };
    float optStartY = windowH * 0.68f;

    for (int i = 0; i < 2; i++)
    {
        float y = optStartY + i * 55.f;
        Text opt(font, options[i], 32);
        FloatRect ob = opt.getLocalBounds();
        float x = (windowW - ob.size.x) / 2;

        if (i == gameOverSelectedIndex)
        {
            // Highlighted option
            float p = sinf(t * 5.f) * 0.5f + 0.5f;
            int g = 200 + (int)(p * 55);
            opt.setFillColor(Color(0, g, 255));
            opt.setOutlineColor(Color(0, 50, 120));
            opt.setOutlineThickness(2);

            // Arrow indicators
            Text la(font, ">>", 28);
            la.setFillColor(Color(255, 200, 0));
            la.setPosition({ x - 70, y + 2 });
            window.draw(la);

            Text ra(font, "<<", 28);
            ra.setFillColor(Color(255, 200, 0));
            ra.setPosition({ x + ob.size.x + 40, y + 2 });
            window.draw(ra);
        }
        else
        {
            opt.setFillColor(Color(160, 160, 160));
            opt.setOutlineColor(Color(50, 50, 50));
            opt.setOutlineThickness(2);
        }

        opt.setPosition({ x, y });
        window.draw(opt);
    }

    // Blinking hint
    if ((int)(t * 2) % 2 == 0)
    {
        Text hint(font, "UP/DOWN to select, ENTER to confirm", 20);
        FloatRect hb = hint.getLocalBounds();
        hint.setFillColor(Color(120, 120, 120));
        hint.setPosition({ (windowW - hb.size.x) / 2, windowH * 0.85f });
        window.draw(hint);
    }

    window.display();
}

void SnowBros::drawStarPanel()
{
    // Semi-transparent dark overlay over the whole screen
    // This dims the game behind the panel so the panel stands out
    RectangleShape overlay({ windowW, windowH });
    overlay.setFillColor(Color(0, 0, 0, 160));  // alpha 160 = ~63% opaque
    overlay.setPosition({ 0.f, 0.f });
    window.draw(overlay);

    // Panel dimensions and position (centered on screen)
    float panelW = 640.f;
    float panelH = 420.f;
    float panelX = (windowW - panelW) / 2.f;
    float panelY = (windowH - panelH) / 2.f;

    // Panel background
    RectangleShape panel({ panelW, panelH });
    panel.setFillColor(Color(10, 10, 40, 230));     // dark blue, slightly transparent
    panel.setOutlineColor(Color(255, 215, 0));       // gold border matches star color
    panel.setOutlineThickness(3.f);
    panel.setPosition({ panelX, panelY });
    window.draw(panel);

    // Title: "CHOOSE A POWER-UP"
    Text title(font, "CHOOSE A POWER-UP!", 32);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 215, 0));          // gold
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(2.f);
    title.setPosition({ panelX + (panelW - tb.size.x) / 2.f, panelY + 18.f });
    window.draw(title);

    // Subtitle hint
    Text hint(font, "UP/DOWN = navigate    ENTER = select", 16);
    FloatRect hb = hint.getLocalBounds();
    hint.setFillColor(Color(160, 160, 160));
    hint.setPosition({ panelX + (panelW - hb.size.x) / 2.f, panelY + 60.f });
    window.draw(hint);

    // Power-up names and descriptions for display
    // Index matches the type ID: 0=SpeedBoost 1=SnowballPower 2=Distance 3=Balloon
    const char* powerNames[4] = {
        "SPEED BOOST",
        "SNOWBALL POWER",
        "DISTANCE INCREASE",
        "BALLOON MODE"
    };
    const char* powerDescs[4] = {
        "Move 50% faster for 15 seconds",
        "1-hit encasing for rest of level",
        "Travels across full screen width",
        "Float upward, immune to ground enemies"
    };
    // Colors per power-up type (match HUD colors)
    Color powerColors[4] = {
        Color(0, 220, 255),    // cyan   -- Speed Boost
        Color(255, 140, 0),    // orange -- Snowball Power
        Color(0, 255, 100),    // green  -- Distance Increase
        Color(255, 100, 200)   // pink   -- Balloon Mode
    };

    // Draw 3 option rows
    float rowH = 82.f;
    float rowStartY = panelY + 105.f;

    for (int i = 0; i < 3; i++)
    {
        int typeId = starPanelOptions[i];  // which power-up this row shows
        bool isSelected = (i == starPanelSelected);

        float rowY = rowStartY + i * rowH;

        // Row background: brighter when selected, dark when not
        // This gives clear visual feedback on which option is highlighted
        RectangleShape rowBg({ panelW - 40.f, rowH - 8.f });
        rowBg.setPosition({ panelX + 20.f, rowY });
        if (isSelected)
            rowBg.setFillColor(Color(30, 30, 80, 200));   // selected: brighter blue
        else
            rowBg.setFillColor(Color(10, 10, 30, 150));   // unselected: very dark
        rowBg.setOutlineColor(isSelected ? powerColors[typeId] : Color(40, 40, 60));
        rowBg.setOutlineThickness(isSelected ? 2.f : 1.f);
        window.draw(rowBg);

        // Arrow indicator on selected row
        if (isSelected)
        {
            Text arrow(font, ">", 24);
            arrow.setFillColor(powerColors[typeId]);
            arrow.setPosition({ panelX + 26.f, rowY + 8.f });
            window.draw(arrow);
        }

        // Power-up name
        Color nameColor = isSelected ? powerColors[typeId] : Color(180, 180, 180);
        Text name(font, powerNames[typeId], 22);
        name.setFillColor(nameColor);
        name.setOutlineColor(Color::Black);
        name.setOutlineThickness(1.f);
        name.setPosition({ panelX + 52.f, rowY + 6.f });
        window.draw(name);

        // Power-up description (smaller, dimmer)
        Color descColor = isSelected ? Color(220, 220, 220) : Color(100, 100, 100);
        Text desc(font, powerDescs[typeId], 15);
        desc.setFillColor(descColor);
        desc.setPosition({ panelX + 52.f, rowY + 38.f });
        window.draw(desc);
    }
}


// DRAW STAR PANEL 2  (player2's independent star panel in multiplayer)
// Same layout as drawStarPanel() but shifted right so both can show
// simultaneously without overlapping, and uses starPanelOptions2.


void SnowBros::drawStarPanel2()
{
    // Semi-transparent overlay
    RectangleShape overlay({ windowW, windowH });
    overlay.setFillColor(Color(0, 0, 0, 100));
    overlay.setPosition({ 0.f, 0.f });
    window.draw(overlay);

    float panelW = 640.f;
    float panelH = 420.f;
    // Shift panel to the right side so it doesn't overlap player1's panel
    float panelX = windowW * 0.55f;
    float panelY = (windowH - panelH) / 2.f;

    RectangleShape panel({ panelW, panelH });
    panel.setFillColor(Color(10, 40, 10, 230));     // dark green tint for P2
    panel.setOutlineColor(Color(0, 255, 100));       // green border
    panel.setOutlineThickness(3.f);
    panel.setPosition({ panelX, panelY });
    window.draw(panel);

    Text title(font, "PLAYER 2 - CHOOSE!", 28);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(0, 255, 100));
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(2.f);
    title.setPosition({ panelX + (panelW - tb.size.x) / 2.f, panelY + 18.f });
    window.draw(title);

    Text hint(font, "NUM8/NUM2 = navigate    NUM0 = select", 14);
    FloatRect hb = hint.getLocalBounds();
    hint.setFillColor(Color(160, 160, 160));
    hint.setPosition({ panelX + (panelW - hb.size.x) / 2.f, panelY + 58.f });
    window.draw(hint);

    const char* powerNames[4] = { "SPEED BOOST", "SNOWBALL POWER", "DISTANCE INCREASE", "BALLOON MODE" };
    const char* powerDescs[4] = {
        "Move 50% faster for 15 seconds",
        "1-hit encasing for rest of level",
        "Travels across full screen width",
        "Float upward, immune to ground enemies"
    };
    Color powerColors[4] = {
        Color(0, 220, 255),
        Color(255, 140, 0),
        Color(0, 255, 100),
        Color(255, 100, 200)
    };

    float rowH = 82.f;
    float rowStartY = panelY + 105.f;

    for (int i = 0; i < 3; i++)
    {
        int typeId = starPanelOptions2[i];
        bool isSelected = (i == starPanelSelected2);
        float rowY = rowStartY + i * rowH;

        RectangleShape rowBg({ panelW - 40.f, rowH - 8.f });
        rowBg.setPosition({ panelX + 20.f, rowY });
        rowBg.setFillColor(isSelected ? Color(10, 50, 20, 200) : Color(5, 20, 10, 150));
        rowBg.setOutlineColor(isSelected ? powerColors[typeId] : Color(20, 60, 30));
        rowBg.setOutlineThickness(isSelected ? 2.f : 1.f);
        window.draw(rowBg);

        if (isSelected)
        {
            Text arrow(font, ">", 24);
            arrow.setFillColor(powerColors[typeId]);
            arrow.setPosition({ panelX + 26.f, rowY + 8.f });
            window.draw(arrow);
        }

        Color nameColor = isSelected ? powerColors[typeId] : Color(180, 180, 180);
        Text name(font, powerNames[typeId], 22);
        name.setFillColor(nameColor);
        name.setOutlineColor(Color::Black);
        name.setOutlineThickness(1.f);
        name.setPosition({ panelX + 52.f, rowY + 6.f });
        window.draw(name);

        Color descColor = isSelected ? Color(220, 220, 220) : Color(100, 100, 100);
        Text desc(font, powerDescs[typeId], 15);
        desc.setFillColor(descColor);
        desc.setPosition({ panelX + 52.f, rowY + 38.f });
        window.draw(desc);
    }
}


// FILE SAVE HELPERS


void SnowBros::saveProgress()
{
    string lines[100];
    int    lineCount = 0;
    ifstream in("users.txt");
    if (in.is_open())
    {
        string line;
        while (getline(in, line) && lineCount < 100)
            if (!line.empty()) { lines[lineCount] = line; lineCount++; }
        in.close();
    }

    for (int i = 0; i < lineCount; i++)
    {
        string parts[8]; int pi = 0; string cur = "";
        for (int j = 0; j < (int)lines[i].size(); j++)
        {
            if (lines[i][j] == '|') { if (pi < 7) { parts[pi++] = cur; cur = ""; } }
            else cur += lines[i][j];
        }
        parts[pi] = cur;
        if (pi < 5 || parts[0] != currentPlayer.username) continue;

        string gStr = "";
        int g = currentPlayer.gems;
        if (g == 0) gStr = "0";
        else while (g > 0) { gStr = char('0' + g % 10) + gStr; g /= 10; }

        string lvlStr = "";
        // Parse the already-saved level from file so we never go backwards.
        int savedLv = 0;
        for (int k = 0; k < (int)parts[4].size(); k++)
            if (parts[4][k] >= '0' && parts[4][k] <= '9')
                savedLv = savedLv * 10 + (parts[4][k] - '0');
        int lv = currentPlayer.currentLevel;
        if (lv < savedLv) lv = savedLv;
        if (lv == 0) lvlStr = "0";
        else while (lv > 0) { lvlStr = char('0' + lv % 10) + lvlStr; lv /= 10; }

        // Rebuild all 8 fields -- also update recorded_at so date is never N/A
        string today = getCurrentDate();
        lines[i] = parts[0] + "|" + parts[1] + "|" + parts[2] + "|" + gStr + "|" + lvlStr
            + "|" + parts[5] + "|" + parts[6] + "|" + today;
    }

    ofstream out("users.txt");
    if (out.is_open())
    {
        for (int i = 0; i < lineCount; i++) out << lines[i] << "\n";
        out.close();
    }
}

void SnowBros::saveHighScore()
{
    if (score <= currentPlayer.highScore) return;
    currentPlayer.highScore = score;

    string lines[100];
    int lineCount = 0;
    ifstream in("users.txt");
    if (in.is_open())
    {
        string line;
        while (getline(in, line) && lineCount < 100)
            if (!line.empty()) { lines[lineCount] = line; lineCount++; }
        in.close();
    }

    for (int i = 0; i < lineCount; i++)
    {
        // parts[8] holds all 8 fields: 0-username 1-password 2-email
        // 3-gems 4-level 5-highScore 6-created_at 7-recorded_at
        string parts[8]; int pi = 0; string cur = "";
        for (int j = 0; j < (int)lines[i].size(); j++)
        {
            if (lines[i][j] == '|') { if (pi < 7) { parts[pi++] = cur; cur = ""; } }
            else cur += lines[i][j];
        }
        parts[pi] = cur;
        if (pi < 5 || parts[0] != currentPlayer.username) continue;

        string hsStr = "";
        int hs = score;
        if (hs == 0) hsStr = "0";
        else while (hs > 0) { hsStr = char('0' + hs % 10) + hsStr; hs /= 10; }

        // parts[6] = created_at -- preserve exactly, never change after registration
        // parts[7] = recorded_at -- update to today: this is the session end timestamp
        string today = getCurrentDate();
        lines[i] = parts[0] + "|" + parts[1] + "|" + parts[2] + "|" + parts[3] + "|" + parts[4] + "|"
            + hsStr + "|" + parts[6] + "|" + today;
    }

    ofstream out("users.txt");
    if (out.is_open())
    {
        for (int i = 0; i < lineCount; i++) out << lines[i] << "\n";
        out.close();
    }
}


// SPLASH SCREEN


void SnowBros::drawMenu()
{
    window.clear(Color::Black);
    float t = clock.getElapsedTime().asSeconds();

    for (int i = 0; i < 40; i++)
    {
        float snowX = fmod(i * 173.7f + t * 50, windowW);
        float snowY = fmod(i * 97.3f + t * 50, windowH);
        float size = 1.5f + rand() % 4;
        CircleShape flake(size);
        flake.setFillColor(Color(255, 255, 255, 120 + (i % 5) * 15));
        flake.setPosition({ snowX, snowY });
        window.draw(flake);
    }

    RectangleShape topLine; topLine.setSize({ windowW, 3 }); topLine.setFillColor(Color(0, 200, 255)); topLine.setPosition({ 0, 0 }); window.draw(topLine);
    RectangleShape botLine; botLine.setSize({ windowW, 3 }); botLine.setFillColor(Color(0, 200, 255)); botLine.setPosition({ 0, windowH - 3 }); window.draw(botLine);

    int   titleSize = 130;
    float titleY = windowH / 8;
    Text  titleMain(font, "SNOW BROS", titleSize);
    FloatRect txt = titleMain.getLocalBounds();
    titleMain.setFillColor(Color(255, 255, 255)); titleMain.setOutlineColor(Color(255, 140, 0)); titleMain.setOutlineThickness(5);
    titleMain.setPosition({ (windowW - txt.size.x) / 2, titleY });
    window.draw(titleMain);

    int   nickSize = 65;
    float nickY = titleY + titleSize - 20;
    Text  nickText(font, "NICK", nickSize); nickText.setFillColor(Color(0, 210, 0));   nickText.setOutlineColor(Color(0, 80, 0));     nickText.setOutlineThickness(3);
    Text  andText(font, " & ", nickSize); andText.setFillColor(Color(255, 255, 255)); andText.setOutlineColor(Color(100, 100, 100)); andText.setOutlineThickness(3);
    Text  tomText(font, "TOM", nickSize); tomText.setFillColor(Color(220, 30, 30));   tomText.setOutlineColor(Color(80, 0, 0));      tomText.setOutlineThickness(3);
    FloatRect nb = nickText.getLocalBounds(), ab = andText.getLocalBounds(), tb2 = tomText.getLocalBounds();
    float totalWidth = nb.size.x + ab.size.x + tb2.size.x;
    float nickStartX = (windowW - totalWidth) / 2 - 30;
    nickText.setPosition({ nickStartX, nickY });
    andText.setPosition({ nickStartX + nb.size.x, nickY });
    tomText.setPosition({ nickStartX + nb.size.x + ab.size.x, nickY });
    window.draw(nickText); window.draw(andText); window.draw(tomText);

    float dividerY = nickY + nickSize + 20;
    RectangleShape divLeft;  divLeft.setSize({ (windowW / 2) - 180, 2 }); divLeft.setFillColor(Color(0, 180, 255, 160)); divLeft.setPosition({ 60, dividerY }); window.draw(divLeft);
    RectangleShape divRight; divRight.setSize({ (windowW / 2) - 180, 2 }); divRight.setFillColor(Color(0, 180, 255, 160)); divRight.setPosition({ windowW / 2 + 120, dividerY }); window.draw(divRight);
    RectangleShape sq; sq.setSize({ 10, 10 }); sq.setFillColor(Color(0, 200, 255)); sq.setPosition({ windowW / 2 - 6, dividerY - 4 }); window.draw(sq);

    float menuStartY = dividerY + 100;
    if ((int)(t * 2) % 2 == 0)
    {
        Text ps(font, "- PRESS ENTER TO CONTINUE -", 28);
        ps.setFillColor(Color(255, 255, 0));
        FloatRect pb = ps.getLocalBounds();
        ps.setPosition({ (windowW - pb.size.x) / 2, menuStartY });
        window.draw(ps);
    }
    menuStartY += 55;

    const char* options[4] = { "Login / Register", "Leaderboard", "Help", "Exit" };
    for (int i = 0; i < 4; i++)
    {
        float y = menuStartY + i * 75.f;
        Text item(font, options[i], 44);
        FloatRect ib = item.getLocalBounds();
        float x = (windowW - ib.size.x) / 2;

        if (i == selectedIndex)
        {
            float p = sinf(t * 5.f) * 0.5f + 0.5f;
            int   g = 200 + (int)(p * 55);
            item.setFillColor(Color(0, g, 255)); item.setOutlineColor(Color(0, 50, 120)); item.setOutlineThickness(2);
            Text la(font, ">>", 30); la.setFillColor(Color(255, 200, 0)); la.setPosition({ x - 90, y + 7 }); window.draw(la);
            Text ra(font, "<<", 30); ra.setFillColor(Color(255, 200, 0)); ra.setPosition({ x + ib.size.x + 55, y + 7 }); window.draw(ra);
            RectangleShape glow; glow.setSize({ ib.size.x + 40, 55 }); glow.setFillColor(Color(0, 80, 180, 60)); glow.setOutlineColor(Color(0, 150, 255, 100)); glow.setOutlineThickness(1); glow.setPosition({ x - 20, y - 2 }); window.draw(glow);
        }
        else { item.setFillColor(Color(160, 160, 160)); item.setOutlineColor(Color(50, 50, 50)); item.setOutlineThickness(2); }

        item.setPosition({ x, y }); window.draw(item);
    }

    RectangleShape creditLine; creditLine.setSize({ windowW - 200, 1 }); creditLine.setFillColor(Color(0, 150, 200, 100)); creditLine.setPosition({ 100, windowH - 70 }); window.draw(creditLine);
    Text c1(font, "Muhammad Abdullah Malik  -  25I-3006", 20); c1.setFillColor(Color(0, 180, 200)); FloatRect cb1 = c1.getLocalBounds(); c1.setPosition({ (windowW - cb1.size.x) / 2, windowH - 58 }); window.draw(c1);
    Text c2(font, "Muhammad Habib Ashfaq  -  25I-3025", 20); c2.setFillColor(Color(0, 180, 200)); FloatRect cb2 = c2.getLocalBounds(); c2.setPosition({ (windowW - cb2.size.x) / 2, windowH - 32 }); window.draw(c2);

    window.display();
}

// SPAWN GEM
void SnowBros::spawnGem(float x, float y, int value, bool hasTimer)
{
    for (int i = 0; i < MAX_GEMS; i++)
    {
        if (gems[i] == nullptr || !gems[i]->isActive())
        {
            delete gems[i];
            gems[i] = new Gem(x, y, value, hasTimer);
            return;
        }
    }
}

// UPDATE GEMS
void SnowBros::updateGems(float dt)
{
    for (int i = 0; i < MAX_GEMS; i++)
    {
        if (gems[i] == nullptr) continue;
        gems[i]->update(dt);
        if (!gems[i]->isActive())
        {
            delete gems[i];
            gems[i] = nullptr;
        }
    }
}

// DRAW GEMS
void SnowBros::drawGems()
{
    for (int i = 0; i < MAX_GEMS; i++)
        if (gems[i] != nullptr && gems[i]->isActive())
            gems[i]->draw(window);
}

// CHECK GEM COLLECTION
void SnowBros::checkGemCollection()
{
    for (int playerNum = 1; playerNum <= 2; playerNum++)
    {
        if (playerNum == 2 && !twoPlayerMode) continue;
        if (playerNum == 2 && player2 == nullptr) continue;

        Player* p = (playerNum == 1) ? player1 : player2;
        if (p == nullptr || p->getIsDead()) continue;

        FloatRect pr = p->getRect();

        for (int i = 0; i < MAX_GEMS; i++)
        {
            if (gems[i] == nullptr || !gems[i]->isActive()) continue;

            FloatRect gr = gems[i]->getRect();
            bool overlap = pr.position.x + pr.size.x > gr.position.x &&
                pr.position.x < gr.position.x + gr.size.x &&
                pr.position.y + pr.size.y > gr.position.y &&
                pr.position.y < gr.position.y + gr.size.y;

            if (overlap)
            {
                currentPlayer.gems += gems[i]->getValue();
                gems[i]->collect();
                saveProgress();
            }
        }
    }
}

// CLEAR GEMS
void SnowBros::clearGems()
{
    for (int i = 0; i < MAX_GEMS; i++)
    {
        delete gems[i];
        gems[i] = nullptr;
    }
}

// SPAWN POWER UP
// Randomly picks one of 4 concrete subclasses
void SnowBros::spawnPowerUp(float x, float y)
{
    int maxAllowed = 0;
    if (currentLevelNum <= 2) maxAllowed = 1;
    else if (currentLevelNum <= 4) maxAllowed = 2;
    else if (currentLevelNum <= 7) maxAllowed = 3;
    else                           maxAllowed = 4;

    int currentCount = 0;
    for (int i = 0; i < MAX_POWERUPS; i++)
        if (powerUps[i] != nullptr && powerUps[i]->isActive())
            currentCount++;

    if (currentCount >= maxAllowed) return;

    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (powerUps[i] == nullptr || !powerUps[i]->isActive())
        {
            delete powerUps[i];
            powerUps[i] = nullptr;

            // Randomly pick one of the 4 concrete subclasses
            // Polymorphism: all stored as PowerUp* base pointer
            int r = rand() % 4;
            if (r == 0) powerUps[i] = new SpeedBoost(x, y);
            else if (r == 1) powerUps[i] = new SnowballPower(x, y);
            else if (r == 2) powerUps[i] = new DistanceIncrease(x, y);
            else             powerUps[i] = new BalloonMode(x, y);

            return;
        }
    }
}

// UPDATE POWER UPS
void SnowBros::updatePowerUps(float dt)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (powerUps[i] == nullptr) continue;
        powerUps[i]->update(dt);
        if (!powerUps[i]->isActive())
        {
            delete powerUps[i];
            powerUps[i] = nullptr;
        }
    }
}

// DRAW POWER UPS
void SnowBros::drawPowerUps()
{
    for (int i = 0; i < MAX_POWERUPS; i++)
        if (powerUps[i] != nullptr && powerUps[i]->isActive())
            powerUps[i]->draw(window);
}

// CHECK POWER UP COLLECTION
void SnowBros::checkPowerUpCollection()
{
    for (int playerNum = 1; playerNum <= 2; playerNum++)
    {
        if (playerNum == 2 && !twoPlayerMode) continue;
        if (playerNum == 2 && player2 == nullptr) continue;

        Player* p = (playerNum == 1) ? player1 : player2;
        if (p == nullptr || p->getIsDead()) continue;

        FloatRect pr = p->getRect();

        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            if (powerUps[i] == nullptr || !powerUps[i]->isActive()) continue;

            FloatRect pur = powerUps[i]->getRect();
            bool overlap = pr.position.x + pr.size.x > pur.position.x &&
                pr.position.x < pur.position.x + pur.size.x &&
                pr.position.y + pr.size.y > pur.position.y &&
                pr.position.y < pur.position.y + pur.size.y;

            if (overlap)
            {
                powerUps[i]->applyTo(p);
                powerUps[i]->collect();
            }
        }
    }
}

// CLEAR POWER UPS
void SnowBros::clearPowerUps()
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        delete powerUps[i];
        powerUps[i] = nullptr;
    }
}

// START BONUS RAIN
// Called when all enemies die on level 4 or 9
// Spawns 10 FallingGem objects staggered above screen
void SnowBros::startBonusRain()
{
    bonusRainActive = true;
    bonusTimer = 0.f;
    clearFallingGems();

    for (int i = 0; i < MAX_FALLING_GEMS; i++)
    {
        // Random X across screen (avoid walls)
        float x = 100.f + (rand() % (int)(windowW - 200.f));
        // Staggered Y above screen so gems fall one after another
        float y = -50.f - (i * 100.f);
        fallingGems[i] = new FallingGem(x, y, windowW, windowH);
    }
}

// UPDATE BONUS RAIN
void SnowBros::updateBonusRain(float dt)
{
    bonusTimer += dt;

    // Update all falling gems
    for (int i = 0; i < MAX_FALLING_GEMS; i++)
    {
        if (fallingGems[i] == nullptr) continue;
        fallingGems[i]->update(dt);
    }

    // Check collection for player 1
    if (player1 && !player1->getIsDead())
    {
        FloatRect pr = player1->getRect();
        for (int i = 0; i < MAX_FALLING_GEMS; i++)
        {
            if (fallingGems[i] == nullptr || !fallingGems[i]->isActive()) continue;
            FloatRect gr = fallingGems[i]->getRect();
            bool overlap = pr.position.x + pr.size.x > gr.position.x &&
                pr.position.x < gr.position.x + gr.size.x &&
                pr.position.y + pr.size.y > gr.position.y &&
                pr.position.y < gr.position.y + gr.size.y;
            if (overlap)
            {
                fallingGems[i]->collect();
                currentPlayer.gems += 10;
                score += 1000;
                saveProgress();
            }
        }
    }

    // Check collection for player 2
    if (twoPlayerMode && player2 && !player2->getIsDead())
    {
        FloatRect pr = player2->getRect();
        for (int i = 0; i < MAX_FALLING_GEMS; i++)
        {
            if (fallingGems[i] == nullptr || !fallingGems[i]->isActive()) continue;
            FloatRect gr = fallingGems[i]->getRect();
            bool overlap = pr.position.x + pr.size.x > gr.position.x &&
                pr.position.x < gr.position.x + gr.size.x &&
                pr.position.y + pr.size.y > gr.position.y &&
                pr.position.y < gr.position.y + gr.size.y;
            if (overlap)
            {
                fallingGems[i]->collect();
                currentPlayer.gems += 10;
                score += 1000;
                saveProgress();
            }
        }
    }

    // Clean up collected gems
    for (int i = 0; i < MAX_FALLING_GEMS; i++)
    {
        if (fallingGems[i] != nullptr && !fallingGems[i]->isActive())
        {
            delete fallingGems[i];
            fallingGems[i] = nullptr;
        }
    }

    // Check end conditions: all collected OR 20 seconds passed
    bool allGone = true;
    for (int i = 0; i < MAX_FALLING_GEMS; i++)
        if (fallingGems[i] != nullptr) { allGone = false; break; }

    if (allGone || bonusTimer >= 20.f)
    {
        bonusRainActive = false;
        clearFallingGems();
        currentScreen = 8;   // go to level complete screen
    }
}

// DRAW BONUS RAIN
// Called from drawGameplay() when bonusRainActive is true
void SnowBros::drawBonusRain()
{
    // Draw all active falling gems
    for (int i = 0; i < MAX_FALLING_GEMS; i++)
        if (fallingGems[i] != nullptr && fallingGems[i]->isActive())
            fallingGems[i]->draw(window);

    // "BONUS LEVEL!" text at top center
    Text bonusText(font, "BONUS LEVEL!", 55);
    FloatRect tb = bonusText.getLocalBounds();
    bonusText.setFillColor(Color(255, 215, 0));
    bonusText.setOutlineColor(Color(150, 80, 0));
    bonusText.setOutlineThickness(4);
    bonusText.setPosition({ (windowW - tb.size.x) / 2.f, 50.f });
    window.draw(bonusText);

    // Timer bar background (grey)
    float barW = 400.f;
    float barH = 18.f;
    float barX = (windowW - barW) / 2.f;
    float barY = 118.f;

    RectangleShape barBg({ barW, barH });
    barBg.setFillColor(Color(60, 60, 60, 200));
    barBg.setOutlineColor(Color(255, 215, 0));
    barBg.setOutlineThickness(2);
    barBg.setPosition({ barX, barY });
    window.draw(barBg);

    // Timer bar fill - shrinks from full to zero over 20 seconds
    float pct = 1.f - (bonusTimer / 20.f);
    if (pct < 0.f) pct = 0.f;

    // Color changes: green â†’ yellow â†’ red as time runs out
    Color fillColor;
    if (pct > 0.5f)       fillColor = Color(0, 220, 80);
    else if (pct > 0.25f) fillColor = Color(255, 200, 0);
    else                  fillColor = Color(220, 50, 50);

    RectangleShape barFill({ barW * pct, barH });
    barFill.setFillColor(fillColor);
    barFill.setPosition({ barX, barY });
    window.draw(barFill);

    // "Collect gems!" hint text
    Text hint(font, "Collect the gems!", 20);
    FloatRect hb = hint.getLocalBounds();
    hint.setFillColor(Color(255, 255, 200));
    hint.setPosition({ (windowW - hb.size.x) / 2.f, 142.f });
    window.draw(hint);
}

// CLEAR FALLING GEMS
// Deletes all FallingGem objects and resets slots to nullptr
void SnowBros::clearFallingGems()
{
    for (int i = 0; i < MAX_FALLING_GEMS; i++)
    {
        delete fallingGems[i];
        fallingGems[i] = nullptr;
    }
}

// MAIN LOOP
void SnowBros::run()
{
    Clock gameClock;

    while (window.isOpen())
    {
        // AUTH 
        if (currentScreen == 1)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                auth->handleEvent(*event);
            }
            if (auth->isBackRequested()) { currentScreen = 0; auth->reset(); continue; }
            if (auth->isLoginSuccessful())
            {
                currentPlayer = auth->getLoggedInUser();
                // Re-read currentLevel from file to ensure it is the latest saved value.
                {
                    ifstream rIn("users.txt");
                    string rLine;
                    while (getline(rIn, rLine))
                    {
                        string rParts[6]; int rPi = 0; string rCur = "";
                        for (int j = 0; j < (int)rLine.size(); j++)
                        {
                            if (rLine[j] == '|') { if (rPi < 5) { rParts[rPi++] = rCur; rCur = ""; } }
                            else rCur += rLine[j];
                        }
                        rParts[rPi] = rCur;
                        if (rParts[0] != currentPlayer.username) continue;
                        int rLv = 0;
                        for (int k = 0; k < (int)rParts[4].size(); k++)
                            if (rParts[4][k] >= '0' && rParts[4][k] <= '9')
                                rLv = rLv * 10 + (rParts[4][k] - '0');
                        if (rLv > currentPlayer.currentLevel)
                            currentPlayer.currentLevel = rLv;
                    }
                    rIn.close();
                }
                mainMenu->init(windowW, windowH, &currentPlayer);
                currentScreen = 2; auth->reset(); continue;
            }
            auth->draw(window); continue;
        }

        // MAIN MENU 
        if (currentScreen == 2)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                mainMenu->handleEvent(*event);
            }
            int opt = mainMenu->getSelectedOption();
            if (opt == 1) { modeSelect->init(windowW, windowH, &currentPlayer); modeSelect->reset(); currentScreen = 4; mainMenu->reset(); continue; }
            if (opt == 2) {
                twoPlayerMode = false;
                currentLevelNum = currentPlayer.currentLevel;
                if (currentLevelNum < 1) currentLevelNum = 1;
                if (currentLevelNum > 10) currentLevelNum = 1;
                startGameplay(false, false);
                mainMenu->reset();
                continue;
            }
            if (opt == 3) { shop->init(windowW, windowH, &currentPlayer, 2); shop->reset(); currentScreen = 3; mainMenu->reset(); continue; }
            if (opt == 4) { settings->reset(); currentScreen = 11; mainMenu->reset(); continue; }
            if (opt == 5) { currentScreen = 0; mainMenu->reset(); continue; }
            mainMenu->draw(window); continue;
        }

        // SHOP 
        if (currentScreen == 3)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                shop->handleEvent(*event);
            }
            int boughtItem = shop->getSelectedItem();
            if (boughtItem >= 0 && player1 != nullptr)
            {
                if (boughtItem == 0) player1->addLife();
                if (boughtItem == 1) player1->activateSpeedBoost();
                if (boughtItem == 2) player1->activateSnowballPower();
                if (boughtItem == 3) player1->activateDistanceIncrease();
                if (boughtItem == 4) player1->activateBalloonMode();
                shop->clearSelectedItem();
            }
            if (shop->getSelectedOption() == -1) { int prev = shop->getPreviousScreen(); shop->reset(); currentScreen = prev; if (prev == 6) isPaused = true; continue; }
            shop->draw(window); continue;
        }

        // MODE SELECT 
        if (currentScreen == 4)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                modeSelect->handleEvent(*event);
            }
            int mode = modeSelect->getSelectedOption();
            if (mode == 1) {
                twoPlayerMode = false;
                // Re-read currentLevel from file before opening level select.
                // This ensures the displayed unlocked levels always match
                // what is actually saved, regardless of in-memory state.
                {
                    ifstream rIn("users.txt");
                    string rLine;
                    while (getline(rIn, rLine))
                    {
                        string rParts[6]; int rPi = 0; string rCur = "";
                        for (int j = 0; j < (int)rLine.size(); j++)
                        {
                            if (rLine[j] == '|') { if (rPi < 5) { rParts[rPi++] = rCur; rCur = ""; } }
                            else rCur += rLine[j];
                        }
                        rParts[rPi] = rCur;
                        if (rParts[0] != currentPlayer.username) continue;
                        int rLv = 0;
                        for (int k = 0; k < (int)rParts[4].size(); k++)
                            if (rParts[4][k] >= '0' && rParts[4][k] <= '9')
                                rLv = rLv * 10 + (rParts[4][k] - '0');
                        if (rLv > currentPlayer.currentLevel)
                            currentPlayer.currentLevel = rLv;
                    }
                    rIn.close();
                }
                levelSelect->init(windowW, windowH, &currentPlayer, false);
                levelSelect->reset();
                levelSelectReturnScreen = 4;
                currentScreen = 5;
                modeSelect->reset();
                continue;
            }
            if (mode == 2) {
                twoPlayerMode = true;
                {
                    ifstream rIn("users.txt");
                    string rLine;
                    while (getline(rIn, rLine))
                    {
                        string rParts[6]; int rPi = 0; string rCur = "";
                        for (int j = 0; j < (int)rLine.size(); j++)
                        {
                            if (rLine[j] == '|') { if (rPi < 5) { rParts[rPi++] = rCur; rCur = ""; } }
                            else rCur += rLine[j];
                        }
                        rParts[rPi] = rCur;
                        if (rParts[0] != currentPlayer.username) continue;
                        int rLv = 0;
                        for (int k = 0; k < (int)rParts[4].size(); k++)
                            if (rParts[4][k] >= '0' && rParts[4][k] <= '9')
                                rLv = rLv * 10 + (rParts[4][k] - '0');
                        if (rLv > currentPlayer.currentLevel)
                            currentPlayer.currentLevel = rLv;
                    }
                    rIn.close();
                }
                levelSelect->init(windowW, windowH, &currentPlayer, true);
                levelSelect->reset();
                levelSelectReturnScreen = 4;
                currentScreen = 5;
                modeSelect->reset();
                continue;
            }
            if (mode == -1) { currentScreen = 2; modeSelect->reset(); continue; }
            modeSelect->draw(window); continue;
        }

        // LEVEL SELECT 
        if (currentScreen == 5)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                levelSelect->handleEvent(*event);
            }
            int lvlOpt = levelSelect->getSelectedOption();
            if (lvlOpt >= 1 && lvlOpt <= 10)
            {
                currentLevelNum = levelSelect->getSelectedLevel();
                bool isNew = false;  // level select never resets saved progress
                startGameplay(twoPlayerMode, isNew);
                levelSelect->reset();
                gameClock.restart();
                continue;
            }
            if (lvlOpt == -1) {
                currentScreen = levelSelectReturnScreen;
                levelSelect->reset();
                continue;
            }
            levelSelect->draw(window); continue;
        }

        // GAMEPLAY
        if (currentScreen == 6)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                if (const auto* key = event->getIf<Event::KeyPressed>())
                {
                    if (key->code == Keyboard::Key::F1)
                        showHitBoxes = !showHitBoxes;

                    // When starPanelOpen, UP/DOWN navigate options,
                    // ENTER applies the chosen power-up and closes panel.
                    // Player1 star panel: W/S or Up/Down to navigate, Space/Enter to select
                    if (starPanelOpen)
                    {
                        if (key->code == Keyboard::Key::Up || key->code == Keyboard::Key::W)
                            starPanelSelected = (starPanelSelected - 1 + 3) % 3;
                        if (key->code == Keyboard::Key::Down || key->code == Keyboard::Key::S)
                            starPanelSelected = (starPanelSelected + 1) % 3;
                        if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space)
                        {
                            int chosen = starPanelOptions[starPanelSelected];
                            if (chosen == 0)      player1->activateSpeedBoostPermanent();
                            else if (chosen == 1) player1->activateSnowballPowerPermanent();
                            else if (chosen == 2) player1->activateDistanceIncreasePermanent();
                            else                  player1->activateBalloonModePermanent();
                            starPanelOpen = false;
                        }
                    }
                    // Player2 star panel: Numpad8/2 to navigate, Numpad0 to select
                    if (starPanelOpen2 && twoPlayerMode && player2)
                    {
                        if (key->code == Keyboard::Key::Numpad8)
                            starPanelSelected2 = (starPanelSelected2 - 1 + 3) % 3;
                        if (key->code == Keyboard::Key::Numpad2)
                            starPanelSelected2 = (starPanelSelected2 + 1) % 3;
                        if (key->code == Keyboard::Key::Numpad0)
                        {
                            int chosen = starPanelOptions2[starPanelSelected2];
                            if (chosen == 0)      player2->activateSpeedBoostPermanent();
                            else if (chosen == 1) player2->activateSnowballPowerPermanent();
                            else if (chosen == 2) player2->activateDistanceIncreasePermanent();
                            else                  player2->activateBalloonModePermanent();
                            starPanelOpen2 = false;
                        }
                    }
                }
                if (const auto* key = event->getIf<Event::KeyReleased>())
                {
                    if (key->code == Keyboard::Key::Escape)
                        isPaused = !isPaused;
                }
                if (isPaused)
                    pauseMenu->handleEvent(*event);
            }

            float dt = gameClock.restart().asSeconds();
            if (dt > 0.05f) dt = 0.05f;

            if (isPaused)
            {
                pauseMenu->update(dt);

                window.clear(Color::Black);
                currentLevel->draw(window);
                for (int i = 0; i < MAX_ENEMIES; i++)
                    if (enemies[i] != nullptr && enemies[i]->isAlive())
                        enemies[i]->draw(window);
                player1->draw(window);
                if (twoPlayerMode && player2) player2->draw(window);
                drawHUD();
                pauseMenu->draw(window);
                window.display();

                int pOpt = pauseMenu->getSelectedOption();
                if (pOpt == 1) { isPaused = false; pauseMenu->clearOption(); }
                if (pOpt == 2) { shop->init(windowW, windowH, &currentPlayer, 6); shop->reset(); currentScreen = 3; isPaused = false; pauseMenu->clearOption(); }
                if (pOpt == 3) { saveProgress(); pauseMenu->showSaveMessage(); pauseMenu->clearOption(); }
                if (pOpt == 4) {
                    saveProgress();
                    music.stop(); bossMusic.stop();
                    music.openFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
                    music.setLooping(true); music.play();
                    isPaused = false; pauseMenu->reset(); currentScreen = 0;
                }
                if (pOpt == 5) {
                    saveProgress();
                    music.stop(); bossMusic.stop();
                    music.openFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
                    music.setLooping(true); music.play();
                    isPaused = false; pauseMenu->reset();
                    currentScreen = 2;
                    mainMenu->init(windowW, windowH, &currentPlayer);
                    mainMenu->reset();
                }
            }
            else
            {
                // Star panel open = gameplay frozen during power-up selection
                if (!starPanelOpen)
                    updateGameplay(dt);
                if (currentScreen == 6) drawGameplay();
            }
            continue;
        }

        // GAME OVER
        if (currentScreen == 7)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();

                if (const auto* key = event->getIf<Event::KeyPressed>())
                {
                    if (key->code == Keyboard::Key::Up)
                    {
                        gameOverSelectedIndex = (gameOverSelectedIndex - 1 + 2) % 2;
                    }
                    if (key->code == Keyboard::Key::Down)
                    {
                        gameOverSelectedIndex = (gameOverSelectedIndex + 1) % 2;
                    }
                    if (key->code == Keyboard::Key::Enter)
                    {
                        if (gameOverSelectedIndex == 0)
                        {
                            // RESTART LEVEL
                            // Reset lives to default (2) and restart current level
                            saveHighScore();  // Save score before restart

                            // Reset music
                            bossMusic.stop();
                            music.stop();

                            // Restart same level as new game
                            startGameplay(twoPlayerMode, true);

                            // Reset game over state
                            gameOverSelectedIndex = 0;
                            continue;
                        }
                        else
                        {
                            // MAIN MENU
                            // Do NOT reset currentPlayer.currentLevel here.
                            // saveProgress() already uses max(current, saved)
                            // so the highest level reached is always preserved.
                            saveProgress();
                            bossMusic.stop();
                            music.stop();
                            music.openFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
                            music.setLooping(true);
                            music.play();
                            currentScreen = 2;
                            mainMenu->init(windowW, windowH, &currentPlayer);
                            mainMenu->reset();
                            gameOverSelectedIndex = 0;
                        }
                    }
                    // Keep Escape as quick exit to menu
                    if (key->code == Keyboard::Key::Escape)
                    {
                        // Do NOT reset currentPlayer.currentLevel here.
                        // saveProgress() preserves the highest level reached.
                        saveProgress();
                        bossMusic.stop();
                        music.stop();
                        music.openFromFile("SnowBrosAssets/Sounds/snow_bros_theme_01.ogg");
                        music.setLooping(true);
                        music.play();
                        currentScreen = 2;
                        mainMenu->init(windowW, windowH, &currentPlayer);
                        mainMenu->reset();
                        gameOverSelectedIndex = 0;
                    }
                }
            }
            drawGameOver();
            continue;
        }

        // LEVEL COMPLETE 
        if (currentScreen == 8)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                // Pass event to LevelComplete -- it handles ENTER skip/advance
                levelComplete.handleEvent(*event);
            }
            // Compute dt here the same way the gameplay block does --
            // gameClock is declared at the top of run() so it is accessible.
            float lcDt = gameClock.restart().asSeconds();
            if (lcDt > 0.05f) lcDt = 0.05f;
            // update() advances the typewriter timer each frame
            levelComplete.update(lcDt);
            // isDone() returns true after player confirms with ENTER
            if (levelComplete.isDone()) { advanceLevel(); continue; }
            levelComplete.draw(window, currentLevelNum, score);
            continue;
        }

        // LEADERBOARD 
        if (currentScreen == 9)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                leaderboard->handleEvent(*event);
            }
            if (leaderboard->getSelectedOption() == -1)
            {
                leaderboard->reset();
                currentScreen = 0;
                continue;
            }
            leaderboard->draw(window);
            continue;
        }
        //  HELP SCREEN 
        if (currentScreen == 10)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                helpScreen->handleEvent(*event);
            }
            if (helpScreen->getSelectedOption() == -1)
            {
                helpScreen->reset();
                currentScreen = 0;
                continue;
            }
            helpScreen->draw(window);
            continue;
        }

        // SETTINGS (screen 11)
        if (currentScreen == 11)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>()) window.close();
                settings->handleEvent(*event);
            }
            // Apply volumes every frame so changes are heard immediately.
            // getMusicVolume()/getSfxVolume() return 0-100 floats which
            // map directly to SFML's setVolume() parameter range.
            music.setVolume(settings->getMusicVolume());
            bossMusic.setVolume(settings->getMusicVolume());
            shootSnd->setVolume(settings->getSfxVolume());
            dieSnd->setVolume(settings->getSfxVolume());
            jumpSnd->setVolume(settings->getSfxVolume());
            enemyDieSnd->setVolume(settings->getSfxVolume());
            roarSnd->setVolume(settings->getSfxVolume());
            if (settings->getSelectedOption() == -1)
            {
                settings->reset();
                currentScreen = 2;
                continue;
            }
            settings->draw(window);
            continue;
        }

        // SPLASH SCREEN (screen == 0) 
        while (const auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>()) window.close();
            if (const auto* key = event->getIf<Event::KeyPressed>())
            {
                if (key->code == Keyboard::Key::Up)   selectedIndex = (selectedIndex - 1 + 4) % 4;
                if (key->code == Keyboard::Key::Down) selectedIndex = (selectedIndex + 1) % 4;
                if (key->code == Keyboard::Key::Enter)
                {
                    if (selectedIndex == 0) { currentScreen = 1; auth->reset(); }
                    if (selectedIndex == 1) { leaderboard->init(windowW, windowH); currentScreen = 9; }
                    if (selectedIndex == 2) { helpScreen->reset(); currentScreen = 10; }
                    if (selectedIndex == 3) window.close();
                }
                if (key->code == Keyboard::Key::Escape) window.close();
            }
        }
        drawMenu();
    }
}