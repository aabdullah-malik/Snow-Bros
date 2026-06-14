/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#include "Shop.h"
#include <iostream>
#include <cmath>

   // CONSTRUCTOR

Shop::Shop(Font& f, float w, float h, User* p)
    : font(&f), windowW(w), windowH(h), player(p)
{
    selectedOption = 0;
    selectedItem = -1;
    messageIsError = false;
    textureLoaded = false;
    itemSprite = nullptr;
    previousScreen = 2;   // default  assume main menu until told otherwise

    for (int i = 0; i < 5; i++)
    {
        cardX[i] = 0;
        cardY[i] = 0;
    }

    if (itemsTexture.loadFromFile("SnowBrosAssets/Images/Items.png"))
    {
        textureLoaded = true;
        itemSprite = new Sprite(itemsTexture);
        itemSprite->setTextureRect(iconRects[0]);
    }
    else
    {
        cout << "WARNING: Could not load Items.png" << endl;
    }
}

// INIT

void Shop::init(float w, float h, User* p, int prevScreen)
{
    windowW = w;
    windowH = h;
    player = p;
    previousScreen = prevScreen;
}

// RESET

void Shop::reset()
{
    selectedOption = 0;
    selectedItem = -1;
    message = "";
}

// Clean up sprite pointer when Shop is destroyed
Shop::~Shop()
{
    if (itemSprite != nullptr)
    {
        delete itemSprite;
        itemSprite = nullptr;
    }
}

// RESULT GETTER

int Shop::getSelectedOption()
{
    return selectedOption;
}

// HELPER - int to string without to_string

string Shop::intToStr(int n)
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

// HELPER - check if mouse is inside a rectangle

bool Shop::isMouseOver(float mx, float my,
    float rx, float ry,
    float rw, float rh)
{
    return (mx >= rx && mx <= rx + rw &&
        my >= ry && my <= ry + rh);
}

// FILE HELPER - update gems in users.txt

void Shop::updateGemsInFile()
{
    // Read all lines into array
    string lines[100];
    int    lineCount = 0;

    ifstream fileIn("users.txt");
    if (!fileIn.is_open())
    {
        cout << "ERROR: Could not open users.txt" << endl;
        return;
    }

    string line;
    while (getline(fileIn, line) && lineCount < 100)
    {
        if (!line.empty())
        {
            lines[lineCount] = line;
            lineCount++;
        }
    }
    fileIn.close();

    // Find player line and update gems
    for (int i = 0; i < lineCount; i++)
    {
        string parts[8];
        int    partIndex = 0;
        string current = "";

        for (int j = 0; j < (int)lines[i].size(); j++)
        {
            if (lines[i][j] == '|')
            {
                if (partIndex < 7) { parts[partIndex] = current; partIndex++; }
                current = "";
            }
            else
            {
                current += lines[i][j];
            }
        }
        parts[partIndex] = current;

        if (parts[0] == player->username)
        {
            parts[3] = intToStr(player->gems);

            // Rebuild all 8 fields — preserve created_at (parts[6]) and recorded_at (parts[7])
            lines[i] = parts[0] + "|" +
                parts[1] + "|" +
                parts[2] + "|" +
                parts[3] + "|" +
                parts[4] + "|" +
                parts[5] + "|" +
                parts[6] + "|" +
                parts[7];
        }
    }

    // Rewrite entire file
    ofstream fileOut("users.txt");
    if (!fileOut.is_open())
    {
        cout << "ERROR: Could not write users.txt" << endl;
        return;
    }

    for (int i = 0; i < lineCount; i++)
        fileOut << lines[i] << "\n";

    fileOut.close();
}

// DRAW ONE CARD

void Shop::drawCard(RenderWindow& window,
    float x, float y,
    int index,
    float mouseX, float mouseY,
    float t)
{
    // --- Check if mouse is hovering over the whole card ---
    bool cardHover = isMouseOver(mouseX, mouseY, x, y, cardW, cardH);

    // --- Card background ---
    RectangleShape card(Vector2f(cardW, cardH));
    card.setPosition({ x, y });
    card.setFillColor(Color(45, 20, 8));        // dark warm brown

    // Border glows gold on hover
    if (cardHover)
        card.setOutlineColor(Color(255, 230, 80));   // bright gold hover
    else
        card.setOutlineColor(Color(180, 120, 0));    // normal gold

    card.setOutlineThickness(3);
    window.draw(card);

    // --- Item icon (from sprite sheet) ---
    float iconSize = 80;    // display size on screen
    float iconX = x + (cardW - iconSize) / 2;  // centered in card
    float iconY = y + 25;

    if (textureLoaded && itemSprite != nullptr)
    {
        // Cut out the correct icon from the sprite sheet
        itemSprite->setTextureRect(iconRects[index]);

       FloatRect iconBounds = itemSprite->getLocalBounds();
        float scaleX = iconSize / iconBounds.size.x;
        float scaleY = iconSize / iconBounds.size.y;
        itemSprite->setScale({ scaleX, scaleY });
        itemSprite->setPosition({ iconX, iconY });
        window.draw(*itemSprite);
    }
    else
    {
        // No texture - draw a placeholder colored circle
        CircleShape placeholder(iconSize / 2);
        placeholder.setFillColor(Color(100, 60, 20));
        placeholder.setOutlineColor(Color(180, 120, 0));
        placeholder.setOutlineThickness(2);
        placeholder.setPosition({ iconX, iconY });
        window.draw(placeholder);
    }

    // --- Item name (2 lines) ---
    const char* line1[5] = { "EXTRA",    "SPEED",    "SNOWBALL", "DISTANCE", "BALLOON" };
    const char* line2[5] = { "LIFE",     "BOOST",    "POWER",    "BOOST",    "MODE" };

    // Duration shown under the name
    // Permanent = stays forever,  Level = whole level
    const char* duration[5] = { "PERMANENT", "30 SECONDS", "FULL LEVEL",
                                 "FULL LEVEL", "30 SECONDS" };

    Text nameLine1(*font, line1[index], 16);
    FloatRect nb1 = nameLine1.getLocalBounds();
    nameLine1.setFillColor(Color(255, 255, 255));
    nameLine1.setPosition({ x + (cardW - nb1.size.x) / 2, y + 110 });
    window.draw(nameLine1);

    Text nameLine2(*font, line2[index], 16);
    FloatRect nb2 = nameLine2.getLocalBounds();
    nameLine2.setFillColor(Color(255, 255, 255));
    nameLine2.setPosition({ x + (cardW - nb2.size.x) / 2, y + 132 });
    window.draw(nameLine2);

    // Duration text - smaller, cyan color
    Text durText(*font, duration[index], 12);
    FloatRect db = durText.getLocalBounds();
    durText.setFillColor(Color(0, 220, 255));
    durText.setPosition({ x + (cardW - db.size.x) / 2, y + 156 });
    window.draw(durText);

    // --- Gem cost ---
    Text costText(*font, intToStr(itemCosts[index]) + " gems", 16);
    FloatRect cb = costText.getLocalBounds();
    costText.setFillColor(Color(255, 220, 0));
    costText.setPosition({ x + (cardW - cb.size.x) / 2, y + 178 });
    window.draw(costText);

    // --- BUY button - shows LOCKED if opened from main menu ---
    float buyX = x + (cardW - buyBtnW) / 2;
    float buyY = y + cardH - buyBtnH - 20;

    bool buyHover = isMouseOver(mouseX, mouseY, buyX, buyY, buyBtnW, buyBtnH);

    RectangleShape buyBtn(Vector2f(buyBtnW, buyBtnH));
    buyBtn.setPosition({ buyX, buyY });

    if (previousScreen == 2)
    {
        // Locked - grey button from main menu
        buyBtn.setFillColor(Color(80, 80, 80));
        buyBtn.setOutlineColor(Color(120, 120, 120));
    }
    else
    {
        // Normal - orange buy button during gameplay
        buyBtn.setFillColor(buyHover ? Color(255, 120, 40) : Color(180, 60, 10));
        buyBtn.setOutlineColor(Color(255, 180, 0));
    }

    buyBtn.setOutlineThickness(2);
    window.draw(buyBtn);

    // Button text - BUY or LOCKED
    string btnLabel = (previousScreen == 2) ? "LOCKED" : "BUY";
    Text buyText(*font, btnLabel, 18);
    FloatRect bb = buyText.getLocalBounds();
    buyText.setFillColor(previousScreen == 2 ? Color(160, 160, 160)
        : Color(255, 255, 255));
    buyText.setPosition({ buyX + (buyBtnW - bb.size.x) / 2,
                          buyY + (buyBtnH - bb.size.y) / 2 - 4 });
    window.draw(buyText);
}

// DRAW FULL SHOP

void Shop::drawShop(RenderWindow& window, float t)
{
    if (player == nullptr) return;

    // Get current mouse position for hover detection
    Vector2i mousePos = Mouse::getPosition(window);
    float    mouseX = (float)mousePos.x;
    float    mouseY = (float)mousePos.y;

    //  Background - deep purple instead of black 

    // --- Gold sparkles instead of snowflakes ---
    for (int i = 0; i < 40; i++)
    {
        float sparkX = fmod(i * 173.7f + t * 30, windowW);
        float sparkY = fmod(i * 97.3f + t * 20, windowH);
        float size = 1.0f + (i % 3);

        CircleShape spark(size);
        spark.setFillColor(Color(255, 215, 0, 100 + (i % 5) * 20));
        spark.setPosition({ sparkX, sparkY });
        window.draw(spark);
    }

    // --- Top line - gold ---
    RectangleShape topLine(Vector2f(windowW, 3));
    topLine.setFillColor(Color(255, 180, 0));
    topLine.setPosition({ 0, 0 });
    window.draw(topLine);

    // --- Bottom line - gold ---
    RectangleShape botLine(Vector2f(windowW, 3));
    botLine.setFillColor(Color(255, 180, 0));
    botLine.setPosition({ 0, windowH - 3 });
    window.draw(botLine);

    // --- SHOP title - gold with dark red outline ---
    Text title(*font, "SHOP", 90);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 210, 0));
    title.setOutlineColor(Color(150, 50, 0));
    title.setOutlineThickness(5);
    title.setPosition({ (windowW - tb.size.x) / 2, 20 });
    window.draw(title);

    // --- Your Gems - centered under title ---
    Text gemText(*font, "Your Gems: " + intToStr(player->gems), 26);
    FloatRect gb = gemText.getLocalBounds();
    gemText.setFillColor(Color(255, 220, 0));
    gemText.setPosition({ (windowW - gb.size.x) / 2, 125 });
    window.draw(gemText);

    // --- Gold divider under gems ---
    RectangleShape div(Vector2f(windowW - 300, 2));
    div.setFillColor(Color(180, 120, 0));
    div.setPosition({ 150, 165 });
    window.draw(div);

    // CALCULATE CARD POSITIONS

    // Row 1: 3 cards centered
    float row1TotalW = 3 * cardW + 2 * cardGap;
    float row1StartX = (windowW - row1TotalW) / 2;
    float row1Y = windowH * 0.28f;

    // Row 2: 2 cards centered
    float row2TotalW = 2 * cardW + 1 * cardGap;
    float row2StartX = (windowW - row2TotalW) / 2;
    float row2Y = row1Y + cardH + 40;

    // Store positions for click detection in handleEvent()
    cardX[0] = row1StartX;                   cardY[0] = row1Y;
    cardX[1] = row1StartX + cardW + cardGap; cardY[1] = row1Y;
    cardX[2] = row1StartX + 2 * (cardW + cardGap); cardY[2] = row1Y;
    cardX[3] = row2StartX;                   cardY[3] = row2Y;
    cardX[4] = row2StartX + cardW + cardGap; cardY[4] = row2Y;

    // --- Draw all 5 cards ---
    for (int i = 0; i < 5; i++)
    {
        drawCard(window, cardX[i], cardY[i], i, mouseX, mouseY, t);
    }

    // --- Message (success or error) ---
    if (!message.empty())
    {
        Text msg(*font, message, 22);
        FloatRect mb = msg.getLocalBounds();
        msg.setFillColor(messageIsError ? Color(255, 80, 80)
            : Color(80, 255, 120));
        msg.setPosition({ (windowW - mb.size.x) / 2, row2Y + cardH + 20 });
        window.draw(msg);
    }

    // --- Disclaimer - only shown when opened from main menu ---
    if (previousScreen == 2)
    {
        Text disc1(*font, "* Items are for PREVIEW only from main menu", 16);
        FloatRect d1 = disc1.getLocalBounds();
        disc1.setFillColor(Color(200, 200, 80));
        disc1.setPosition({ (windowW - d1.size.x) / 2, windowH - 90 });
        window.draw(disc1);

        Text disc2(*font, "* Effects activate during gameplay", 16);
        FloatRect d2 = disc2.getLocalBounds();
        disc2.setFillColor(Color(200, 200, 80));
        disc2.setPosition({ (windowW - d2.size.x) / 2, windowH - 68 });
        window.draw(disc2);
    }

    // --- ESC hint ---
    Text hint(*font, previousScreen == 6 ? "ESC = Back to Game" : "ESC = Back to Main Menu", 20);
    hint.setFillColor(Color(150, 100, 30));
    FloatRect hb = hint.getLocalBounds();
    hint.setPosition({ (windowW - hb.size.x) / 2, windowH - 46 });
    window.draw(hint);
}

// EVENT HANDLING

void Shop::handleEvent(const Event& event)
{
    // Escape goes back to main menu
    if (const auto* key = event.getIf<Event::KeyPressed>())
    {
        if (key->code == Keyboard::Key::Escape)
            selectedOption = -1;
    }

    // Mouse click detection
    if (const auto* click = event.getIf<Event::MouseButtonPressed>())
    {
        if (click->button == Mouse::Button::Left)
        {
            float mx = (float)click->position.x;
            float my = (float)click->position.y;

            // Check each card's BUY button
            for (int i = 0; i < 5; i++)
            {
                // BUY button position inside card
                float buyX = cardX[i] + (cardW - buyBtnW) / 2;
                float buyY = cardY[i] + cardH - buyBtnH - 20;

                if (isMouseOver(mx, my, buyX, buyY, buyBtnW, buyBtnH))
                {
                    // If opened from main menu - purchases are disabled
                    if (previousScreen == 2)
                    {
                        message = "Available during gameplay only!";
                        messageIsError = true;
                        return;
                    }

                    // BUY button clicked for item i
                    int cost = itemCosts[i];

                    if (player->gems >= cost)
                    {
                        // Enough gems - deduct and save
                        player->gems -= cost;
                        updateGemsInFile();
                        selectedItem = i;    // store which item was bought
                        message = "Purchased! Gems left: " + intToStr(player->gems);
                        messageIsError = false;
                    }
                    else
                    {
                        // Not enough gems
                        int needed = cost - player->gems;
                        message = "Need " + intToStr(needed) + " more gems!";
                        messageIsError = true;
                    }
                }
            }
        }
    }
}

// MAIN DRAW - called every frame by SnowBros

void Shop::draw(RenderWindow& window)
{
    // Deep purple background - different from rest of game
    window.clear(Color(20, 10, 40));

    float t = clock.getElapsedTime().asSeconds();
    drawShop(window, t);

    window.display();
}