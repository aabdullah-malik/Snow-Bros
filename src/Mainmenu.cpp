/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#include "MainMenu.h"
#include <cmath>

   // CONSTRUCTOR

MainMenu::MainMenu(Font& f, float w, float h, User* p)
    : font(&f), windowW(w), windowH(h), player(p)
{
    selectedIndex = 0;
    selectedOption = 0;
}

// INIT - called from SnowBros after window size and login are known

void MainMenu::init(float w, float h, User* p)
{
    windowW = w;
    windowH = h;
    player = p;
}

// RESET - called when coming back to main menu

void MainMenu::reset()
{
    selectedIndex = 0;
    selectedOption = 0;
}

// RESULT GETTER - SnowBros reads this every frame

int MainMenu::getSelectedOption()
{
    return selectedOption;
}

// EVENT HANDLING

void MainMenu::handleEvent(const Event& event)
{
    if (const auto* key = event.getIf<Event::KeyPressed>())
    {
        // Move selection up - wrap from 0 back to 4
        if (key->code == Keyboard::Key::Up)
        {
            selectedIndex = (selectedIndex - 1 + 5) % 5;
        }

        // Move selection down - wrap from 4 back to 0
        if (key->code == Keyboard::Key::Down)
        {
            selectedIndex = (selectedIndex + 1) % 5;
        }

        if (key->code == Keyboard::Key::Enter)
        {
            if (selectedIndex == 0) selectedOption = 1;  // New Game
            if (selectedIndex == 1) selectedOption = 2;  // Continue
            if (selectedIndex == 2) selectedOption = 3;  // Shop
            if (selectedIndex == 3) selectedOption = 4;  // Settings
            if (selectedIndex == 4) selectedOption = 5;  // Logout
        }

        // Escape = Logout - go back to splash screen
        if (key->code == Keyboard::Key::Escape)
        {
            selectedOption = 5;
        }
    }
}

// DRAW HELPERS

void MainMenu::drawMainMenu(RenderWindow& window, float t)
{
    // Safety check - if player pointer is null don't draw
    if (player == nullptr) return;
    // --- Snowflakes - copied exactly from SnowBros to look consistent ---
    for (int i = 0; i < 40; i++)
    {
        float snowX = fmod(i * 173.7f + t * 50, windowW);
        float snowY = fmod(i * 97.3f + t * 50, windowH);
        float size = 1.5f + rand() % 4;

        CircleShape flake(size);
        flake.setFillColor(Color(255, 255, 255, 120 + (i % 5) * 15));
        flake.setPosition({ snowX, snowY });
        window.draw(flake);
    }

    // --- Top line ---
    RectangleShape topLine;
    topLine.setSize({ windowW, 3 });
    topLine.setFillColor(Color(0, 200, 255));
    topLine.setPosition({ 0, 0 });
    window.draw(topLine);

    // --- Bottom line ---
    RectangleShape botLine;
    botLine.setSize({ windowW, 3 });
    botLine.setFillColor(Color(0, 200, 255));
    botLine.setPosition({ 0, windowH - 3 });
    window.draw(botLine);

    // --- SNOW BROS title - size 90 (smaller than splash screen's 130) ---
    int   titleSize = 90;
    float titleY = windowH / 10;

    Text titleMain(*font, "SNOW BROS", titleSize);
    FloatRect tb = titleMain.getLocalBounds();
    float titleX = (windowW - tb.size.x) / 2;
    titleMain.setFillColor(Color(255, 255, 255));
    titleMain.setOutlineColor(Color(255, 140, 0));
    titleMain.setOutlineThickness(5);
    titleMain.setPosition({ titleX, titleY });
    window.draw(titleMain);

    // --- NICK & TOM - size 45 (smaller than splash screen's 65) ---
    int   nickSize = 45;
    float nickY = titleY + titleSize - 10;

    Text nickText(*font, "NICK", nickSize);
    nickText.setFillColor(Color(0, 210, 0));
    nickText.setOutlineColor(Color(0, 80, 0));
    nickText.setOutlineThickness(3);
    FloatRect nb = nickText.getLocalBounds();

    Text andText(*font, " & ", nickSize);
    andText.setFillColor(Color(255, 255, 255));
    andText.setOutlineColor(Color(100, 100, 100));
    andText.setOutlineThickness(3);
    FloatRect ab = andText.getLocalBounds();

    Text tomText(*font, "TOM", nickSize);
    tomText.setFillColor(Color(220, 30, 30));
    tomText.setOutlineColor(Color(80, 0, 0));
    tomText.setOutlineThickness(3);
    FloatRect tb2 = tomText.getLocalBounds();

    float totalWidth = nb.size.x + ab.size.x + tb2.size.x;
    float nickStartX = (windowW - totalWidth) / 2 - 20;

    nickText.setPosition({ nickStartX, nickY });
    andText.setPosition({ nickStartX + nb.size.x, nickY });
    tomText.setPosition({ nickStartX + nb.size.x + ab.size.x, nickY });

    window.draw(nickText);
    window.draw(andText);
    window.draw(tomText);

    // --- Divider - same style as splash screen ---
    float dividerY = nickY + nickSize + 15;

    RectangleShape divLeft;
    divLeft.setSize({ (windowW / 2) - 180, 2 });
    divLeft.setFillColor(Color(0, 180, 255, 160));
    divLeft.setPosition({ 60, dividerY });
    window.draw(divLeft);

    RectangleShape divRight;
    divRight.setSize({ (windowW / 2) - 180, 2 });
    divRight.setFillColor(Color(0, 180, 255, 160));
    divRight.setPosition({ windowW / 2 + 120, dividerY });
    window.draw(divRight);

    RectangleShape square;
    square.setSize({ 10, 10 });
    square.setFillColor(Color(0, 200, 255));
    square.setPosition({ windowW / 2 - 6, dividerY - 4 });
    window.draw(square);

    // --- Player name at top right ---
    Text nameText(*font, "Player: " + player->username, 22);
    nameText.setFillColor(Color(0, 200, 255));
    FloatRect nm = nameText.getLocalBounds();
    // Position: 30px from right edge, 15px from top
    nameText.setPosition({ windowW - nm.size.x - 30, 15 });
    window.draw(nameText);

    // --- Gem count at top left ---
    // Convert gems int to string manually - no to_string needed
    int g = player->gems;
    string gemStr = "";
    if (g == 0) { gemStr = "0"; }
    else { while (g > 0) { gemStr = char('0' + g % 10) + gemStr; g /= 10; } }
    Text gemText(*font, "Gems: " + gemStr, 22);
    gemText.setFillColor(Color(255, 220, 0));
    gemText.setPosition({ 30, 15 });
    window.draw(gemText);

    // --- 5 Menu options ---
    const char* options[5] =
    {
        "New Game",
        "Continue",
        "Shop",
        "Settings",
        "Logout"
    };

    int   menuFontSize = 36;
    float menuSpacing = 58;
    float menuStartY = dividerY + 90;

    for (int i = 0; i < 5; i++)
    {
        float y = menuStartY + i * menuSpacing;

        Text item(*font, options[i], menuFontSize);
        FloatRect ib = item.getLocalBounds();
        float x = (windowW - ib.size.x) / 2;

        if (i == selectedIndex)
        {
            // Pulsing glow - same formula as your splash screen
            float p = sin(t * 5.0f) * 0.5f + 0.5f;
            int   g = 200 + (int)(p * 55);

            item.setFillColor(Color(0, g, 255));
            item.setOutlineColor(Color(0, 50, 120));
            item.setOutlineThickness(2);

            // Left arrow
            Text leftArrow(*font, ">>", 26);
            leftArrow.setFillColor(Color(255, 200, 0));
            leftArrow.setPosition({ x - 80, y + 5 });
            window.draw(leftArrow);

            // Right arrow
            Text rightArrow(*font, "<<", 26);
            rightArrow.setFillColor(Color(255, 200, 0));
            rightArrow.setPosition({ x + ib.size.x + 45, y + 5 });
            window.draw(rightArrow);

            // Subtle glow rectangle behind selected item
            RectangleShape glow;
            glow.setSize({ ib.size.x + 40, 46 });
            glow.setFillColor(Color(0, 80, 180, 60));
            glow.setOutlineColor(Color(0, 150, 255, 100));
            glow.setOutlineThickness(1);
            glow.setPosition({ x - 20, y - 2 });
            window.draw(glow);
        }
        else
        {
            item.setFillColor(Color(160, 160, 160));
            item.setOutlineColor(Color(50, 50, 50));
            item.setOutlineThickness(2);
        }

        item.setPosition({ x, y });
        window.draw(item);
    }

    // --- Credits at bottom - same as splash screen ---
    RectangleShape creditLine;
    creditLine.setSize({ windowW - 200, 1 });
    creditLine.setFillColor(Color(0, 150, 200, 100));
    creditLine.setPosition({ 100, windowH - 70 });
    window.draw(creditLine);

    Text credit1(*font, "Muhammad Abdullah Malik  |  25I-3006", 20);
    credit1.setFillColor(Color(0, 180, 200));
    FloatRect cb1 = credit1.getLocalBounds();
    credit1.setPosition({ (windowW - cb1.size.x) / 2, windowH - 58 });
    window.draw(credit1);

    Text credit2(*font, "Muhammad Habib Ashfaq  |  25I-3025", 20);
    credit2.setFillColor(Color(0, 180, 200));
    FloatRect cb2 = credit2.getLocalBounds();
    credit2.setPosition({ (windowW - cb2.size.x) / 2, windowH - 32 });
    window.draw(credit2);
}

// MAIN DRAW - SnowBros calls this every frame

void MainMenu::draw(RenderWindow& window)
{
    window.clear(Color::Black);

    float t = clock.getElapsedTime().asSeconds();

    drawMainMenu(window, t);

    window.display();
}