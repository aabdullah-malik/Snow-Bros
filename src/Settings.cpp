/* Muhammad Abdullah Malik  25I-3006
   Muhammad Habib Ashfaq    25I-3025  SE-D */

#include "Settings.h"

   // CONSTRUCTOR
   // Default volumes: music 70, sfx 80 -- reasonable starting values.

Settings::Settings(Font& f, float w, float h)
    : font(&f), windowW(w), windowH(h),
    musicVolume(70.f), sfxVolume(80.f),
    selectedIndex(0), selectedOption(0)
{
}

// RESET Called every time the settings screen is opened.
// Resets selection to first row, clears any pending option.
// Does NOT reset volumes  they persist across opens.

void Settings::reset()
{
    selectedIndex = 0;
    selectedOption = 0;
    blinkClock.restart();
}

// HANDLE EVENT
// 0=music, 1=sfx, 2=back
// LEFT/RIGHT: adjust the currently selected volume by 5 per press
//   Clamped to [0, 100]  never goes below 0 or above 100
// ENTER/ESC: if on Back row (index 2) or ESC anywhere, set option = -1
//   SnowBros reads -1 as "return to main menu"

void Settings::handleEvent(const Event& event)
{
    if (const auto* key = event.getIf<Event::KeyPressed>())
    {
        // Navigate rows
        if (key->code == Keyboard::Key::Up)
            selectedIndex = (selectedIndex - 1 + 3) % 3;

        if (key->code == Keyboard::Key::Down)
            selectedIndex = (selectedIndex + 1) % 3;

        // Adjust volume on music or sfx row
        // Volume step = 5.0 per keypress (20 steps across 0-100)
        if (key->code == Keyboard::Key::Left)
        {
            if (selectedIndex == 0) { musicVolume -= 5.f; if (musicVolume < 0.f)   musicVolume = 0.f; }
            if (selectedIndex == 1) { sfxVolume -= 5.f; if (sfxVolume < 0.f)   sfxVolume = 0.f; }
        }
        if (key->code == Keyboard::Key::Right)
        {
            if (selectedIndex == 0) { musicVolume += 5.f; if (musicVolume > 100.f) musicVolume = 100.f; }
            if (selectedIndex == 1) { sfxVolume += 5.f; if (sfxVolume > 100.f) sfxVolume = 100.f; }
        }

        // Confirm Back or press ESC to return to main menu
        if (key->code == Keyboard::Key::Enter && selectedIndex == 2)
            selectedOption = -1;
        if (key->code == Keyboard::Key::Escape)
            selectedOption = -1;
    }
}

// DRAW VOLUME ROW  (private helper)
// Draws one complete volume control row:
// the bar uses two RectangleShapes:
//   1. Background bar empty" part
//   2. Fill bar the "filled" part
//   fillRatio = volume / 100.f  (0.0 to 1.0)

void Settings::drawVolumeRow(RenderWindow& window,
    const string& label,
    float volume,
    float rowY,
    Color barColor,
    bool  isSelected)
{
    // Dim color for unselected rows
    Color labelColor = isSelected ? barColor : Color(120, 120, 120);
    Color darkBar = Color(barColor.r / 5, barColor.g / 5, barColor.b / 5);

    // Arrow indicator on left of selected row
    if (isSelected)
    {
        Text arrow(*font, ">", 22);
        arrow.setFillColor(barColor);
        arrow.setPosition({ windowW / 2.f - 240.f, rowY });
        window.draw(arrow);
    }

    // Row label (e.g. "MUSIC VOLUME")
    Text lbl(*font, label, 22);
    lbl.setFillColor(labelColor);
    lbl.setOutlineColor(Color::Black);
    lbl.setOutlineThickness(1.f);
    lbl.setPosition({ windowW / 2.f - 210.f, rowY });
    window.draw(lbl);

    // Bar constants
    const float barMaxW = 300.f;
    const float barH = 14.f;
    float barX = windowW / 2.f - 210.f;
    float barY = rowY + 34.f;

    // Left arrow hint "<" -- shows you can decrease
    Text leftArrow(*font, "<", 18);
    leftArrow.setFillColor(isSelected ? Color(200, 200, 200) : Color(60, 60, 60));
    leftArrow.setPosition({ barX - 22.f, barY - 2.f });
    window.draw(leftArrow);

    // Background bar (full width, dark color -- the "empty" portion)
    RectangleShape bgBar({ barMaxW, barH });
    bgBar.setFillColor(darkBar);
    bgBar.setOutlineColor(isSelected ? Color(80, 80, 80) : Color(40, 40, 40));
    bgBar.setOutlineThickness(1.f);
    bgBar.setPosition({ barX, barY });
    window.draw(bgBar);

    
    // fillRatio = volume / 100.f maps 0-100 volume to 0.0-1.0 ratio
    // barMaxW * fillRatio gives pixel width for this volume level
    float fillRatio = volume / 100.f;
    if (fillRatio < 0.f) fillRatio = 0.f;
    if (fillRatio > 1.f) fillRatio = 1.f;
    RectangleShape fillBar({ barMaxW * fillRatio, barH });
    fillBar.setFillColor(isSelected ? barColor : Color(barColor.r / 2, barColor.g / 2, barColor.b / 2));
    fillBar.setPosition({ barX, barY });
    window.draw(fillBar);

    // Right arrow hint ">" -- shows you can increase
    Text rightArrow(*font, ">", 18);
    rightArrow.setFillColor(isSelected ? Color(200, 200, 200) : Color(60, 60, 60));
    rightArrow.setPosition({ barX + barMaxW + 6.f, barY - 2.f });
    window.draw(rightArrow);

    // Percentage label -- convert int to string manually (no to_string())
    int pct = (int)volume;
    string pctStr = (pct == 0) ? "0%" : "";
    while (pct > 0) { pctStr = char('0' + pct % 10) + pctStr; pct /= 10; }
    if (pctStr != "0%") pctStr += "%";
    Text pctText(*font, pctStr, 20);
    pctText.setFillColor(labelColor);
    pctText.setPosition({ barX + barMaxW + 28.f, rowY + 1.f });
    window.draw(pctText);

    // LEFT/RIGHT hint below bar (only on selected row)
    if (isSelected)
    {
        Text hint(*font, "LEFT / RIGHT to adjust", 14);
        hint.setFillColor(Color(140, 140, 140));
        FloatRect hb = hint.getLocalBounds();
        hint.setPosition({ barX + (barMaxW - hb.size.x) / 2.f, barY + barH + 4.f });
        window.draw(hint);
    }
}

// DRAW
// Renders the full settings screen:
//   - Dark background
//   - "SETTINGS" title
//   - Music Volume row  (cyan bar)
//   - SFX Volume row    (orange bar)
//   - Back button

void Settings::draw(RenderWindow& window)
{
    // Dark background
    window.clear(Color(8, 8, 20));

    // Title
    Text title(*font, "SETTINGS", 72);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 215, 0));        // gold
    title.setOutlineColor(Color(100, 80, 0));
    title.setOutlineThickness(4.f);
    title.setPosition({ (windowW - tb.size.x) / 2.f, windowH * 0.10f });
    window.draw(title);

    // Accent line below title
    RectangleShape accent({ windowW * 0.5f, 2.f });
    accent.setFillColor(Color(255, 215, 0, 120));
    accent.setPosition({ windowW * 0.25f, windowH * 0.10f + 80.f });
    window.draw(accent);

    // Row Y positions
    float row0Y = windowH * 0.32f;   // Music Volume
    float row1Y = windowH * 0.50f;   // SFX Volume
    float row2Y = windowH * 0.70f;   // Back

    // Music Volume row (cyan)
    drawVolumeRow(window, "MUSIC VOLUME", musicVolume,
        row0Y, Color(0, 200, 255), selectedIndex == 0);

    // SFX Volume row (orange)
    drawVolumeRow(window, "SFX VOLUME", sfxVolume,
        row1Y, Color(255, 140, 0), selectedIndex == 1);

    // Back button
    bool backSelected = (selectedIndex == 2);
    Color backColor = backSelected ? Color(255, 80, 80) : Color(120, 120, 120);
    Text backText(*font, "BACK", 28);
    FloatRect bb = backText.getLocalBounds();
    backText.setFillColor(backColor);
    backText.setOutlineColor(Color::Black);
    backText.setOutlineThickness(2.f);
    backText.setPosition({ (windowW - bb.size.x) / 2.f, row2Y });
    window.draw(backText);

    // Blink arrow on Back when selected
    if (backSelected)
    {
        float blinkT = blinkClock.getElapsedTime().asSeconds();
        if ((int)(blinkT * 2) % 2 == 0)
        {
            Text arrow(*font, ">", 28);
            arrow.setFillColor(Color(255, 80, 80));
            arrow.setPosition({ (windowW - bb.size.x) / 2.f - 36.f, row2Y });
            window.draw(arrow);
        }
    }

    // Bottom hint
    Text hint(*font, "UP/DOWN = navigate    ENTER/ESC = back", 16);
    FloatRect hb = hint.getLocalBounds();
    hint.setFillColor(Color(80, 80, 80));
    hint.setPosition({ (windowW - hb.size.x) / 2.f, windowH - 30.f });
    window.draw(hint);

    window.display();
}