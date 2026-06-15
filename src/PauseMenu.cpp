/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#include "PauseMenu.h"
#include <cmath>

   // HELPER - calculate button positions
   // Called in constructor and init so positions are always ready

void PauseMenu::calculateButtonPositions()
{
    float bx = panelX + (panelW - btnW) / 2.f;
    float btnStartY = panelY + 125.f;  // pushed down to clear title + username + accent line
    float btnSpacing = 62.f;

    for (int i = 0; i < BTN_COUNT; i++)
    {
        btnX[i] = bx;
        btnY[i] = btnStartY + i * btnSpacing;
    }
}

// CONSTRUCTOR

PauseMenu::PauseMenu(Font& f, float w, float h, User* p)
    : font(&f), windowW(w), windowH(h), player(p)
{
    selectedOption = 0;
    selectedBtn = 0;
    saveMessage = "";
    saveMessageTimer = 0.f;
    saveMessageDuration = 1.5f;

    // Panel size - fixed, centered on screen
    panelW = 460.f;   // wider so hint text does not get clipped at edges
    panelH = 600.f;   // taller so title, username, buttons and hints all fit
    panelX = (windowW - panelW) / 2.f;
    panelY = (windowH - panelH) / 2.f;

    // Button size
    btnW = 280.f;
    btnH = 52.f;

    // Calculate positions immediately so handleEvent works before first draw
    calculateButtonPositions();
}

// INIT - call when gameplay is paused

void PauseMenu::init(float w, float h, User* p)
{
    windowW = w;
    windowH = h;
    player = p;

    // Recalculate panel center in case window size changed
    panelX = (windowW - panelW) / 2.f;
    panelY = (windowH - panelH) / 2.f;

    selectedOption = 0;
    selectedBtn = 0;
    saveMessage = "";
    saveMessageTimer = 0.f;

    // Recalculate button positions for new window size
    calculateButtonPositions();
}

// RESET

void PauseMenu::reset()
{
    selectedOption = 0;
    selectedBtn = 0;
    saveMessage = "";
    saveMessageTimer = 0.f;
}

// RESULT GETTER

int PauseMenu::getSelectedOption()
{
    return selectedOption;
}

void PauseMenu::clearOption()
{
    selectedOption = 0;
}

// SHOW SAVE MESSAGE - called by SnowBros after saveProgress()

void PauseMenu::showSaveMessage()
{
    saveMessage = "Game Saved!";
    saveMessageTimer = saveMessageDuration;
}

// UPDATE - countdown save message timer

void PauseMenu::update(float dt)
{
    if (saveMessageTimer > 0.f)
    {
        saveMessageTimer -= dt;
        if (saveMessageTimer <= 0.f)
        {
            saveMessageTimer = 0.f;
            saveMessage = "";
        }
    }
}

// EVENT HANDLING - keyboard only
// Up/Down arrows to navigate, Enter to select, ESC to resume

void PauseMenu::handleEvent(const Event& event)
{
    if (const auto* key = event.getIf<Event::KeyPressed>())
    {
        // ESC = Resume immediately
        if (key->code == Keyboard::Key::Escape)
        {
            selectedOption = 1;
            return;
        }

        // Move selection UP - wrap from 0 back to last button
        if (key->code == Keyboard::Key::Up)
        {
            selectedBtn = (selectedBtn - 1 + BTN_COUNT) % BTN_COUNT;
            return;
        }

        // Move selection DOWN - wrap from last button back to 0
        if (key->code == Keyboard::Key::Down)
        {
            selectedBtn = (selectedBtn + 1) % BTN_COUNT;
            return;
        }

        // ENTER = confirm the currently highlighted button
        if (key->code == Keyboard::Key::Enter)
        {
            // selectedBtn 0=Resume 1=Shop 2=Save 3=Logout 4=MainMenu
            // selectedOption 1=Resume 2=Shop 3=Save 4=Logout 5=MainMenu
            selectedOption = selectedBtn + 1;
            return;
        }
    }
}

// DRAW OVERLAY - semi-transparent dark sheet over frozen gameplay

void PauseMenu::drawOverlay(RenderWindow& window)
{
    RectangleShape overlay;
    overlay.setSize({ windowW, windowH });
    overlay.setFillColor(Color(0, 0, 0, 165));
    overlay.setPosition({ 0, 0 });
    window.draw(overlay);
}

// DRAW ONE BUTTON Highlighted based on selectedBtn (keyboard navigation)

void PauseMenu::drawButton(RenderWindow& window,
    int index,
    const string& label,
    float t)
{
    float x = btnX[index];
    float y = btnY[index];

    // This button is selected if index matches selectedBtn
    bool selected = (index == selectedBtn);

    // Button background
    RectangleShape btn(Vector2f(btnW, btnH));
    btn.setPosition({ x, y });

    if (selected)
    {
        // Pulsing cyan glow on selected button
        float pulse = sin(t * 5.f) * 0.5f + 0.5f;
        int   blue = 180 + (int)(pulse * 75);
        btn.setFillColor(Color(0, 60, blue, 200));
        btn.setOutlineColor(Color(0, blue, 255));
        btn.setOutlineThickness(2);
    }
    else
    {
        btn.setFillColor(Color(10, 20, 55, 220));
        btn.setOutlineColor(Color(0, 120, 180, 160));
        btn.setOutlineThickness(1);
    }
    window.draw(btn);

    // Button label text
    Text btnText(*font, label, 22);
    FloatRect tb = btnText.getLocalBounds();

    // Centered inside button
    float textX = x + (btnW - tb.size.x) / 2.f;
    float textY = y + (btnH - tb.size.y) / 2.f - 4.f;

    if (selected)
        btnText.setFillColor(Color(0, 220, 255));
    else
        btnText.setFillColor(Color(180, 200, 220));

    btnText.setOutlineColor(Color(0, 0, 0));
    btnText.setOutlineThickness(1);
    btnText.setPosition({ textX, textY });
    window.draw(btnText);

    // Left arrow indicator on selected button
    if (selected)
    {
        Text arrow(*font, ">", 18);
        arrow.setFillColor(Color(255, 200, 0));
        arrow.setPosition({ x + 12.f, y + (btnH - 18.f) / 2.f - 2.f });
        window.draw(arrow);
    }
}

// DRAW PANEL - the centered dialog box

void PauseMenu::drawPanel(RenderWindow& window, float t)
{
    // --- Panel background ---
    RectangleShape panel(Vector2f(panelW, panelH));
    panel.setPosition({ panelX, panelY });
    panel.setFillColor(Color(8, 15, 45, 240));
    panel.setOutlineColor(Color(0, 180, 255));
    panel.setOutlineThickness(3);
    window.draw(panel);

    // --- Decorative top accent line inside panel ---
    RectangleShape accent;
    accent.setSize({ panelW - 40.f, 2.f });
    accent.setFillColor(Color(0, 180, 255, 120));
    accent.setPosition({ panelX + 20.f, panelY + 105.f }); // moved down from 85 -- sits below username
    window.draw(accent);

    // --- "PAUSED" title ---
    Text title(*font, "PAUSED", 54);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 255, 255));
    title.setOutlineColor(Color(255, 140, 0));
    title.setOutlineThickness(4);
    title.setPosition({ panelX + (panelW - tb.size.x) / 2.f,
                        panelY + 18.f });
    window.draw(title);

    // --- Player name under title ---
    if (player != nullptr && !player->username.empty())
    {
        Text nameText(*font, player->username, 18);
        FloatRect nb = nameText.getLocalBounds();
        nameText.setFillColor(Color(0, 180, 255));
        nameText.setPosition({ panelX + (panelW - nb.size.x) / 2.f,
                               panelY + 80.f });  // moved down from 62 -- clears PAUSED title (size 54 ends ~panelY+72)
        window.draw(nameText);
    }

    // --- Draw 5 buttons ---
    const char* labels[BTN_COUNT] =
    {
        "RESUME",
        "SHOP",
        "SAVE GAME",
        "LOGOUT",
        "MAIN MENU"
    };

    for (int i = 0; i < BTN_COUNT; i++)
        drawButton(window, i, labels[i], t);

    // --- Navigation hint at bottom ---
    if (!saveMessage.empty())
    {
        // Pulsing green text when saved
        float pulse = sin(t * 6.f) * 0.5f + 0.5f;
        int   green = 180 + (int)(pulse * 75);

        Text msg(*font, saveMessage, 20);
        FloatRect mb = msg.getLocalBounds();
        msg.setFillColor(Color(0, green, 80));
        msg.setOutlineColor(Color(0, 0, 0));
        msg.setOutlineThickness(1);
        msg.setPosition({ panelX + (panelW - mb.size.x) / 2.f,
                          panelY + panelH - 48.f });
        window.draw(msg);
    }
    else
    {
        // Navigation hint
        Text hint(*font, "UP/DOWN = navigate   ENTER = select", 13);
        FloatRect hb = hint.getLocalBounds();
        hint.setFillColor(Color(60, 60, 80));
        hint.setPosition({ panelX + (panelW - hb.size.x) / 2.f,
                           panelY + panelH - 46.f });
        window.draw(hint);

        Text hint2(*font, "ESC = Resume", 13);
        FloatRect hb2 = hint2.getLocalBounds();
        hint2.setFillColor(Color(60, 60, 80));
        hint2.setPosition({ panelX + (panelW - hb2.size.x) / 2.f,
                            panelY + panelH - 26.f });
        window.draw(hint2);
    }
}

// MAIN DRAW - called by SnowBros every frame while paused

void PauseMenu::draw(RenderWindow& window)
{
    if (player == nullptr) return;

    float t = clock.getElapsedTime().asSeconds();

    // 1. Dark overlay over frozen gameplay
    drawOverlay(window);

    // 2. Centered dialog panel + buttons on top
    drawPanel(window, t);
}