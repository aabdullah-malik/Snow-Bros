#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;


// HelpScreen - shows game instructions on a single page
// Accessed from splash screen as the 4th option
// ESC goes back to splash screen

class HelpScreen
{
private:
    Font* font;
    float  windowW;
    float  windowH;
    Clock  clock;
    int    selectedOption;  // 0 = nothing, -1 = go back

    

public:
    HelpScreen(Font& f, float w, float h);

    void handleEvent(const Event& event);
    void draw(RenderWindow& window);
    void reset();
    int  getSelectedOption();
};