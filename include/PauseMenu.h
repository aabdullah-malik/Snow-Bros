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

class PauseMenu
{
private:
    Font* font;
    float  windowW;
    float  windowH;
    Clock  clock;
    User* player;

    // -------------------------------------------------------
    // Result codes:
    //  0  = nothing happened yet
    //  1  = Resume
    //  2  = Shop
    //  3  = Save Game (stay paused)
    //  4  = Logout    (go to screen 0)
    //  5  = Main Menu (go to screen 2)
    // -------------------------------------------------------
    int    selectedOption;

    // Keyboard navigation - which button is currently highlighted
    int    selectedBtn;

    // Save confirmation message
    string saveMessage;
    float  saveMessageTimer;
    float  saveMessageDuration;

    // Panel dimensions
    float  panelW;
    float  panelH;
    float  panelX;
    float  panelY;

    // 5 buttons
    static const int BTN_COUNT = 5;
    float  btnX[BTN_COUNT];
    float  btnY[BTN_COUNT];
    float  btnW;
    float  btnH;

    // -------------------------------------------------------
    // Private helpers
    // -------------------------------------------------------
    void calculateButtonPositions();
    void drawOverlay(RenderWindow& window);
    void drawPanel(RenderWindow& window, float t);
    void drawButton(RenderWindow& window,
        int index,
        const string& label,
        float t);

public:
    PauseMenu(Font& font, float windowW, float windowH, User* player);

    void init(float w, float h, User* p);
    void handleEvent(const Event& event);
    void draw(RenderWindow& window);

    int  getSelectedOption();
    void clearOption();
    void reset();
    void showSaveMessage();
    void update(float dt);

    ~PauseMenu() {}
};