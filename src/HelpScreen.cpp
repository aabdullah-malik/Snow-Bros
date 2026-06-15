#include "HelpScreen.h"
#include <cmath>

// CONSTRUCTOR

HelpScreen::HelpScreen(Font& f, float w, float h)
{
    font = &f;
    windowW = w;
    windowH = h;
    selectedOption = 0;
}

void HelpScreen::reset()
{
    selectedOption = 0;
}

int HelpScreen::getSelectedOption()
{
    return selectedOption;
}

void HelpScreen::handleEvent(const Event& event)
{
    if (const auto* key = event.getIf<Event::KeyPressed>())
    {
        if (key->code == Keyboard::Key::Escape ||
            key->code == Keyboard::Key::Enter)
            selectedOption = -1;
    }
}

void HelpScreen::draw(RenderWindow& window)
{
    window.clear(Color(20, 10, 40));
    float t = clock.getElapsedTime().asSeconds();

    // ── Snowflakes ────────────────────────────────────────────────────
    for (int i = 0; i < 40; i++)
    {
        float snowX = fmod(i * 173.7f + t * 30, windowW);
        float snowY = fmod(i * 97.3f + t * 20, windowH);
        CircleShape flake(1.5f + (i % 3));
        flake.setFillColor(Color(255, 255, 255, 60 + (i % 5) * 10));
        flake.setPosition({ snowX, snowY });
        window.draw(flake);
    }

    // ── Title ─────────────────────────────────────────────────────────
    Text title(*font, "HOW TO PLAY", 60);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 215, 0));
    title.setOutlineColor(Color(100, 80, 0));
    title.setOutlineThickness(4);
    title.setPosition({ (windowW - tb.size.x) / 2.f, 20.f });
    window.draw(title);

    // ── Big box - orange border like shop ─────────────────────────────
    float boxX = 60.f;
    float boxY = 110.f;
    float boxW = windowW - 120.f;
    float boxH = windowH - 170.f;

    RectangleShape box;
    box.setSize({ boxW, boxH });
    box.setPosition({ boxX, boxY });
    box.setFillColor(Color(30, 15, 5, 220));
    box.setOutlineColor(Color(255, 180, 0));
    box.setOutlineThickness(3);
    window.draw(box);

    // ── Layout values ─────────────────────────────────────────────────
    float col1X = boxX + 40.f;
    float col2X = boxX + boxW / 2.f + 20.f;
    float lineY = boxY + 20.f;
    float lineH = 28.f;

    Color headCol = Color(255, 180, 0);    // orange - section headings
    Color textCol = Color(200, 200, 210);  // light grey - normal text
    Color keyCol = Color(0, 220, 255);    // cyan - key names

    // ── CONTROLS HEADINGS ─────────────────────────────────────────────
    Text h1(*font, "PLAYER 1  (NICK)", 20); h1.setFillColor(headCol); h1.setPosition({ col1X, lineY }); window.draw(h1);
    Text h2(*font, "PLAYER 2  (TOM)", 20); h2.setFillColor(headCol); h2.setPosition({ col2X, lineY }); window.draw(h2);
    lineY += lineH + 4.f;

    // Player 1 controls
    const char* keys1[6] = { "A / D", "W", "SPACE", "K", "ESC", "F1" };
    const char* desc1[6] = { "Move Left / Right", "Jump", "Throw Snowball", "Push Snowball", "Pause Game", "Toggle Hitboxes" };

    // Player 2 controls
    const char* keys2[5] = { "LEFT / RIGHT", "UP", "ENTER", "O", "ESC" };
    const char* desc2[5] = { "Move Left / Right", "Jump", "Throw Snowball", "Push Snowball", "Pause Game" };

    for (int i = 0; i < 6; i++)
    {
        // Player 1 row
        Text k1(*font, keys1[i], 17); k1.setFillColor(keyCol);  k1.setPosition({ col1X,         lineY }); window.draw(k1);
        Text d1(*font, "-", 17); d1.setFillColor(textCol); d1.setPosition({ col1X + 160.f,  lineY }); window.draw(d1);
        Text e1(*font, desc1[i], 17); e1.setFillColor(textCol); e1.setPosition({ col1X + 185.f,  lineY }); window.draw(e1);

        // Player 2 row (only 5)
        if (i < 5)
        {
            Text k2(*font, keys2[i], 17); k2.setFillColor(keyCol);  k2.setPosition({ col2X,         lineY }); window.draw(k2);
            Text d2(*font, "-", 17); d2.setFillColor(textCol); d2.setPosition({ col2X + 210.f,  lineY }); window.draw(d2);
            Text e2(*font, desc2[i], 17); e2.setFillColor(textCol); e2.setPosition({ col2X + 235.f,  lineY }); window.draw(e2);
        }

        lineY += lineH;
    }

    // ── DIVIDER 1 ─────────────────────────────────────────────────────
    lineY += 8.f;
    RectangleShape div1({ boxW - 80.f, 1.f });
    div1.setFillColor(Color(255, 180, 0, 120));
    div1.setPosition({ boxX + 40.f, lineY });
    window.draw(div1);
    lineY += 14.f;

    // ── POWER-UPS ─────────────────────────────────────────────────────
    Text ph(*font, "POWER-UPS", 20); ph.setFillColor(headCol); ph.setPosition({ col1X, lineY }); window.draw(ph);
    lineY += lineH + 4.f;

    const char* puNames[4] = { "Speed Boost", "Snowball Power", "Distance Boost", "Balloon Mode" };
    const char* puDesc[4] = {
        "-  Move 50% faster  (15 sec)",
        "-  1 hit enemy coating  (15 sec)",
        "-  Snowball wraps screen  (15 sec)",
        "-  Float + kill enemies  (15 sec)"
    };

    for (int i = 0; i < 4; i++)
    {
        Text pn(*font, puNames[i], 17); pn.setFillColor(keyCol);  pn.setPosition({ col1X,         lineY }); window.draw(pn);
        Text pd(*font, puDesc[i], 17); pd.setFillColor(textCol); pd.setPosition({ col1X + 240.f,  lineY }); window.draw(pd);
        lineY += lineH;
    }

    // ── DIVIDER 2 ─────────────────────────────────────────────────────
    lineY += 8.f;
    RectangleShape div2({ boxW - 80.f, 1.f });
    div2.setFillColor(Color(255, 180, 0, 120));
    div2.setPosition({ boxX + 40.f, lineY });
    window.draw(div2);
    lineY += 14.f;

    // ── GAME MECHANICS ────────────────────────────────────────────────
    Text mh(*font, "GAME MECHANICS", 20); mh.setFillColor(headCol); mh.setPosition({ col1X, lineY }); window.draw(mh);
    lineY += lineH + 4.f;

    Text mech(*font,
        "Throw snowball  >>  Enemy gets coated  >>  Push snowball  >>  Chain kill for bonus score",
        17);
    mech.setFillColor(textCol);
    mech.setPosition({ col1X, lineY });
    window.draw(mech);

    // ── ESC hint ─────────────────────────────────────────────────────
    Text hint(*font, "ESC / ENTER = Back to Menu", 18);
    FloatRect hb = hint.getLocalBounds();
    hint.setFillColor(Color(120, 100, 50));
    hint.setPosition({ (windowW - hb.size.x) / 2.f, windowH - 38.f });
    window.draw(hint);

    window.display();
}