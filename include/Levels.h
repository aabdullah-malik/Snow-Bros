#pragma once
#include <SFML/Graphics.hpp>
#include "Platform.h"
using namespace sf;

class Level
{
private:
    static const int MAX_PLATFORMS = 10; 
    Platform platforms[MAX_PLATFORMS];
    int platformCount;
    int levelNum;
    float windowW;
    float windowH;
    Texture bgTexture;
    Sprite* bgSprite;
    Clock clock;

public:
    Level(int num, float w, float h);
    
    void draw(RenderWindow& window);
    void drawBoundaries(RenderWindow& window);
    void drawBackground(RenderWindow& window);
    Platform* getPlatforms();
    int getPlatformCount();
    void setupLevel();
    ~Level();
};