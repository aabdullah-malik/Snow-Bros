/* Muhammad Abdullah Malik  25I-3006
   Muhammad Habib Ashfaq    25I-3025  SE-D */

#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

// Settings
// A separate screen accessible from the main menu (screen 11).
// Provides polished volume controls for music and sound effects.
//
// Two independent sliders:
//   musicVolume -- controls Music objects (background + boss music)
//   sfxVolume   -- controls Sound objects (shoot, die, jump, roar etc.)
//
// Both range from 0.0 (silent) to 100.0 (full) matching SFML's
// Music::setVolume() and Sound::setVolume() parameter range exactly.
//
// How SnowBros uses it:
//   settings->handleEvent(*event)    pass events
//   settings->draw(window)          render
//   settings->getMusicVolume()       apply to Music objects each frame
//   settings->getSfxVolume()         apply to Sound objects each frame
//   settings->getSelectedOption()    -1 = back to main menu
//   settings->reset()               reset selection on open


class Settings
{
private:
    Font* font;
    float  windowW;
    float  windowH;

    // Volume values  range 0.0 to 100.0 matching SFML's setVolume() range.
    // Stored as floats so LEFT/RIGHT key steps of 5.0 are precise.
    float  musicVolume;
    float  sfxVolume;

    // selectedIndex which row is highlighted
    //   0 = Music Volume row
    //   1 = SFX Volume row
    //   2 = Back button
    int    selectedIndex;

    // selectedOption what action was confirmed
    //   0  = nothing selected yet
    //   -1 = Back (return to main menu)
    int    selectedOption;

    // Clock for blinking cursor on selected row
    Clock  blinkClock;

    // Helper: draws one volume row (label + bar + percentage + hint)
    // barColor = color of the fill bar and label when selected
    // isSelected = whether this row is currently highlighted
    void drawVolumeRow(RenderWindow& window,
        const string& label,
        float volume,
        float rowY,
        Color barColor,
        bool  isSelected);

public:
    Settings(Font& f, float w, float h);

    // handleEvent  UP/DOWN changes selectedIndex,
    // LEFT/RIGHT adjusts the selected volume slider,
    // ENTER/ESC on Back row sets selectedOption = -1
    void handleEvent(const Event& event);

    // draw  renders the full settings screen
    void draw(RenderWindow& window);

    // getSelectedOption  SnowBros polls this each frame
    int  getSelectedOption() const { return selectedOption; }

    // reset  call when opening settings screen
    void reset();

    // Volume getters SnowBros calls these every frame on screen 11
    // and applies values directly to Music/Sound objects
    float getMusicVolume() const { return musicVolume; }
    float getSfxVolume()   const { return sfxVolume; }

    ~Settings() {}
};