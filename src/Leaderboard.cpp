/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#include "Leaderboard.h"
#include <cmath>
using namespace sf;
using namespace std;

// CONSTRUCTOR

Leaderboard::Leaderboard(Font& f, float w, float h)
    : font(&f), windowW(w), windowH(h)
{
    selectedOption = 0;
    count = 0;

    // Initialize all arrays to safe empty values
    for (int i = 0; i < MAX_ENTRIES; i++)
    {
        names[i] = "";
        scores[i] = 0;
        levels[i] = 0;
        dates[i] = "N/A";  // default until real date is loaded from file
    }
}


// INIT - call every time the leaderboard screen is opened
// so it always shows fresh data from users.txt


void Leaderboard::init(float w, float h)
{
    windowW = w;
    windowH = h;
    selectedOption = 0;
    count = 0;

    // Load fresh data from file every time screen opens
    loadData();
}

// RESET

void Leaderboard::reset()
{
    selectedOption = 0;
}

// RESULT GETTER

int Leaderboard::getSelectedOption()
{
    return selectedOption;
}

// HELPER - int to string without to_string

string Leaderboard::intToStr(int n)
{
    if (n == 0) return "0";
    string result = "";
    bool negative = (n < 0);
    if (negative) n = -n;
    while (n > 0)
    {
        result = char('0' + n % 10) + result;
        n /= 10;
    }
    if (negative) result = "-" + result;
    return result;
}

// LOAD DATA - reads users.txt and sorts entries by highScore

void Leaderboard::loadData()
{
    count = 0;
    for (int i = 0; i < MAX_ENTRIES; i++)
    {
        names[i] = ""; scores[i] = 0; levels[i] = 0; dates[i] = "N/A";
    }

    ifstream file("users.txt");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line) && count < MAX_ENTRIES)
    {
        if (line.empty()) continue;

        // Split line by '|' into 8 parts (fields 0-7)
        // 0=username 1=password 2=email 3=gems 4=level 5=highScore
        // 6=created_at 7=recorded_at
        string parts[8];
        int    pi = 0;
        string cur = "";

        for (int i = 0; i < (int)line.size(); i++)
        {
            if (line[i] == '|')
            {
                if (pi < 7) { parts[pi++] = cur; cur = ""; }
            }
            else cur += line[i];
        }
        parts[pi] = cur;

        // Need at least fields 0-5 to be valid 
        if (pi < 5) continue;

        // Parse highScore from parts[5]
        // Use long long to detect overflow; clamp to safe int range
        long long hsLL = 0;
        for (int j = 0; j < (int)parts[5].size(); j++)
            if (parts[5][j] >= '0' && parts[5][j] <= '9')
                hsLL = hsLL * 10 + (parts[5][j] - '0');
        int hs = (hsLL > 999999999LL) ? 999999999 : (int)hsLL;

        // Parse currentLevel from parts[4]
        int lv = 1;
        for (int j = 0; j < (int)parts[4].size(); j++)
            if (parts[4][j] >= '0' && parts[4][j] <= '9')
                lv = lv * 10 + (parts[4][j] - '0');
        
        lv = 0;
        for (int j = 0; j < (int)parts[4].size(); j++)
            if (parts[4][j] >= '0' && parts[4][j] <= '9')
                lv = lv * 10 + (parts[4][j] - '0');

        names[count] = parts[0];   // username
        scores[count] = hs;          // highScore
        levels[count] = lv;          // currentLevel (level reached)
        // parts[7] = recorded_at (date of last session).
        // If the account has no field 7, default to N/A.
        dates[count] = (pi >= 7 && !parts[7].empty()) ? parts[7] : "N/A";
        count++;
    }
    file.close();

    // Bubble sort descending by highScore
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - 1 - i; j++)
        {
            if (scores[j] < scores[j + 1])
            {
                // Swap scores
                int tmp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = tmp;

                // Swap names
                string ts = names[j];
                names[j] = names[j + 1];
                names[j + 1] = ts;

                // Swap levels
                int tl = levels[j];
                levels[j] = levels[j + 1];
                levels[j + 1] = tl;

                // Swap dates -- must move with their owner row
                string td = dates[j];
                dates[j] = dates[j + 1];
                dates[j + 1] = td;
            }
        }
    }
}

// EVENT HANDLING - ESC goes back

void Leaderboard::handleEvent(const Event& event)
{
    if (const auto* key = event.getIf<Event::KeyPressed>())
    {
        if (key->code == Keyboard::Key::Escape ||
            key->code == Keyboard::Key::Enter)
        {
            selectedOption = -1;  // signal SnowBros to go back
        }
    }
}

// DRAW ONE ROW  Shows: rank | name | level reached | score

void Leaderboard::drawRow(RenderWindow& window,int rank,const string& name,
    int level, int score,const string& date,float y, Color rowColor)
{
    float rowH = 52.f;
    float rowW = windowW * 0.72f;
    float rowX = (windowW - rowW) / 2.f;

    // Row background - subtle dark rectangle
    RectangleShape bg;
    bg.setSize({ rowW, rowH - 4.f });
    bg.setPosition({ rowX, y });
    bg.setFillColor(Color(0, 0, 0, 60));
    bg.setOutlineColor(Color(rowColor.r, rowColor.g, rowColor.b, 60));
    bg.setOutlineThickness(1);
    window.draw(bg);

    // --- Rank badge (left side) ---
    string rankStr = intToStr(rank) + ".";
    Text rankText(*font, rankStr, 26);
    rankText.setFillColor(rowColor);
    rankText.setOutlineColor(Color(0, 0, 0));
    rankText.setOutlineThickness(2);
    rankText.setPosition({ rowX + 18.f, y + 10.f });
    window.draw(rankText);

    // --- Medal emoji for top 3 using colored circle ---
    if (rank <= 3)
    {
        Color medalColor = (rank == 1) ? Color(255, 215, 0) :
            (rank == 2) ? Color(192, 192, 192) :
            Color(205, 127, 50);
        CircleShape medal(10.f);
        medal.setFillColor(medalColor);
        medal.setOutlineColor(Color(0, 0, 0, 100));
        medal.setOutlineThickness(1);
        medal.setPosition({ rowX + 58.f, y + 14.f });
        window.draw(medal);
    }

    // --- Username ---
    Text nameText(*font, name, 24);
    nameText.setFillColor(rowColor);
    nameText.setOutlineColor(Color(0, 0, 0));
    nameText.setOutlineThickness(1);
    nameText.setPosition({ rowX + 88.f, y + 12.f });
    window.draw(nameText);

    // --- Level reached ---
    string lvlStr = "Lvl " + intToStr(level);
    Text lvlText(*font, lvlStr, 22);
    lvlText.setFillColor(Color(rowColor.r, rowColor.g, rowColor.b, 200));
    lvlText.setPosition({ rowX + rowW * 0.48f, y + 14.f });
    window.draw(lvlText);

    // --- Date (recorded_at) ---
    // This is the date the player's high score was recorded (field 7 in users.txt)
    Text dateText(*font, date, 20);
    dateText.setFillColor(Color(rowColor.r, rowColor.g, rowColor.b, 170));
    dateText.setPosition({ rowX + rowW * 0.65f, y + 16.f });
    window.draw(dateText);

    // --- Score (right side) ---
    string scoreStr = intToStr(score);
    Text scoreText(*font, scoreStr, 24);
    FloatRect sb = scoreText.getLocalBounds();
    scoreText.setFillColor(rowColor);
    scoreText.setOutlineColor(Color(0, 0, 0));
    scoreText.setOutlineThickness(1);
    // Right-align the score
    scoreText.setPosition({ rowX + rowW - sb.size.x - 18.f, y + 12.f });
    window.draw(scoreText);
}

// DRAW FULL LEADERBOARD SCREEN

void Leaderboard::drawLeaderboard(RenderWindow& window, float t)
{
    // --- Background ---
    window.clear(Color(0, 10, 30));

    // Snowflake particles - same as all other screens
    for (int i = 0; i < 40; i++)
    {
        float snowX = fmod(i * 173.7f + t * 40, windowW);
        float snowY = fmod(i * 97.3f + t * 30, windowH);
        float size = 1.5f + (i % 3);
        CircleShape flake(size);
        flake.setFillColor(Color(255, 255, 255, 80 + (i % 5) * 15));
        flake.setPosition({ snowX, snowY });
        window.draw(flake);
    }

    // Top and bottom accent lines
    RectangleShape topLine(Vector2f(windowW, 3));
    topLine.setFillColor(Color(255, 215, 0));
    topLine.setPosition({ 0, 0 });
    window.draw(topLine);

    RectangleShape botLine(Vector2f(windowW, 3));
    botLine.setFillColor(Color(255, 215, 0));
    botLine.setPosition({ 0, windowH - 3 });
    window.draw(botLine);

    // --- LEADERBOARD title ---
    Text title(*font, "LEADERBOARD", 70);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 215, 0));
    title.setOutlineColor(Color(100, 80, 0));
    title.setOutlineThickness(4);
    title.setPosition({ (windowW - tb.size.x) / 2.f, 22.f });
    window.draw(title);

    // --- Column headers ---
    float headerY = 118.f;
    float rowW = windowW * 0.72f;
    float rowX = (windowW - rowW) / 2.f;

    // Divider line under title
    RectangleShape divider;
    divider.setSize({ rowW, 2.f });
    divider.setFillColor(Color(255, 215, 0, 120));
    divider.setPosition({ rowX, headerY });
    window.draw(divider);

    Text hRank(*font, "#", 20); hRank.setFillColor(Color(150, 150, 150));  hRank.setPosition({ rowX + 18.f,           headerY + 4.f }); window.draw(hRank);
    Text hName(*font, "PLAYER", 20); hName.setFillColor(Color(150, 150, 150));  hName.setPosition({ rowX + 88.f,           headerY + 4.f }); window.draw(hName);
    Text hLvl(*font, "LEVEL", 20); hLvl.setFillColor(Color(150, 150, 150));   hLvl.setPosition({ rowX + rowW * 0.48f,    headerY + 4.f }); window.draw(hLvl);
    Text hDate(*font, "DATE", 20);  hDate.setFillColor(Color(150, 150, 150));  hDate.setPosition({ rowX + rowW * 0.65f,   headerY + 4.f }); window.draw(hDate);
    Text hScore(*font, "HIGH SCORE", 20); hScore.setFillColor(Color(150, 150, 150));
    FloatRect hsb = hScore.getLocalBounds();
    hScore.setPosition({ rowX + rowW - hsb.size.x - 18.f, headerY + 4.f });
    window.draw(hScore);

    // Second divider
    RectangleShape divider2;
    divider2.setSize({ rowW, 1.f });
    divider2.setFillColor(Color(255, 215, 0, 60));
    divider2.setPosition({ rowX, headerY + 30.f });
    window.draw(divider2);

    // --- Entries ---
    if (count == 0)
    {
        // No data message
        Text noData(*font, "No scores yet — play the game!", 30);
        FloatRect nb = noData.getLocalBounds();
        noData.setFillColor(Color(160, 160, 160));
        noData.setPosition({ (windowW - nb.size.x) / 2.f, windowH / 2.f });
        window.draw(noData);
    }
    else
    {
        float startY = headerY + 38.f;
        float rowSpacing = 54.f;

        for (int i = 0; i < count && i < MAX_ENTRIES; i++)
        {
            // Color: gold for 1st, silver for 2nd, bronze for 3rd, white for rest
            Color rowColor = (i == 0) ? Color(255, 215, 0) :
                (i == 1) ? Color(192, 192, 192) :
                (i == 2) ? Color(205, 127, 50) :
                Color(180, 180, 200);

            // Highlight current row with subtle pulse for top 3
            if (i < 3)
            {
                float pulse = sin(t * 2.f + i * 1.5f) * 0.15f + 0.85f;
                rowColor.r = (uint8_t)(rowColor.r * pulse);
                rowColor.g = (uint8_t)(rowColor.g * pulse);
                rowColor.b = (uint8_t)(rowColor.b * pulse);
            }

            drawRow(window,
                i + 1,
                names[i],
                levels[i],
                scores[i],
                dates[i],
                startY + i * rowSpacing,
                rowColor);
        }
    }

    // --- ESC hint at bottom ---
    Text hint(*font, "ESC / ENTER = Back", 20);
    FloatRect hb = hint.getLocalBounds();
    hint.setFillColor(Color(80, 80, 80));
    hint.setPosition({ (windowW - hb.size.x) / 2.f, windowH - 38.f });
    window.draw(hint);
}

// MAIN DRAW - called every frame by SnowBros while on screen 9

void Leaderboard::draw(RenderWindow& window)
{
    float t = clock.getElapsedTime().asSeconds();
    drawLeaderboard(window, t);
    window.display();
}