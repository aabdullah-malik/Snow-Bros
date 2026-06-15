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

class ModeSelect
{
private:
    Font* font;
    float    windowW;
    float    windowH;
    Clock    clock;
    User* player;

    //  0 = nothing   1 = single player   2 = multiplayer   -1 = back
    int      selectedOption;

    Texture  nickTexture;
    Texture  tomTexture;
    Sprite* nickSprite;
    Sprite* tomSprite;
    bool     texturesLoaded;

    float panelW, panelH;
    float leftPanelX, leftPanelY;
    float rightPanelX, rightPanelY;
    float btnW, btnH;

    bool isMouseOver(float mx, float my, float rx, float ry, float rw, float rh);
    void drawModeSelect(RenderWindow& window, float t);
    void drawLeftPanel(RenderWindow& window, float mouseX, float mouseY, float t);
    void drawRightPanel(RenderWindow& window, float mouseX, float mouseY, float t);

public:
    ModeSelect(Font& font, float windowW, float windowH, User* player);
    void init(float w, float h, User* p);
    void handleEvent(const Event& event);
    void draw(RenderWindow& window);
    int  getSelectedOption();
    void reset();
    ~ModeSelect();
};