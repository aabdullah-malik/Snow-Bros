/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#pragma once

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include "User.h"

using namespace sf;
using namespace std;

class Auth
{
private:
    // 0 = sub-menu  1 = login form  2 = register form  -1 = back requested
    int    currentScreen;
    int    selectedIndex;
    int    activeField;

    string usernameInput;
    string passwordInput;
    string emailInput;

    string message;
    bool   messageIsError;

    bool   loginSuccess;
    User   loggedInUser;

    Font* font;
    float  windowW;
    float  windowH;
    Clock  clock;

    bool checkLogin(string username, string password);
    bool usernameExists(string username);
    void saveUser(string username, string password, string email);
    bool hasAtSign(string email);
    void clearInputs();

    void drawSubMenu(RenderWindow& window, float t);
    void drawLoginForm(RenderWindow& window, float t);
    void drawRegisterForm(RenderWindow& window, float t);
    void drawField(RenderWindow& window, float x, float y, float w, float h,
        string label, string value,
        bool isActive, bool hideText);

public:
    Auth(Font& font, float windowW, float windowH);
    void init(float w, float h);
    void handleEvent(const Event& event);
    void draw(RenderWindow& window);
    bool isLoginSuccessful();
    User getLoggedInUser();
    bool isBackRequested();
    void reset();
};