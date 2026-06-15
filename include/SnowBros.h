/* Muhammad Abdullah Malik  25I-3006
   Muhammad Habib Ashfaq    25I-3025  SE-D */
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Auth.h"
#include "MainMenu.h"
#include "Shop.h"
#include "ModeSelect.h"
#include "LevelSelect.h"
#include "Player.h"
#include "Nick.h"
#include "Tom.h"
#include "Enemy.h"
#include "Botom.h"
#include "FlyingFoogaFoog.h"
#include "Tornado.h"
#include "Mogera.h"
#include "Gamakichi.h"
#include "MogeraChild.h"
#include "GamakichiCreature.h"
#include "GamakichiBomb.h"
#include "Levels.h"
#include "PauseMenu.h"
#include "Leaderboard.h"
#include "User.h"
#include "Gem.h"
#include "Powers.h"
#include "HelpScreen.h"
#include "LevelComplete.h"
#include "StarPowerUp.h"
#include "Settings.h"

using namespace sf;
using namespace std;

// Screen IDs
// 0 = splash   1 = auth        2 = main menu
// 3 = shop     4 = mode select  5 = level select
// 6 = gameplay  7 = game over   8 = level complete
// 9 = leaderboard

class SnowBros
{
private:
    RenderWindow window;
    Font font;
    Music music;
    Clock clock;

    // Sound effects
    SoundBuffer shootBuf, dieBuf, jumpBuf, enemyDieBuf, roarBuf;
    Sound* shootSnd;
    Sound* dieSnd;
    Sound* jumpSnd;
    Sound* enemyDieSnd;
    Sound* roarSnd;
    Music bossMusic;
    bool isBossLevel;

    // Sound edge-detect flags
    bool p1WasThrowing;
    bool p2WasThrowing;
    bool mogeraWasRoaring;
    bool gamakichiWasRoaring;

    float windowW;
    float windowH;

    int currentScreen;
    int selectedIndex;           // splash screen cursor
    int levelSelectReturnScreen;

    // UI screens
    Auth* auth;
    MainMenu* mainMenu;
    Shop* shop;
    ModeSelect* modeSelect;
    LevelSelect* levelSelect;
    PauseMenu* pauseMenu;      
    Leaderboard* leaderboard;   
    HelpScreen* helpScreen;
    Settings* settings;

    // Logged-in player
    User currentPlayer;

    // Gameplay 
    Player* player1;
    Player* player2;
    bool twoPlayerMode;

    static const int MAX_ENEMIES = 20;
    Enemy* enemies[MAX_ENEMIES];
    int enemyCount;

    Level* currentLevel;
    int currentLevelNum;
    bool isNewGame;

    bool showHitBoxes;
    bool isPaused;             

    int gameOverSelectedIndex;  // 0 = Restart Level, 1 = Main Menu
    bool gameOverEnterPressed;   // Track if Enter was pressed this frame


    // HUD / score
    int score;
    LevelComplete levelComplete;  // separate class handles level complete screen + story narration

    // Gem system
    static const int MAX_GEMS = 100;
    Gem* gems[MAX_GEMS];
    // Bonus level gem rain 
    static const int MAX_FALLING_GEMS = 10;
    FallingGem* fallingGems[MAX_FALLING_GEMS];
    bool isBonusLevel;
    bool bonusRainActive;
    float bonusTimer;
    static const int MAX_POWERUPS = 20;
    PowerUp* powerUps[MAX_POWERUPS];
    bool  bossRewardGiven;

    static const int STAR_LEVEL = 7;
    StarPowerUp* starPowerUp;
    bool starPanelOpen;
    int starPanelOptions[3];
    int starPanelSelected;

    StarPowerUp* starPowerUp2;         // player2's star (multiplayer only)
    bool starPanelOpen2;
    int starPanelOptions2[3];
    int starPanelSelected2;

public:
    SnowBros();

    void spawnPowerUp(float x, float y);
    void updatePowerUps(float dt);
    void drawPowerUps();
    void checkPowerUpCollection();
    void clearPowerUps();

    void spawnGem(float x, float y, int value, bool hasTimer = false);
    void updateGems(float dt);
    void drawGems();
    void checkGemCollection();
    void clearGems();

    void startBonusRain();
    void updateBonusRain(float dt);
    void drawBonusRain();
    void clearFallingGems();

    void spawnEnemiesForLevel(int levelNum);
    void clearEnemies();
    void startGameplay(bool multiPlayer, bool newGame);
    void advanceLevel();

    void updateGameplay(float dt);
    void drawGameplay();
    void drawHUD();
    void drawMenu();
    void drawGameOver();
    void drawStarPanel();
    void drawStarPanel2();   // player2's independent star panel

    void checkSnowballEnemyCollision();
    void checkPlayerEnemyCollision();
    void checkRollingSnowballChain();
    void checkPlayerPushSnowball();

    void saveProgress();
    void saveHighScore();
    void applyRollHit(Enemy* roller, Enemy* victim);

    ~SnowBros();
    void run();
};