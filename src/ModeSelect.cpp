/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#include "ModeSelect.h"
#include <iostream>
#include <cmath>

   // CONSTRUCTOR

ModeSelect::ModeSelect(Font& f, float w, float h, User* p)
    : font(&f), windowW(w), windowH(h), player(p)
{
    selectedOption = 0;
    texturesLoaded = false;
    nickSprite = nullptr;
    tomSprite = nullptr;
    panelW = 520;
    panelH = 580;
    btnW = 200;
    btnH = 50;
    leftPanelX = 0;
    leftPanelY = 0;
    rightPanelX = 0;
    rightPanelY = 0;
    // Textures loaded in init()  because assets folder not accessible at construction time
}

// DESTRUCTOR

ModeSelect::~ModeSelect()
{
    if (nickSprite != nullptr) { delete nickSprite; nickSprite = nullptr; }
    if (tomSprite != nullptr) { delete tomSprite;  tomSprite = nullptr; }
}

// INIT - loads textures and sets up sprites

void ModeSelect::init(float w, float h, User* p)
{
    windowW = w;
    windowH = h;
    player = p;

    // Clean up old sprites before reloading
    if (nickSprite != nullptr) { delete nickSprite; nickSprite = nullptr; }
    if (tomSprite != nullptr) { delete tomSprite;  tomSprite = nullptr; }
    texturesLoaded = false;

    // Load Nick - Nick.png
    // Idle frame: top-left at (0,0), size 85x61 from Paint measurement
    if (nickTexture.loadFromFile("SnowBrosAssets/Images/Nick.png"))
    {
        nickSprite = new Sprite(nickTexture);
        nickSprite->setTextureRect(IntRect({ 116, 70 }, { 220, 275 }));
    }
    else
        cout << "WARNING: Could not load Nick.png" << endl;

    // Load Tom - Player_Red.png
    // Idle frame: top-left at (0,0), size 64x73 from Paint measurement
    if (tomTexture.loadFromFile("SnowBrosAssets/Images/Player_Red.png"))
    {
        tomSprite = new Sprite(tomTexture);
        tomSprite->setTextureRect(IntRect({ 19, 3 }, { 57, 73 }));
    }
    else
        cout << "WARNING: Could not load Player_Red.png" << endl;

    if (nickSprite != nullptr && tomSprite != nullptr)
        texturesLoaded = true;
}

// RESET

void ModeSelect::reset()
{
    selectedOption = 0;
}

// RESULT GETTER

int ModeSelect::getSelectedOption()
{
    return selectedOption;
}

// HELPER - mouse over check

bool ModeSelect::isMouseOver(float mx, float my,
    float rx, float ry,
    float rw, float rh)
{
    return (mx >= rx && mx <= rx + rw &&
        my >= ry && my <= ry + rh);
}

// EVENT HANDLING

void ModeSelect::handleEvent(const Event& event)
{
    // ESC = back to main menu
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

            // Single player PLAY button position
            float leftBtnX = leftPanelX + (panelW - btnW) / 2;
            float leftBtnY = leftPanelY + panelH - btnH - 30;

            // Multiplayer PLAY button position
            float rightBtnX = rightPanelX + (panelW - btnW) / 2;
            float rightBtnY = rightPanelY + panelH - btnH - 30;

            if (isMouseOver(mx, my, leftBtnX, leftBtnY, btnW, btnH))
            {
                // Single player selected
                // TODO: go to character selection screen (single player mode)
                selectedOption = 1;
            }

            if (isMouseOver(mx, my, rightBtnX, rightBtnY, btnW, btnH))
            {
                // Multiplayer selected
                // TODO: go to character selection screen (multiplayer mode)
                selectedOption = 2;
            }
        }
    }
}

// DRAW LEFT PANEL - Single Player (blue theme)

void ModeSelect::drawLeftPanel(RenderWindow& window,
    float mouseX, float mouseY, float t)
{
    // --- Panel background - dark blue ---
    RectangleShape panel(Vector2f(panelW, panelH));
    panel.setPosition({ leftPanelX, leftPanelY });
    panel.setFillColor(Color(10, 20, 60));
    panel.setOutlineColor(Color(0, 200, 255));
    panel.setOutlineThickness(3);
    window.draw(panel);

    // --- Nick sprite centered in top half of panel ---
    float spriteDisplaySize = 220;   // size on screen

    if (nickSprite != nullptr)
    {
        FloatRect nb = nickSprite->getLocalBounds();
        float scaleX = spriteDisplaySize / nb.size.x;
        float scaleY = spriteDisplaySize / nb.size.y;
        nickSprite->setScale({ scaleX, scaleY });

        // Centered horizontally, positioned in top half
        float spriteX = leftPanelX + (panelW - spriteDisplaySize) / 2;
        float spriteY = leftPanelY + 40;
        nickSprite->setPosition({ spriteX, spriteY });
        window.draw(*nickSprite);
    }
    else
    {
        // Placeholder circle if texture failed to load
        CircleShape placeholder(80);
        placeholder.setFillColor(Color(0, 100, 200));
        placeholder.setOutlineColor(Color(0, 200, 255));
        placeholder.setOutlineThickness(2);
        placeholder.setPosition({ leftPanelX + panelW / 2 - 80, leftPanelY + 40 });
        window.draw(placeholder);
    }

    // --- SINGLE PLAYER text ---
    Text line1(*font, "SINGLE", 30);
    FloatRect l1 = line1.getLocalBounds();
    line1.setFillColor(Color(255, 255, 255));
    line1.setOutlineColor(Color(0, 100, 200));
    line1.setOutlineThickness(2);
    line1.setPosition({ leftPanelX + (panelW - l1.size.x) / 2,
                       leftPanelY + panelH - 185 });
    window.draw(line1);

    Text line2(*font, "PLAYER", 30);
    FloatRect l2 = line2.getLocalBounds();
    line2.setFillColor(Color(255, 255, 255));
    line2.setOutlineColor(Color(0, 100, 200));
    line2.setOutlineThickness(2);
    line2.setPosition({ leftPanelX + (panelW - l2.size.x) / 2,
                       leftPanelY + panelH - 143 });
    window.draw(line2);

    // --- PLAY button ---
    float btnX = leftPanelX + (panelW - btnW) / 2;
    float btnY = leftPanelY + panelH - btnH - 30;

    bool hover = isMouseOver(mouseX, mouseY, btnX, btnY, btnW, btnH);

    RectangleShape btn(Vector2f(btnW, btnH));
    btn.setPosition({ btnX, btnY });
    btn.setFillColor(hover ? Color(0, 230, 255) : Color(0, 150, 200));
    btn.setOutlineColor(Color(255, 255, 255));
    btn.setOutlineThickness(2);
    window.draw(btn);

    Text btnText(*font, "PLAY", 22);
    FloatRect bb = btnText.getLocalBounds();
    btnText.setFillColor(Color(0, 0, 0));
    btnText.setPosition({ btnX + (btnW - bb.size.x) / 2,
                         btnY + (btnH - bb.size.y) / 2 - 4 });
    window.draw(btnText);
}

// DRAW RIGHT PANEL - Multiplayer (pink theme)

void ModeSelect::drawRightPanel(RenderWindow& window,
    float mouseX, float mouseY, float t)
{
    // --- Panel background - dark pink ---
    RectangleShape panel(Vector2f(panelW, panelH));
    panel.setPosition({ rightPanelX, rightPanelY });
    panel.setFillColor(Color(60, 10, 40));
    panel.setOutlineColor(Color(255, 80, 180));
    panel.setOutlineThickness(3);
    window.draw(panel);

    // --- Nick + Tom sprites side by side centered in top half ---
    float spriteDisplaySize = 160;   // each sprite display size
    float totalSpritesW = spriteDisplaySize * 2 + 20;  // 20px gap between
    float startSpriteX = rightPanelX + (panelW - totalSpritesW) / 2;
    float spriteY = rightPanelY + 50;

    if (nickSprite != nullptr)
    {
        FloatRect nb = nickSprite->getLocalBounds();
        float scaleX = spriteDisplaySize / nb.size.x;
        float scaleY = spriteDisplaySize / nb.size.y;
        nickSprite->setScale({ scaleX, scaleY });
        nickSprite->setPosition({ startSpriteX, spriteY });
        window.draw(*nickSprite);
    }

    if (tomSprite != nullptr)
    {
        FloatRect tb = tomSprite->getLocalBounds();
        float scaleX = spriteDisplaySize / tb.size.x;
        float scaleY = spriteDisplaySize / tb.size.y;
        tomSprite->setScale({ scaleX, scaleY });
        // Tom placed right of Nick with 20px gap
        tomSprite->setPosition({ startSpriteX + spriteDisplaySize + 20, spriteY });
        window.draw(*tomSprite);
    }

    if (nickSprite == nullptr && tomSprite == nullptr)
    {
        // Placeholders if textures failed
        CircleShape p1(60);
        p1.setFillColor(Color(0, 100, 200));
        p1.setPosition({ rightPanelX + panelW / 2 - 140, rightPanelY + 50 });
        window.draw(p1);

        CircleShape p2(60);
        p2.setFillColor(Color(200, 50, 0));
        p2.setPosition({ rightPanelX + panelW / 2 + 20, rightPanelY + 50 });
        window.draw(p2);
    }

    // --- MULTI PLAYER text ---
    Text line1(*font, "MULTI", 30);
    FloatRect l1 = line1.getLocalBounds();
    line1.setFillColor(Color(255, 255, 255));
    line1.setOutlineColor(Color(180, 0, 100));
    line1.setOutlineThickness(2);
    line1.setPosition({ rightPanelX + (panelW - l1.size.x) / 2,
                       rightPanelY + panelH - 185 });
    window.draw(line1);

    Text line2(*font, "PLAYER", 30);
    FloatRect l2 = line2.getLocalBounds();
    line2.setFillColor(Color(255, 255, 255));
    line2.setOutlineColor(Color(180, 0, 100));
    line2.setOutlineThickness(2);
    line2.setPosition({ rightPanelX + (panelW - l2.size.x) / 2,
                       rightPanelY + panelH - 143 });
    window.draw(line2);

    // --- PLAY button ---
    float btnX = rightPanelX + (panelW - btnW) / 2;
    float btnY = rightPanelY + panelH - btnH - 30;

    bool hover = isMouseOver(mouseX, mouseY, btnX, btnY, btnW, btnH);

    RectangleShape btn(Vector2f(btnW, btnH));
    btn.setPosition({ btnX, btnY });
    btn.setFillColor(hover ? Color(255, 120, 200) : Color(200, 50, 140));
    btn.setOutlineColor(Color(255, 255, 255));
    btn.setOutlineThickness(2);
    window.draw(btn);

    Text btnText(*font, "PLAY", 22);
    FloatRect bb = btnText.getLocalBounds();
    btnText.setFillColor(Color(255, 255, 255));
    btnText.setPosition({ btnX + (btnW - bb.size.x) / 2,
                         btnY + (btnH - bb.size.y) / 2 - 4 });
    window.draw(btnText);
}

// DRAW FULL SCREEN

void ModeSelect::drawModeSelect(RenderWindow& window, float t)
{
    // Get mouse position every frame for hover
    Vector2i mousePos = Mouse::getPosition(window);
    float    mouseX = (float)mousePos.x;
    float    mouseY = (float)mousePos.y;

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

    // --- SELECT MODE title ---
    Text title(*font, "SELECT MODE", 70);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 255, 255));
    title.setOutlineColor(Color(255, 140, 0));
    title.setOutlineThickness(4);
    title.setPosition({ (windowW - tb.size.x) / 2, 30 });
    window.draw(title);

    // --- Calculate panel positions ---
    float gap = 40;
    float totalWidth = 2 * panelW + gap;
    float startX = (windowW - totalWidth) / 2;
    float panelY = (windowH - panelH) / 2 + 20;

    // Store for click detection in handleEvent()
    leftPanelX = startX;
    leftPanelY = panelY;
    rightPanelX = startX + panelW + gap;
    rightPanelY = panelY;

    // --- Draw both panels ---
    drawLeftPanel(window, mouseX, mouseY, t);
    drawRightPanel(window, mouseX, mouseY, t);

    // --- ESC hint ---
    Text hint(*font, "ESC = Back to Main Menu", 20);
    hint.setFillColor(Color(80, 80, 80));
    FloatRect hb = hint.getLocalBounds();
    hint.setPosition({ (windowW - hb.size.x) / 2, windowH - 50 });
    window.draw(hint);
}

// MAIN DRAW - called every frame by SnowBros

void ModeSelect::draw(RenderWindow& window)
{
    window.clear(Color::Black);
    float t = clock.getElapsedTime().asSeconds();
    drawModeSelect(window, t);
    window.display();
}