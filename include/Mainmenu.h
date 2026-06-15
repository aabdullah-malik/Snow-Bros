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

class MainMenu
{
private:
    Font* font;
    float  windowW;
    float  windowH;
    Clock  clock;
    User* player;

    // 0=New Game  1=Continue  2=Shop  3=Settings  4=Logout
    int selectedIndex;

    // 0=nothing  1=New Game  2=Continue  3=Shop  4=Settings  5=Logout
    int selectedOption;

    void drawMainMenu(RenderWindow& window, float t);

public:
    MainMenu(Font& font, float windowW, float windowH, User* player);
    void init(float w, float h, User* p);
    void handleEvent(const Event& event);
    void draw(RenderWindow& window);
    int  getSelectedOption();
    void reset();
};