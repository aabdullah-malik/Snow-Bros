/* Muhammad Abdullah Malik  25I-3006
   Muhammad Habib Ashfaq    25I-3025  SE-D */

#include "LevelComplete.h"

const char* LevelComplete::storyLines[10] =
{
    // Level 1
    "Nick and Tom set out on their journey, leaving the safety of their home...",
    // Level 2
    "The brothers push deeper into enemy territory. The path grows dangerous...",
    // Level 3
    "Strange new creatures block the way. Nick readies another snowball...",
    // Level 4
    "The enemies grow faster. Tom spots the castle looming in the distance...",
    // Level 5
    "Mogera falls! The first princess is freed, but the battle is far from over...",
    // Level 6
    "Stronger foes appear, battle-hardened and relentless. The brothers press on...",
    // Level 7
    "The enemies move with terrifying speed. Every throw must count now...",
    // Level 8
    "Wave after wave of monsters. Nick and Tom refuse to give in...",
    // Level 9
    "The final castle is in sight. One last push and the princesses will be free...",
    // Level 10
    "Gamakichi falls! The princesses are freed at last. Peace returns to the land!"
};

// DEFAULT CONSTRUCTOR with dummy value to meet requirment in before snowbros constructir runs
// This sets font to nullptr and dimensions to 0
// as safe dummy values. The real init happens when 
// SnowBros assigns: levelComplete = LevelComplete(font, windowW, windowH);

LevelComplete::LevelComplete()
    : font(nullptr), windowW(0.f), windowH(0.f),
    typewriterTimer(0.f), done(false)
{ }

// CONSTRUCTOR
// Stores the font pointer and window dimensions.
// Calls reset() so the object starts in a clean state.

LevelComplete::LevelComplete(Font& f, float w, float h)
    : font(&f), windowW(w), windowH(h)
{
    // Start in a clean state -- same as calling reset()
    typewriterTimer = 0.f;
    done = false;
}

// RESET Called by SnowBros every time a level is completed.
// Resets the typewriter and done flag so the screen starts fresh.

void LevelComplete::reset()
{
    typewriterTimer = 0.f;   // restart typewriter from first character
    done = false;            // player has not pressed ENTER yet
    blinkClock.restart();    // restart blink clock for the hint text
}

// UPDATE Called every frame while screen is active.
// Simply advances the typewriter timer  the draw function uses this
// timer to calculate how many characters to reveal

void LevelComplete::update(float dt)
{
    // dt is the time since the last frame in seconds (e.g. 0.016 at 60fps).
    // Adding dt each frame makes typewriterTimer count up in real time.
    typewriterTimer += dt;
}

// HANDLE EVENT Listens for ENTER or SPACE key presses.
// Two-press logic:
//   Press 1 -- if typewriter still going: skip to full line immediatelyby setting typewriterTimer very high (9999 seconds worth)
//   Press 1 -- if full line already showing: set done = true
//   Press 2 -- (after line was already full): set done = true
//
// This is a standard "press to skip, press again to continue" pattern.

void LevelComplete::handleEvent(const Event& event)
{
    if (const auto* key = event.getIf<Event::KeyPressed>())
    {
        bool enterPressed = (key->code == Keyboard::Key::Enter ||
            key->code == Keyboard::Key::Space);
        if (!enterPressed) return;

        // Check how many characters are currently shown.
        // using valid levelNum to get the story line length.
        // We use storyLines[0] as a fallback -- length check is approximate.
        // The draw() function does the same calculation with the real levelNum.
        // Instead of duplicating level logic here, we use a simple rule:
        // if typewriterTimer is already past 9998, the full line is showing.
        if (typewriterTimer >= 9998.f)
        {
            // Full line is already shown -- this ENTER press advances the level
            done = true;
        }
        else
        {
            // Typewriter still in progress -- skip to full line
            // 9999.f / 30 chars per sec = 333 seconds worth of characters
            // which is always more than any story line length
            typewriterTimer = 9999.f;
        }
    }
}

// DRAW Renders the full level complete screen each frame.
// Parameters:
//   window   -- SFML render window to draw onto
//   levelNum -- current level number (1 to 10)
//   score    -- current player score
// Layout (top to bottom):
//   windowH * 0.18  -- "LEVEL CLEAR!" title
//   windowH * 0.42  -- story line (typewriter)
//   windowH * 0.57  -- score
//   windowH * 0.72  -- blinking PRESS ENTER hint (only when line is full)

void LevelComplete::draw(RenderWindow& window, int levelNum, int score)
{
    // --- Background Same dark green lack as the original drawLevelComplete()
    window.clear(Color(0, 20, 10));

    // --- "LEVEL CLEAR!" title bright green fill with dark green outline
    Text title(*font, "LEVEL CLEAR!", 90);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(0, 255, 120));
    title.setOutlineColor(Color(0, 80, 40));
    title.setOutlineThickness(5);
    // Center horizontally, place at 18% down the screen
    title.setPosition({ (windowW - tb.size.x) / 2.f, windowH * 0.18f });
    window.draw(title);

	// --- Typewriting story line Pick the story line for this level.
    // levelNum is 1-10, array index is 0-9, so subtract 1.
    // Guard against out-of-range values just in case.
    int idx = levelNum - 1;
    if (idx < 0)  idx = 0;
    if (idx > 9)  idx = 9;
    const char* storyLine = storyLines[idx];

    // Calculate length of this story line manually (no STL strlen)
    // We count characters until we hit the null terminator '\0'
    int lineLen = 0;
    while (storyLine[lineLen] != '\0') lineLen++;

    // Calculate how many characters to reveal based on the timer.
    // CHARS_PER_SEC = 30, so after 1 second: 30 chars, after 2s: 60 chars, etc.
    int charsToShow = (int)(typewriterTimer * CHARS_PER_SEC);

    // Never show more characters than the line actually has
    if (charsToShow > lineLen) charsToShow = lineLen;

	// Build the display string character by character appending ONE CHAR AT A TIME until we reach charsToShow. 
    string displayStr = "";
    for (int i = 0; i < charsToShow; i++)
        displayStr += storyLine[i];

    // Draw the typewriter text
    // Font size 26, white fill, thin black outline for readability
    //  Word wraping measure each word, and when adding it would exceed maxLineWidth

    float maxLineWidth = windowW - 160.f;  // 80px padding on each side

    string wrapped = "";
    string currentLine = "";
    string word = "";

    // Walk through displayStr char by char, collect words, then wrap
    for (int i = 0; i <= (int)displayStr.size(); i++)
    {
        // A word ends at a space or at the end of the string
        char c = (i < (int)displayStr.size()) ? displayStr[i] : ' ';
        if (c == ' ' || i == (int)displayStr.size())
        {
            if (word.empty()) { currentLine += ' '; word = ""; continue; }

            // Test if adding this word exceeds maxLineWidth
            string testLine = currentLine + word + " ";
            Text testText(*font, testLine, 26);
            float testWidth = testText.getLocalBounds().size.x;

            if (testWidth > maxLineWidth && !currentLine.empty())
            {
                // Current line is full -- push it and start a new one
                wrapped += currentLine + "\n";
                currentLine = word + " ";
            }
            else
            {
                currentLine = testLine;
            }
            word = "";
        }
        else
        {
            word += c;  // build up the current word char by char
        }
    }
    wrapped += currentLine;  // append whatever is left on the last line

    // Drawing wrapped story text left align within a centered block
    // line individually, which looks cleaner for multi-line story text.
    Text storyText(*font, wrapped, 26);
    storyText.setFillColor(Color(255, 255, 255));
    storyText.setOutlineColor(Color(0, 0, 0));
    storyText.setOutlineThickness(1);
    // Center the whole text block horizontally
    FloatRect sb = storyText.getLocalBounds();
    storyText.setPosition({ (windowW - sb.size.x) / 2.f, windowH * 0.40f });
    window.draw(storyText);

    // --- Score display ---
    // Convert score integer to string manually (no to_string() allowed)
    int s = score;
    string scoreStr = (s == 0) ? "0" : "";
    while (s > 0) { scoreStr = char('0' + s % 10) + scoreStr; s /= 10; }

    Text scoreText(*font, "Score: " + scoreStr, 40);
    FloatRect scb = scoreText.getLocalBounds();
    scoreText.setFillColor(Color(255, 220, 0));   // golden yellow
    scoreText.setPosition({ (windowW - scb.size.x) / 2.f, windowH * 0.57f });
    window.draw(scoreText);

    // --- Blinking "PRESS ENTER" hint ---
    // Only shown after the full story line has finished typing.
    // Blinks every 0.5 seconds using the blinkClock.
    // (int)(blinkClock * 2) % 2 alternates between 0 and 1 every 0.5s.
    if (charsToShow >= lineLen)
    {
        float blinkT = blinkClock.getElapsedTime().asSeconds();
        if ((int)(blinkT * 2) % 2 == 0)
        {
            Text hint(*font, "PRESS ENTER to continue", 28);
            FloatRect hb = hint.getLocalBounds();
            hint.setFillColor(Color(255, 255, 0));
            hint.setPosition({ (windowW - hb.size.x) / 2.f, windowH * 0.72f });
            window.draw(hint);
        }
    }

    window.display();
}