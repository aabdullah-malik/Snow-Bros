/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#pragma once

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>

using namespace sf;
using namespace std;


// Leaderboard class
// Reads users.txt, sorts by highScore descending, displays top 10
// Each entry shows: Rank | Username | Level Reached | High Score
// File format: username|password|email|gems|currentLevel|highScore


class Leaderboard
{
private:
    Font* font;
    float  windowW;
    float  windowH;

    // 0 = nothing   -1 = back requested
    int    selectedOption;

    // Leaderboard data - max 10 entries
    static const int MAX_ENTRIES = 10;
    string names[MAX_ENTRIES];
    int    scores[MAX_ENTRIES];
    int    levels[MAX_ENTRIES];   // level reached per player
    string dates[MAX_ENTRIES];    // recorded_at (field 7) -- date of last session
    int    count;      // how many entries loaded
    Clock clock;


    // Private helpers

    void   loadData();            // reads and sorts users.txt
    string intToStr(int n);       // int to string without to_string
    void   drawLeaderboard(RenderWindow& window, float t);
    void   drawRow(RenderWindow& window,
        int rank,
        const string& name,
        int level,
        int score,
        const string& date,
        float y,
        Color rowColor);

public:
    Leaderboard(Font& font, float windowW, float windowH);

    void init(float w, float h);
    void handleEvent(const Event& event);
    void draw(RenderWindow& window);

    int  getSelectedOption();
    void reset();

    ~Leaderboard() {}
};