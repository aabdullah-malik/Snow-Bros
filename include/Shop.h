/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#pragma once

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include "User.h"

using namespace sf;
using namespace std;

class Shop
{
private:
    Font* font;
    float    windowW;
    float    windowH;
    Clock    clock;
    User* player;

    // 0 = nothing   -1 = go back
    int      selectedOption;
    int      selectedItem;     // -1 = nothing bought, 0-4 = item index

    string   message;
    bool     messageIsError;

    Texture  itemsTexture;
    Sprite* itemSprite;
    bool     textureLoaded;
    int      previousScreen;
    

    const char* itemNames[5] =
    {
        "Extra Life", "Speed Boost", "Snowball Power", "Distance Boost", "Balloon Mode"
    };

    int itemCosts[5] = { 50, 20, 30, 25, 35 };

    IntRect iconRects[5] =
    {
        IntRect({ 301, 643 }, { 191, 165 }),
        IntRect({ 345,  25 }, { 109, 143 }),
        IntRect({ 341, 179 }, { 119, 147 }),
        IntRect({ 341, 339 }, { 115, 141 }),
        IntRect({ 331, 493 }, { 135, 149 })
    };

    float cardW = 200;
    float cardH = 280;
    float cardGap = 40;

    float cardX[5];
    float cardY[5];

    float buyBtnW = 130;
    float buyBtnH = 40;

    string intToStr(int n);
    void   updateGemsInFile();
    void   drawShop(RenderWindow& window, float t);
    void   drawCard(RenderWindow& window, float x, float y, int index,
        float mouseX, float mouseY, float t);
    bool   isMouseOver(float mx, float my, float rx, float ry, float rw, float rh);

public:
    Shop(Font& font, float windowW, float windowH, User* player);
    void init(float w, float h, User* p, int prevScreen);
    void handleEvent(const Event& event);
    void draw(RenderWindow& window);
    int  getSelectedOption();
    int  getSelectedItem() { return selectedItem; }
    void clearSelectedItem() { selectedItem = -1; }
    int getPreviousScreen() { return previousScreen; }
    void reset();
    ~Shop();
};