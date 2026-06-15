/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "User.h"

using namespace sf;
using namespace std;

class LevelSelect
{
private:
    Font* font;
    float  windowW;
    float  windowH;
    Clock  clock;
    User* player;
    bool   twoPlayerMode;

    // 0 = nothing   -1 = back   1-10 = level chosen
    int    selectedOption;

    // 10 cards in a 2-row grid (row1: levels 1-5, row2: levels 6-10)
    static const int CARD_COUNT = 10;
    float  cardX[CARD_COUNT];
    float  cardY[CARD_COUNT];
    float  cardW;
    float  cardH;

    // Level background textures - one per level
    Texture  bgTextures[CARD_COUNT];
    Sprite* bgSprites[CARD_COUNT];

    // Boss sprites for level 5 (Mogera) and level 10 (Gamakichi)
    Texture  mogenTexture;
    Texture  gamaTexture;
    Sprite* mogenSprite;
    Sprite* gamaSprite;

    bool     texturesLoaded;

    // Private helpers

    bool   isMouseOver(float mx, float my,
        float rx, float ry,
        float rw, float rh);
    string intToStr(int n);
    void   drawCard(RenderWindow& window,
        int index,
        float mouseX, float mouseY,
        float t);
    void   drawLock(RenderWindow& window,
        float cx, float cy);
    void   drawLevelSelect(RenderWindow& window, float t);

public:
    LevelSelect(Font& font, float windowW, float windowH, User* player);
    void init(float w, float h, User* p, bool isTwoPlayer);
    void handleEvent(const Event& event);
    void draw(RenderWindow& window);
    int  getSelectedOption();
    int  getSelectedLevel();
    void reset();
    ~LevelSelect();
};