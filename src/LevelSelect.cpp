/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#include "LevelSelect.h"
#include <cmath>
#include <iostream>
using namespace std;
using namespace sf;

// CONSTRUCTOR

LevelSelect::LevelSelect(Font& f, float w, float h, User* p)
    : font(&f), windowW(w), windowH(h), player(p)
{
    selectedOption = 0;
    twoPlayerMode = false;
    texturesLoaded = false;

    // Card size - fits 5 per row with gaps on a 1920-wide screen so it works on any resolution
    cardW = windowW * 0.155f;
    cardH = windowH * 0.38f;

    // All sprite pointers start as nullptr (safe before init)
    for (int i = 0; i < CARD_COUNT; i++)
    {
        bgSprites[i] = nullptr;
        cardX[i] = 0;
        cardY[i] = 0;
    }

    mogenSprite = nullptr;
    gamaSprite = nullptr;
}

// INIT - loads textures, called from SnowBros after mode is chosen

void LevelSelect::init(float w, float h, User* p, bool isTwoPlayer)
{
    windowW = w;
    windowH = h;
    player = p;
    twoPlayerMode = isTwoPlayer;
    selectedOption = 0;

    // Recalculate card size in case window changed
    cardW = windowW * 0.155f;
    cardH = windowH * 0.38f;

    // Clean up any old sprites before reloading
    for (int i = 0; i < CARD_COUNT; i++)
    {
        if (bgSprites[i] != nullptr)
        {
            delete bgSprites[i];
            bgSprites[i] = nullptr;
        }
    }
    if (mogenSprite != nullptr) { delete mogenSprite; mogenSprite = nullptr; }
    if (gamaSprite != nullptr) { delete gamaSprite;  gamaSprite = nullptr; }

    texturesLoaded = true;

    // Load each level background - same files used by Levels.cpp
    const char* bgFiles[CARD_COUNT] =
    {
        "SnowBrosAssets/Backgrounds/level1.png",
        "SnowBrosAssets/Backgrounds/level2.png",
        "SnowBrosAssets/Backgrounds/level3.png",
        "SnowBrosAssets/Backgrounds/level4.png",
        "SnowBrosAssets/Backgrounds/level5.png",
        "SnowBrosAssets/Backgrounds/level6.png",
        "SnowBrosAssets/Backgrounds/level7.png",
        "SnowBrosAssets/Backgrounds/level8.png",
        "SnowBrosAssets/Backgrounds/level9.png",
        "SnowBrosAssets/Backgrounds/level10.png"
    };

    for (int i = 0; i < CARD_COUNT; i++)
    {
        if (bgTextures[i].loadFromFile(bgFiles[i]))
        {
            bgSprites[i] = new Sprite(bgTextures[i]);
        }
        else
        {
            cout << "WARNING: Could not load " << bgFiles[i] << endl;
            texturesLoaded = false;
        }
    }
}

// RESET

void LevelSelect::reset()
{
    selectedOption = 0;
}

// RESULT GETTER

int LevelSelect::getSelectedOption()
{
    return selectedOption;
}

// When selectedOption is 1-10 this returns which level was chosen
int LevelSelect::getSelectedLevel()
{
    if (selectedOption >= 1 && selectedOption <= 10)
        return selectedOption;
    return 1;
}

// HELPER - check if mouse point is inside a rectangle

bool LevelSelect::isMouseOver(float mx, float my,
    float rx, float ry,
    float rw, float rh)
{
    return (mx >= rx && mx <= rx + rw &&
        my >= ry && my <= ry + rh);
}

// HELPER - int to string  (same as Shop.cpp)

string LevelSelect::intToStr(int n)
{
    if (n == 0) return "0";
    string result = "";
    while (n > 0)
    {
        result = char('0' + n % 10) + result;
        n /= 10;
    }
    return result;
}

// HELPER - draw a padlock icon using only shapes
//   cx, cy = center of the card where lock is drawn

void LevelSelect::drawLock(RenderWindow& window, float cx, float cy)
{
    // --- Shackle (the U-shaped arch at the top) ---
    // Drawn as a thick ring segment using two circles
    float shackleR = 22.f;
    float shackleY = cy - 18.f;

    // Outer circle (darker)
    CircleShape outer(shackleR);
    outer.setFillColor(Color(80, 80, 80));
    outer.setPosition({ cx - shackleR, shackleY - shackleR });
    window.draw(outer);

    // Inner circle (cut out center to make ring)
    CircleShape inner(shackleR * 0.55f);
    inner.setFillColor(Color(30, 30, 40));
    inner.setPosition({ cx - shackleR * 0.55f, shackleY - shackleR * 0.55f });
    window.draw(inner);

    // Block out the bottom half of the ring so it looks like a U
    RectangleShape blockBottom;
    blockBottom.setSize({ shackleR * 2.2f, shackleR });
    blockBottom.setFillColor(Color(30, 30, 40));
    blockBottom.setPosition({ cx - shackleR * 1.1f, shackleY });
    window.draw(blockBottom);

    // --- Lock body (rectangle below shackle) ---
    float bodyW = 48.f;
    float bodyH = 36.f;
    float bodyX = cx - bodyW / 2;
    float bodyY = cy;

    RectangleShape body;
    body.setSize({ bodyW, bodyH });
    body.setFillColor(Color(90, 90, 90));
    body.setOutlineColor(Color(130, 130, 130));
    body.setOutlineThickness(2);
    body.setPosition({ bodyX, bodyY });
    window.draw(body);

    // Keyhole dot
    CircleShape keyhole(5.f);
    keyhole.setFillColor(Color(30, 30, 40));
    keyhole.setPosition({ cx - 5.f, bodyY + bodyH * 0.3f });
    window.draw(keyhole);
}

// DRAW ONE CARD

void LevelSelect::drawCard(RenderWindow& window,
    int index,
    float mouseX, float mouseY,
    float t)
{
    float x = cardX[index];
    float y = cardY[index];

    int  levelNum = index + 1;         // levels are 1-indexed

    // Safety check - player must be valid
    if (player == nullptr) return;

    bool isUnlocked = (levelNum <= player->currentLevel);
    bool isBossLevel = (levelNum == 5 || levelNum == 10);
    bool isHovered = isMouseOver(mouseX, mouseY, x, y, cardW, cardH);

    // CARD BACKGROUND
    
    RectangleShape card(Vector2f(cardW, cardH));
    card.setPosition({ x, y });

    if (!isUnlocked)
    {
        // Locked card - dark and grey
        card.setFillColor(Color(25, 25, 35));
        card.setOutlineColor(Color(70, 70, 80));
        card.setOutlineThickness(2);
    }
    else if (isBossLevel)
    {
        // Boss card - red/dark border
        card.setFillColor(Color(40, 10, 10));
        float pulse = sin(t * 3.f) * 0.5f + 0.5f;
        int   r = 180 + (int)(pulse * 75);
        card.setOutlineColor(isHovered ? Color(r, 30, 30) : Color(160, 30, 30));
        card.setOutlineThickness(isHovered ? 4 : 3);
    }
    else
    {
        // Normal unlocked card - blue/cyan border
        card.setFillColor(Color(10, 20, 45));
        float pulse = sin(t * 3.f) * 0.5f + 0.5f;
        int   b = 200 + (int)(pulse * 55);
        card.setOutlineColor(isHovered ? Color(0, b, 255) : Color(0, 150, 200));
        card.setOutlineThickness(isHovered ? 4 : 2);
    }
    window.draw(card);

    // LEVEL BACKGROUND IMAGE (fills the card, only if unlocked)
    if (isUnlocked && bgSprites[index] != nullptr)
    {
        // Scale the background to fill the card
        FloatRect texBounds = bgSprites[index]->getLocalBounds();
        float scaleX = cardW / texBounds.size.x;
        float scaleY = cardH / texBounds.size.y;
        bgSprites[index]->setScale({ scaleX, scaleY });
        bgSprites[index]->setPosition({ x, y });

        // Darken it slightly so text on top is readable
        bgSprites[index]->setColor(Color(180, 180, 180));
        window.draw(*bgSprites[index]);

        // Dark overlay strip at the bottom for text readability
        RectangleShape overlay;
        overlay.setSize({ cardW, cardH * 0.35f });
        overlay.setFillColor(Color(0, 0, 0, 160));
        overlay.setPosition({ x, y + cardH * 0.65f });
        window.draw(overlay);
    }

    // BOSS SPRITE (centered in top half of card)
    if (isUnlocked)
    {
        float bossDisplayH = cardH * 0.42f;

        if (levelNum == 5 && mogenSprite != nullptr)
        {
            FloatRect mb = mogenSprite->getLocalBounds();
            float scaleX = (cardW * 0.75f) / mb.size.x;
            float scaleY = bossDisplayH / mb.size.y;
            // Keep aspect ratio - use smaller scale
            float s = (scaleX < scaleY) ? scaleX : scaleY;
            mogenSprite->setScale({ s, s });
            float bossX = x + (cardW - mb.size.x * s) / 2;
            float bossY = y + cardH * 0.1f;
            mogenSprite->setPosition({ bossX, bossY });
            window.draw(*mogenSprite);
        }

        if (levelNum == 10 && gamaSprite != nullptr)
        {
            FloatRect gb = gamaSprite->getLocalBounds();
            float scaleX = (cardW * 0.85f) / gb.size.x;
            float scaleY = bossDisplayH / gb.size.y;
            float s = (scaleX < scaleY) ? scaleX : scaleY;
            gamaSprite->setScale({ s, s });
            float bossX = x + (cardW - gb.size.x * s) / 2;
            float bossY = y + cardH * 0.08f;
            gamaSprite->setPosition({ bossX, bossY });
            window.draw(*gamaSprite);
        }
    }

    // LOCKED CARD - draw lock icon and "LOCKED" text

    if (!isUnlocked)
    {
        float centerX = x + cardW / 2;
        float centerY = y + cardH * 0.38f;
        drawLock(window, centerX, centerY);

        Text lockText(*font, "LOCKED", 16);
        FloatRect lt = lockText.getLocalBounds();
        lockText.setFillColor(Color(100, 100, 110));
        lockText.setPosition({ x + (cardW - lt.size.x) / 2,
                               y + cardH * 0.65f });
        window.draw(lockText);
    }

    // LEVEL NUMBER (shown on all cards)
    string numStr = intToStr(levelNum);
    int    numSize = (levelNum >= 10) ? 32 : 38;

    Text numText(*font, numStr, numSize);
    FloatRect nb = numText.getLocalBounds();

    if (isUnlocked)
    {
        numText.setFillColor(Color(255, 255, 255));
        numText.setOutlineColor(Color(0, 0, 0));
        numText.setOutlineThickness(3);
    }
    else
    {
        numText.setFillColor(Color(70, 70, 80));
        numText.setOutlineThickness(0);
    }

    // Top-left corner of card with small padding
    numText.setPosition({ x + 8, y + 6 });
    window.draw(numText);

    // BOSS LABEL (level 5 and 10 only, when unlocked)
    if (isUnlocked && isBossLevel)
    {
        Text bossLabel(*font, "BOSS", 14);
        FloatRect bl = bossLabel.getLocalBounds();
        bossLabel.setFillColor(Color(255, 80, 80));
        bossLabel.setOutlineColor(Color(80, 0, 0));
        bossLabel.setOutlineThickness(2);
        bossLabel.setPosition({ x + (cardW - bl.size.x) / 2,
                                 y + cardH * 0.72f });
        window.draw(bossLabel);
    }

    // LEVEL NAME LABEL (bottom of card, unlocked only)
    if (isUnlocked)
    {
        const char* levelNames[CARD_COUNT] =
        {
            "Snowy Village",
            "Ice Caves",
            "Green Forest",
            "Frozen Lake",
            "Dark Lair",
            "Volcanic Waste",
            "Haunted Castle",
            "Alien Planet",
            "Storm Clouds",
            "Final Arena"
        };

        Text nameText(*font, levelNames[index], 13);
        FloatRect nt = nameText.getLocalBounds();
        nameText.setFillColor(Color(200, 220, 255));
        nameText.setPosition({ x + (cardW - nt.size.x) / 2,
                                y + cardH * 0.84f });
        window.draw(nameText);
    }

    // HOVER GLOW OVERLAY (thin bright rect on top of card border)
    if (isUnlocked && isHovered)
    {
        RectangleShape glow(Vector2f(cardW, cardH));
        glow.setPosition({ x, y });
        glow.setFillColor(Color(255, 255, 255, 18));
        glow.setOutlineThickness(0);
        window.draw(glow);
    }
}

// DRAW FULL SCREEN

void LevelSelect::drawLevelSelect(RenderWindow& window, float t)
{
    if (player == nullptr) return;

    // Mouse position for hover
    Vector2i mousePos = Mouse::getPosition(window);
    float mouseX = (float)mousePos.x;
    float mouseY = (float)mousePos.y;

    // --- Snowflakes ---
    for (int i = 0; i < 40; i++)
    {
        float snowX = fmod(i * 173.7f + t * 50, windowW);
        float snowY = fmod(i * 97.3f + t * 50, windowH);
        float size = 1.5f + (i % 3);
        CircleShape flake(size);
        flake.setFillColor(Color(255, 255, 255, 120 + (i % 5) * 15));
        flake.setPosition({ snowX, snowY });
        window.draw(flake);
    }

    // --- Top and bottom lines ---
    RectangleShape topLine(Vector2f(windowW, 3));
    topLine.setFillColor(Color(0, 200, 255));
    topLine.setPosition({ 0, 0 });
    window.draw(topLine);

    RectangleShape botLine(Vector2f(windowW, 3));
    botLine.setFillColor(Color(0, 200, 255));
    botLine.setPosition({ 0, windowH - 3 });
    window.draw(botLine);

    // --- Title ---
    Text title(*font, "SELECT LEVEL", 70);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 255, 255));
    title.setOutlineColor(Color(255, 140, 0));
    title.setOutlineThickness(4);
    title.setPosition({ (windowW - tb.size.x) / 2, 22 });
    window.draw(title);

    // --- Mode indicator under title ---
    string modeStr = twoPlayerMode ? "MULTIPLAYER MODE" : "SINGLE PLAYER MODE";
    Text modeText(*font, modeStr, 22);
    FloatRect mb = modeText.getLocalBounds();
    modeText.setFillColor(Color(0, 200, 255));
    modeText.setPosition({ (windowW - mb.size.x) / 2, 105 });
    window.draw(modeText);

    // CALCULATE CARD POSITIONS
    // Row 1: levels 1-5   Row 2: levels 6-10

    float cardGap = windowW * 0.018f;
    float row1TotalW = 5 * cardW + 4 * cardGap;
    float row1StartX = (windowW - row1TotalW) / 2;
    float row1Y = windowH * 0.17f;
    float row2Y = row1Y + cardH + windowH * 0.04f;

    for (int i = 0; i < CARD_COUNT; i++)
    {
        int col = i % 5;   // column 0-4 in each row
        cardX[i] = row1StartX + col * (cardW + cardGap);
        cardY[i] = (i < 5) ? row1Y : row2Y;
    }

    // --- Draw all 10 cards ---
    for (int i = 0; i < CARD_COUNT; i++)
        drawCard(window, i, mouseX, mouseY, t);

    // --- Player progress hint ---
    string progressStr = "Unlocked up to Level " + intToStr(player->currentLevel);
    Text progText(*font, progressStr, 18);
    FloatRect pb = progText.getLocalBounds();
    progText.setFillColor(Color(180, 180, 180));
    progText.setPosition({ (windowW - pb.size.x) / 2, windowH - 46 });
    window.draw(progText);

    // --- ESC hint ---
    Text hint(*font, "ESC = Back to Mode Select", 18);
    FloatRect hb = hint.getLocalBounds();
    hint.setFillColor(Color(80, 80, 80));
    hint.setPosition({ (windowW - hb.size.x) / 2, windowH - 24 });
    window.draw(hint);
}

// EVENT HANDLING

void LevelSelect::handleEvent(const Event& event)
{
    // ESC goes back to mode select
    if (const auto* key = event.getIf<Event::KeyPressed>())
    {
        if (key->code == Keyboard::Key::Escape)
            selectedOption = -1;
    }

    // Mouse click
    if (const auto* click = event.getIf<Event::MouseButtonPressed>())
    {
        if (click->button == Mouse::Button::Left)
        {
            float mx = (float)click->position.x;
            float my = (float)click->position.y;

            for (int i = 0; i < CARD_COUNT; i++)
            {
                if (isMouseOver(mx, my, cardX[i], cardY[i], cardW, cardH))
                {
                    int levelNum = i + 1;

                    // Safety: player pointer must be valid
                    if (player == nullptr) return;

                    if (levelNum <= player->currentLevel)
                    {
                        // Unlocked - go to this level
                        selectedOption = levelNum;
                    }
                    // Locked levels - do nothing (click silently ignored)
                    return;
                }
            }
        }
    }
}

// MAIN DRAW - called every frame by SnowBros

void LevelSelect::draw(RenderWindow& window)
{
    window.clear(Color::Black);
    float t = clock.getElapsedTime().asSeconds();
    drawLevelSelect(window, t);
    window.display();
}

// DESTRUCTOR - clean up all heap-allocated sprites

LevelSelect::~LevelSelect()
{
    for (int i = 0; i < CARD_COUNT; i++)
    {
        if (bgSprites[i] != nullptr)
        {
            delete bgSprites[i];
            bgSprites[i] = nullptr;
        }
    }

    if (mogenSprite != nullptr) { delete mogenSprite; mogenSprite = nullptr; }
    if (gamaSprite != nullptr) { delete gamaSprite;  gamaSprite = nullptr; }
}