/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#pragma once

#include <string>

using namespace std;

// User class - holds ALL data for one logged in player
// This object is filled by Auth after login and passed around the game
// File format in users.txt:
// username|password|email|gems|currentLevel|highScore|created_at|recorded_at

class User
{
public:

    string username;
    string password;
    string email;
    int gems;
    int currentLevel;
    int highScore;

    // Default constructor - creates an empty/invalid user
    // Used before anyone has logged in
    User()
    {
        username = "";
        password = "";
        email = "";
        gems = 0;
        currentLevel = 1;
        highScore = 0;
    }

    // constructor  Used by Auth after reading a line from users.txt
    User(string uname, string pass, string mail, int g, int level, int score)
    {
        username = uname;
        password = pass;
        email = mail;
        gems = g;
        currentLevel = level;
        highScore = score;
    }

    // Returns true if this is a valid logged in user
    bool isValid()
    {
        return username != "";
    }
};