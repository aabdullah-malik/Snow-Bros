#include "Levels.h"
#include <cmath>

Level::Level(int num, float w, float h)
{
    levelNum = num;
    windowW = w;
    windowH = h;
    platformCount = 0;
    bgSprite = nullptr;
    setupLevel();

    switch (levelNum)
    {
    case 1:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level1.png"); break;
    case 2:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level2.png"); break;
    case 3:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level3.png"); break;
    case 4:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level4.png"); break;
    case 5:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level5.png"); break;
    case 6:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level6.png"); break;
    case 7:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level7.png"); break;
    case 8:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level8.png"); break;
    case 9:  bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level9.png"); break;
    case 10: bgTexture.loadFromFile("SnowBrosAssets/Backgrounds/level10.png"); break;
    }

    bgSprite = new Sprite(bgTexture);
    float scaleX = windowW / bgTexture.getSize().x;
    float scaleY = windowH / bgTexture.getSize().y;
    bgSprite->setScale({ scaleX, scaleY }); // resizes the sprite to fill exactly the screen
    bgSprite->setPosition({ 0, 0 });
}

void Level::setupLevel()
{
    switch (levelNum)
    {
        // LEVEL 1 - Snowy Village
    case 1:
        platformCount = 8;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(60, 100, 160));
        // bottom left
        platforms[1] = Platform(80, windowH - 200, 240, 45, Color(60, 100, 160));
        // bottom right
        platforms[2] = Platform(windowW - 320, windowH - 200, 240, 45, Color(60, 100, 160));
        // bottom center
        platforms[3] = Platform((windowW - 300) / 2, windowH - 300, 300, 45, Color(60, 100, 160));
        // center 
        platforms[4] = Platform((windowW - 600) / 2, windowH - 300, 600, 45, Color(60, 100, 160));
        // upper right
        platforms[5] = Platform(windowW - 500, windowH - 450, 420, 45, Color(60, 100, 160));
        // upper left
        platforms[6] = Platform(80, windowH - 450, 420, 45, Color(60, 100, 160));
        // top mid
        platforms[7] = Platform((windowW - 800) / 2, windowH - 600, 800, 45, Color(60, 100, 160));

        break;

        // LEVEL 2 - Ice Caves 
    case 2:
        platformCount = 7;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(0, 100, 180));
        // bottom mid 
        platforms[1] = Platform((windowW - 300) / 2, windowH - 230, 300, 45, Color(0, 100, 180));
        // mid left
        platforms[2] = Platform(80, windowH - 380, 440, 45, Color(0, 100, 180));
        // mid right
        platforms[3] = Platform(windowW - 520, windowH - 380, 440, 45, Color(0, 100, 180));
        // center
        platforms[4] = Platform((windowW - 600) / 2, windowH - 550, 600, 45, Color(0, 100, 180));
        // top left
        platforms[5] = Platform(80, windowH - 780, 800, 45, Color(0, 100, 180));
        // top right
        platforms[6] = Platform((windowW - 880), windowH - 780, 800, 45, Color(0, 100, 180));
        break;

        // LEVEL 3 - Green Forest
    case 3:
        platformCount = 6;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(0, 80, 40));
        // bottom left
        platforms[1] = Platform(80, windowH - 280, 800, 45, Color(0, 80, 40));
        // bottom right
        platforms[2] = Platform(windowW - 880, windowH - 430, 800, 45, Color(0, 80, 40));
        // top left
        platforms[3] = Platform(80, windowH - 560, 360, 45, Color(0, 80, 40));
        // top mid-right
        platforms[4] = Platform(windowW - 640, windowH - 600, 360, 45, Color(0, 80, 40));
        // top 
        platforms[5] = Platform(80, windowH - 760, 500, 45, Color(0, 80, 40));
        break;

        // LEVEL 4 - Frozen Lake 
    case 4:
        platformCount = 8;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(0, 160, 200));
        // bottom mid
        platforms[1] = Platform((windowW - 250) / 2, windowH - 280, 250, 45, Color(0, 160, 200));
        // bottom left
        platforms[2] = Platform(80, windowH - 280, 250, 45, Color(0, 160, 200));
        // bottom right
        platforms[3] = Platform(windowW - 330, windowH - 280, 250, 45, Color(0, 160, 200));
        // mid
        platforms[4] = Platform((windowW - 450) / 2, windowH - 440, 450, 45, Color(0, 160, 200));
        // top
        platforms[5] = Platform((windowW - 650) / 2, windowH - 600, 650, 45, Color(0, 160, 200));
        // top left
        platforms[6] = Platform(80, windowH - 760, 250, 45, Color(0, 160, 200));
        // top right
        platforms[7] = Platform(windowW - 330, windowH - 760, 250, 45, Color(0, 160, 200));
        break;

        // LEVEL 5 - Boss (Mogera) Dark Lair
    case 5:
        platformCount = 7;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(60, 0, 0));
        // bottom left
        platforms[1] = Platform(200, windowH - 250, 750, 45, Color(60, 0, 0));
        // above bottom
        platforms[2] = Platform(80, windowH - 430, 750, 45, Color(60, 0, 0));
        // mid
        platforms[3] = Platform(200, windowH - 610, 750, 45, Color(60, 0, 0));
        // top
        platforms[4] = Platform(80, windowH - 790, 750, 45, Color(60, 0, 0));
        // boss place
        platforms[5] = Platform(windowW - 480, windowH - 300, 400, 45, Color(60, 0, 0));
        platforms[6] = Platform(windowW - 480, windowH - 345, 400, 45, Color(60, 0, 0));
        break;

        // LEVEL 6 - Volcanic Wasteland 
    case 6:
        platformCount = 8;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(150, 40, 0));
        // bottom left
        platforms[1] = Platform(80, windowH - 250, 700, 45, Color(150, 40, 0));
        // bottom right
        platforms[2] = Platform(windowW - 380, windowH - 250, 300, 45, Color(150, 40, 0));
        // mid left
        platforms[3] = Platform(80, windowH - 430, 500, 45, Color(150, 40, 0));
        // mid right
        platforms[4] = Platform(windowW - 580, windowH - 430, 500, 45, Color(150, 40, 0));
        // top left
        platforms[5] = Platform(80, windowH - 610, 300, 45, Color(150, 40, 0));
        // top right
        platforms[6] = Platform(windowW - 780, windowH - 610, 700, 45, Color(150, 40, 0));
        // toppest
        platforms[7] = Platform((windowW - 800) / 2, windowH - 770, 800, 45, Color(150, 40, 0));
        break;

        // LEVEL 7 - Haunted Castle 
    case 7:
        platformCount = 7;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(60, 0, 80));
        // bottom left
        platforms[1] = Platform(80, windowH - 250, 500, 45, Color(60, 0, 80));
        // bottom right
        platforms[2] = Platform(windowW - 580, windowH - 250, 500, 45, Color(60, 0, 80));
        // middle
        platforms[3] = Platform((windowW - 500) / 2, windowH - 430, 500, 45, Color(60, 0, 80));
        // top left
        platforms[4] = Platform(200, windowH - 610, 300, 45, Color(60, 0, 80));
        // top right
        platforms[5] = Platform(windowW - 480, windowH - 610, 300, 45, Color(60, 0, 80));
        // top
        platforms[6] = Platform((windowW - 500) / 2, windowH - 790, 500, 45, Color(60, 0, 80));
        break;

        // LEVEL 8 - Alien Planet 
    case 8:
        platformCount = 8;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(60, 0, 100));
        // bottom left
        platforms[1] = Platform(180, windowH - 270, 550, 45, Color(60, 0, 100));
        // bottom right
        platforms[2] = Platform(windowW - 700, windowH - 270, 550, 45, Color(60, 0, 100));
        // mid left
        platforms[3] = Platform(180, windowH - 470, 550, 45, Color(60, 0, 100));
        // mid right
        platforms[4] = Platform(windowW - 700, windowH - 470, 550, 45, Color(60, 0, 100));
        // top left
        platforms[5] = Platform(180, windowH - 670, 550, 45, Color(60, 0, 100));
        // top right
        platforms[6] = Platform(windowW - 700, windowH - 670, 550, 45, Color(60, 0, 100));
        // top 
        platforms[7] = Platform((windowW - 650) / 2, windowH - 870, 650, 45, Color(60, 0, 100));
        break;

        // LEVEL 9 - Storm Clouds
    case 9:
        platformCount = 8;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(60, 60, 80));
        // bottom left
        platforms[1] = Platform(80, windowH - 220, 350, 45, Color(60, 60, 80));
        // bottom mid
        platforms[2] = Platform((windowW - 350) / 2, windowH - 270, 350, 45, Color(60, 60, 80));
        // bottom right
        platforms[3] = Platform(windowW - 430, windowH - 270, 350, 45, Color(60, 60, 80));
        // mid left
        platforms[4] = Platform(80, windowH - 470, 350, 45, Color(60, 60, 80));
        // mid middle
        platforms[5] = Platform((windowW - 350) / 2, windowH - 470, 350, 45, Color(60, 60, 80));
        // mid right
        platforms[6] = Platform(windowW - 430, windowH - 470, 350, 45, Color(60, 60, 80));
        // top
        platforms[7] = Platform((windowW - 600) / 2, windowH - 670, 600, 45, Color(60, 60, 80));
        break;

        // LEVEL 10 - Boss (Gamakichi) Arena
    case 10:
        platformCount = 8;
        // floor
        platforms[0] = Platform(80, windowH - 70, windowW - 160, 70, Color(80, 0, 0));
        // bottom left
        platforms[1] = Platform(130, windowH - 250, 350, 45, Color(80, 0, 0));
        // bottom right
        platforms[2] = Platform(windowW - 430, windowH - 250, 350, 45, Color(80, 0, 0));
        // middle left
        platforms[3] = Platform(80, windowH - 430, 500, 45, Color(80, 0, 0));
        // middle right
        platforms[4] = Platform(windowW - 580, windowH - 430, 500, 45, Color(80, 0, 0));
        // top
        platforms[5] = Platform((windowW - 650) / 2, windowH - 610, 650, 45, Color(80, 0, 0));
        // top left
        platforms[6] = Platform(80, windowH - 790, 650, 45, Color(80, 0, 0));
        // top right
        platforms[7] = Platform(windowW - 730, windowH - 790, 650, 45, Color(80, 0, 0));
        break;
    }
}

void Level::drawBoundaries(RenderWindow& window)
{
    float wallW = 80;

    // wall color per level
    Color wallBase;
    Color wallAccent;   // for decoration
    Color wallCenter;

    switch (levelNum)
    {
    case 1:
        wallBase = Color(0, 40, 80);
        wallAccent = Color(0, 150, 200);
        wallCenter = Color(0, 80, 120);
        break;
    case 2:
        wallBase = Color(0, 40, 80);
        wallAccent = Color(0, 150, 200);
        wallCenter = Color(0, 80, 120);
        break;
    case 3:
        wallBase = Color(20, 60, 20);
        wallAccent = Color(100, 180, 0);
        wallCenter = Color(0, 100, 50);
        break;
    case 4:
        wallBase = Color(0, 60, 90);
        wallAccent = Color(0, 200, 220);
        wallCenter = Color(0, 100, 130);
        break;
    case 5:
        wallBase = Color(60, 0, 0);
        wallAccent = Color(200, 80, 0);
        wallCenter = Color(100, 0, 0);
        break;
    case 6:
        wallBase = Color(80, 30, 0);
        wallAccent = Color(220, 100, 0);
        wallCenter = Color(120, 40, 0);
        break;
    case 7:
        wallBase = Color(40, 0, 60);
        wallAccent = Color(150, 0, 200);
        wallCenter = Color(80, 0, 100);
        break;
    case 8:
        wallBase = Color(40, 0, 60);
        wallAccent = Color(180, 0, 200);
        wallCenter = Color(80, 0, 120);
        break;
    case 9:
        wallBase = Color(30, 30, 50);
        wallAccent = Color(100, 100, 180);
        wallCenter = Color(50, 50, 80);
        break;
    case 10:
        wallBase = Color(50, 0, 0);
        wallAccent = Color(200, 0, 0);
        wallCenter = Color(80, 0, 0);
        break;
    }

    // LEFT WALL

    // base
    RectangleShape leftWall;
    leftWall.setSize({ wallW, windowH });
    leftWall.setFillColor(wallBase);
    leftWall.setPosition({ 0, 0 });
    window.draw(leftWall);

    // center vertical strip
    RectangleShape leftCenter;
    leftCenter.setSize({ wallW * 0.25f, windowH });
    leftCenter.setFillColor(wallCenter);
    leftCenter.setPosition({ wallW * 0.375f, 0 });  // this makes it sit at the center of the wall
    window.draw(leftCenter);

    // horizontal divider lines
    for (float y = 80; y < windowH; y += 80)
    {
        RectangleShape divider;
        divider.setSize({ wallW, 2 });
        divider.setFillColor(Color(wallAccent.r, wallAccent.g, wallAccent.b, 150));
        divider.setPosition({ 0, y });
        window.draw(divider);
    }

    // right edge bright line
    RectangleShape leftBright;
    leftBright.setSize({ 3, windowH });
    leftBright.setFillColor(Color(wallAccent.r, wallAccent.g, wallAccent.b, 200));
    leftBright.setPosition({ wallW - 3, 0 });
    window.draw(leftBright);

    // RIGHT WALL 

    float rightX = windowW - wallW;

    // base
    RectangleShape rightWall;
    rightWall.setSize({ wallW, windowH });
    rightWall.setFillColor(wallBase);
    rightWall.setPosition({ rightX, 0 });
    window.draw(rightWall);

    // center vertical strip
    RectangleShape rightCenter;
    rightCenter.setSize({ wallW * 0.25f, windowH });
    rightCenter.setFillColor(wallCenter);
    rightCenter.setPosition({ rightX + wallW * 0.375f, 0 });
    window.draw(rightCenter);


    // horizontal dividers
    for (float y = 80; y < windowH; y += 80)
    {
        RectangleShape divider;
        divider.setSize({ wallW, 2 });
        divider.setFillColor(Color(wallAccent.r, wallAccent.g, wallAccent.b, 150));
        divider.setPosition({ rightX, y });
        window.draw(divider);
    }

    // left edge bright line
    RectangleShape rightBright;
    rightBright.setSize({ 3, windowH });
    rightBright.setFillColor(Color(wallAccent.r, wallAccent.g, wallAccent.b, 200));
    rightBright.setPosition({ rightX, 0 });
    window.draw(rightBright);
}

void Level::drawBackground(RenderWindow& window)
{
    if (bgSprite != nullptr)
    {
        window.draw(*bgSprite);
    }
}

void Level::draw(RenderWindow& window)
{
    drawBackground(window);
    drawBoundaries(window);
    for (int i = 0; i < platformCount; i++)
    {
        platforms[i].draw(window);
    }
}

Platform* Level::getPlatforms()  // returns an array of platforms on that specific level
{
    return platforms;
}

int Level::getPlatformCount()
{
    return platformCount;
}

Level::~Level()
{
    delete bgSprite;
    bgSprite = nullptr;
}