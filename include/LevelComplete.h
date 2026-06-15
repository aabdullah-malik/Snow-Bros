/* Muhammad Abdullah Malik  25I-3006
   Muhammad Habib Ashfaq    25I-3025  SE-D */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;


// LevelComplete
// Handles the level complete screen as a fully self-contained class.
// Responsibilities:
//   - Stores 10 story lines (one per level)
//   - Runs the typewriter effect (reveals text char by char)
//   - Tracks whether the player has pressed ENTER to advance
//   - Draws everything: background, title, story, score, hint
//
// How SnowBros uses it:
//   levelComplete.reset()               -- call when level is cleared
//   levelComplete.update(dt)            -- call every frame
//   levelComplete.handleEvent(*event)   -- pass events to it
//   levelComplete.draw(window, lvl, score) -- call to render
//   levelComplete.isDone()              -- true when ready to advance

class LevelComplete
{
private:
    Font* font;       // pointer to shared font -- we do not own it
    float  windowW;    // screen width (for centering text)
    float  windowH;    // screen height

    // Typewriter state
    // typewriterTimer counts up every frame via update(dt).
    // charsToShow = (int)(typewriterTimer * CHARS_PER_SEC)
    // We reveal the story string one char at a time until fully shown.
    float  typewriterTimer;
    static const int CHARS_PER_SEC = 30;  // 30 characters revealed per second

    // done = true after player presses ENTER once the full line is shown.
    // SnowBros checks isDone() each frame to decide when to advance.
    bool   done;

    // Clock for the blinking PRESS ENTER hint animation
    Clock  blinkClock;

    // ─── Story Lines
    // One line per level, index 0 = level 1, index 9 = level 10.
    // These are const char* so no STL string management is needed.
    static const char* storyLines[10];

public:
    // Default constructor needed because LevelComplete is a value member
    // inside SnowBros (not a pointer). initialize the member before the SnowBros constructor
    // body runs real initialization happens via the parameterized
    // constructor called in SnowBros
    LevelComplete();

    // Real constructor: takes the shared font and window dimensions.
    // windowW/H are used to center all text on screen.
    LevelComplete(Font& f, float w, float h);

    // reset() call every time a level is completed.
    // Resets the typewriter timer and done flag so the screen
    // starts fresh for the new level's story line.
    void reset();

    // update(dt)  call every frame while screen is active.
    // Advances the typewriter timer so more characters appear.
    void update(float dt);

    // handleEvent  pass SFML events here.
    // First ENTER press: if typewriter still going, skip to full line.
    // Second ENTER press (after full line shown): sets done = true.
    void handleEvent(const Event& event);

    // draw -- renders the full level complete screen.
    // Parameters:
    //   window   -- the SFML render window
    //   levelNum -- current level (1-10), used to pick the story line
    //   score    -- current score, drawn below the story line
    void draw(RenderWindow& window, int levelNum, int score);

    // isDone() -- returns true when the player has pressed ENTER
    // after the full story line was shown. SnowBros calls this
    // each frame to know when to call advanceLevel().
    bool isDone() const { return done; }

    ~LevelComplete() {}
};