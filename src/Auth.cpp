/* Muhammad Abdullah Malik
   25I-3006
   Muhammad Habib Ashfaq
   25I-3025
   SE-D */

#include "Auth.h"
#include <iostream>
#include <ctime>

static string getCurrentDate()
{
    time_t now = time(nullptr);       // get current time as epoch seconds
    tm t{};
    localtime_s(&t, &now);            // MSVC-safe: fills our tm struct

    int year = t.tm_year + 1900;     // tm_year counts from 1900, add it back
    int month = t.tm_mon + 1;        // tm_mon is 0 based (0=Jan), make 1-based
    int day = t.tm_mday;            // tm_mday is already 1-based

    // Build "YYYY-MM-DD" string digit by digit
    
    string d = "";
    d += char('0' + year / 1000);
    d += char('0' + (year / 100) % 10);
    d += char('0' + (year / 10) % 10);
    d += char('0' + year % 10);
    d += '-';
    d += char('0' + month / 10);    // leading zero for months 1-9
    d += char('0' + month % 10);
    d += '-';
    d += char('0' + day / 10);      // leading zero for days 1-9
    d += char('0' + day % 10);
    return d;                        // e.g. "2026-05-02"
}

// CONSTRUCTOR

Auth::Auth(Font& f, float w, float h)
    : font(&f), windowW(w), windowH(h)  // &f = store the ADDRESS of f as a pointer
{
    currentScreen = 0;   // start at sub menu
    selectedIndex = 0;   // first option highlighted
    activeField = 0;   // first field active
    loginSuccess = false;
    messageIsError = true;
}

// RESET   called when user comes back to this screen

void Auth::reset()
{
    currentScreen = 0;
    selectedIndex = 0;
    activeField = 0;
    loginSuccess = false;
    clearInputs();
}

// Called from SnowBros constructor after windowW/H are set

void Auth::init(float w, float h)
{
    windowW = w;
    windowH = h;
}

void Auth::clearInputs()
{
    usernameInput = "";
    passwordInput = "";
    emailInput = "";
    message = "";
}

// RESULT GETTERS  -  SnowBros calls these

bool Auth::isLoginSuccessful()
{
    return loginSuccess;
}

// currentScreen == -1 means user pressed ESC from sub-menu
bool Auth::isBackRequested()
{
    return currentScreen == -1;
}

User Auth::getLoggedInUser()
{
    return loggedInUser;
}

// FILE HELPER FUNCTIONS

// Check if '@' exists anywhere in the email string
bool Auth::hasAtSign(string email)
{
    for (int i = 0; i < (int)email.size(); i++)
    {
        if (email[i] == '@')
            return true;
    }
    return false;
}


// hashPassword  (file-local helper)
// Converts a password into an unreadable code string using a custom
// character mapping table  like Morse code but with numbers.

static string hashPassword(const string& password)
{
    // Parallel lookup arrays chars[i] maps to codes[i]
    // a-z (1-26), A-Z (27-52), 0-9 (53-62), symbols (63+)
    const char chars[] =
        "abcdefghijklmnopqrstuvwxyz"    // index 0-25
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"    // index 26-51
        "0123456789"                     // index 52-61
        "!@#$%^&*()_+-=[]{}|;:,.<>?/ "; // index 62-85

    const char* codes[] = {
        // a-z → codes 1-26
        "1","2","3","4","5","6","7","8","9","10",
        "11","12","13","14","15","16","17","18","19","20",
        "21","22","23","24","25","26",
        // A-Z → codes 27-52
        "27","28","29","30","31","32","33","34","35","36",
        "37","38","39","40","41","42","43","44","45","46",
        "47","48","49","50","51","52",
        // 0-9 → codes 53-62
        "53","54","55","56","57","58","59","60","61","62",
        // symbols → codes 63-88
        "63","64","65","66","67","68","69","70","71","72",
        "73","74","75","76","77","78","79","80","81","82",
        "83","84","85","86","87","88"
    };

    int tableSize = 86;  // total entries in lookup table
    string result = "";

    for (int i = 0; i < (int)password.size(); i++)
    {
        char c = password[i];

        // Linear search: scan chars[] until we find matching character
        int foundIndex = -1;
        for (int j = 0; j < tableSize; j++)
        {
            if (chars[j] == c) { foundIndex = j; break; }
        }

        if (i > 0) result += "-";  // separator between codes

        if (foundIndex != -1)
        {
            result += codes[foundIndex];  // use mapped code
        }
        else
        {
            // Falling back for unknown character use raw ASCII value
            int ascii = (int)c;
            string fallback = "";
            if (ascii == 0) fallback = "0";
            while (ascii > 0) { fallback = char('0' + ascii % 10) + fallback; ascii /= 10; }
            result += fallback;
        }
    }
    return result;  // in format like this  "8-9-55-63" for "hi2!"
}

// Read users.txt line by line
// Each line format: username|password|email
// Returns true if a line matches both username AND password
bool Auth::checkLogin(string username, string password)
{
    ifstream file("users.txt");

    if (!file.is_open())
    {
        return false;
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty()) continue;

        // We need 8 parts:
        // 0=username, 1=password, 2=email, 3=gems, 4=currentLevel, 5=highScore
        // 6=created_at, 7=recorded_at
        string parts[8];
        int partIndex = 0;
        string current = "";

        for (int i = 0; i < (int)line.size(); i++)
        {
            if (line[i] == '|')
            {
                if (partIndex < 7)
                {
                    parts[partIndex] = current;
                    partIndex++;
                    current = "";
                }
                else
                {
                    // too many separators, ignore malformed line
                    current = "";
                }
            }
            else
            {
                current += line[i];
            }
        }

        // store last part
        if (partIndex <= 7)
            parts[partIndex] = current;

        // line must have at least 6 parts (fields 0-5 required; 6,7 are new)
        if (partIndex < 5)
            continue;

        if (parts[0] == username && parts[1] == hashPassword(password))
        {
            int g = 0, lvl = 0, hs = 0;

            for (int j = 0; j < (int)parts[3].size(); j++)
            {
                if (parts[3][j] >= '0' && parts[3][j] <= '9')
                    g = g * 10 + (parts[3][j] - '0');
            }

            for (int j = 0; j < (int)parts[4].size(); j++)
            {
                if (parts[4][j] >= '0' && parts[4][j] <= '9')
                    lvl = lvl * 10 + (parts[4][j] - '0');
            }

            for (int j = 0; j < (int)parts[5].size(); j++)
            {
                if (parts[5][j] >= '0' && parts[5][j] <= '9')
                    hs = hs * 10 + (parts[5][j] - '0');
            }

            loggedInUser = User(parts[0], parts[1], parts[2], g, lvl, hs);

            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// Read users.txt and check if username already taken
bool Auth::usernameExists(string username)
{
    ifstream file("users.txt");

    if (!file.is_open())
        return false;   // no file = no users = username is free

    string line;
    while (getline(file, line))
    {
        if (line.empty()) continue;

        // We only need the first part (username) before the first '|'
        string storedUsername = "";
        for (int i = 0; i < (int)line.size(); i++)
        {
            if (line[i] == '|') break;   // stop at first '|'
            storedUsername += line[i];
        }

        if (storedUsername == username)
        {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// Append one new line to users.txt
// Using ios::app so existing users are NOT deleted
void Auth::saveUser(string username, string password, string email)
{
    ofstream file("users.txt", ios::app);

    if (!file.is_open())
    {
        cout << "ERROR: Could not open users.txt for writing" << endl;
        return;
    }

    string hashedPassword = hashPassword(password);
    string today = getCurrentDate();
    file << username << "|" << hashedPassword << "|" << email << "|0|1|0|" << today << "|" << today << "\n";
    file.close();
}

// EVENT HANDLING

void Auth::handleEvent(const Event& event)
{

    // SUB-MENU  (currentScreen == 0)

    if (currentScreen == 0)
    {
        if (const auto* key = event.getIf<Event::KeyPressed>())
        {
            if (key->code == Keyboard::Key::Up)
            {
                selectedIndex = (selectedIndex - 1 + 2) % 2;  // wrap between 0 and 1
            }

            if (key->code == Keyboard::Key::Down)
            {
                selectedIndex = (selectedIndex + 1) % 2;
            }

            if (key->code == Keyboard::Key::Enter)
            {
                if (selectedIndex == 0)
                {
                    // Go to Login form
                    currentScreen = 1;
                    activeField = 0;
                    clearInputs();
                }
                if (selectedIndex == 1)
                {
                    // Go to Register form
                    currentScreen = 2;
                    activeField = 0;
                    clearInputs();
                }
            }

            if (key->code == Keyboard::Key::Escape)
            {
                // Signal SnowBros to go back to main menu
                // We do this by setting currentScreen to -1
                // SnowBros will check this
                currentScreen = -1;
            }
        }
    }


    // LOGIN FORM  (currentScreen == 1)

    else if (currentScreen == 1)
    {
        // --- KeyPressed: navigation and submit ---
        if (const auto* key = event.getIf<Event::KeyPressed>())
        {
            if (key->code == Keyboard::Key::Down)
            {
                // Move to next field - login has 2 fields (0 and 1)
                activeField = (activeField + 1) % 2;
            }

            if (key->code == Keyboard::Key::Up)
            {
                // Move to previous field
                activeField = (activeField - 1 + 2) % 2;
            }

            if (key->code == Keyboard::Key::Enter)
            {
                // Validate - nothing can be empty
                if (usernameInput.empty() || passwordInput.empty())
                {
                    message = "Please fill in all fields!";
                    messageIsError = true;
                    return;
                }

                // Check against file
                if (checkLogin(usernameInput, passwordInput))
                {
                    // SUCCESS - loggedInUser User object already filled in checkLogin()
                    loginSuccess = true;
                    message = "Login successful! Welcome " + usernameInput;
                    messageIsError = false;
                }
                else
                {
                    // WRONG credentials - clear password, stay on screen
                    message = "Wrong username or password. Try again!";
                    messageIsError = true;
                    passwordInput = "";     // clear password for safety
                    activeField = 1;      // put cursor back on password field
                }
            }

            if (key->code == Keyboard::Key::Escape)
            {
                // Back to sub-menu
                currentScreen = 0;
                selectedIndex = 0;
                clearInputs();
            }
        }

        // --- TextEntered: actual character typing ---
        // This fires when user types a real character
        if (const auto* typed = event.getIf<Event::TextEntered>())
        {
            // Unicode 8 = Backspace key
            if (typed->unicode == 8)
            {
                if (activeField == 0 && !usernameInput.empty())
                    usernameInput.pop_back();   // remove last character
                if (activeField == 1 && !passwordInput.empty())
                    passwordInput.pop_back();
            }
            // Unicode 32-126 = normal printable characters (letters, numbers, symbols)
            else if (typed->unicode >= 32 && typed->unicode < 127)
            {
                char c = (char)typed->unicode;

                if (activeField == 0) usernameInput += c;
                if (activeField == 1) passwordInput += c;
            }
        }
    }

    // REGISTER FORM  (currentScreen == 2)

    else if (currentScreen == 2)
    {
        if (const auto* key = event.getIf<Event::KeyPressed>())
        {
            if (key->code == Keyboard::Key::Down)
            {
                // Register has 3 fields (0, 1, 2)
                activeField = (activeField + 1) % 3;
            }

            if (key->code == Keyboard::Key::Up)
            {
                activeField = (activeField - 1 + 3) % 3;
            }

            if (key->code == Keyboard::Key::Enter)
            {
                // --- Validate each field one by one 

                if (usernameInput.empty())
                {
                    message = "Username cannot be empty!";
                    messageIsError = true;
                    activeField = 0;
                    return;
                }

                if (usernameExists(usernameInput))
                {
                    message = "Username already taken. Choose another!";
                    messageIsError = true;
                    activeField = 0;
                    return;
                }

                if (passwordInput.size() < 6)
                {
                    message = "Password must be at least 6 characters!";
                    messageIsError = true;
                    activeField = 1;
                    return;
                }

                if (emailInput.empty())
                {
                    message = "Email cannot be empty!";
                    messageIsError = true;
                    activeField = 2;
                    return;
                }

                if (!hasAtSign(emailInput))
                {
                    message = "Email must contain '@'!";
                    messageIsError = true;
                    activeField = 2;
                    return;
                }

                // All checks passed - save and switch to login 
                saveUser(usernameInput, passwordInput, emailInput);

                // Remember the username to pre-fill on login screen
                string savedUsername = usernameInput;

                clearInputs();
                currentScreen = 1;       // switch to login form
                activeField = 1;       // put cursor on password field
                usernameInput = savedUsername;  // pre-fill username
                message = "Account created! Now log in.";
                messageIsError = false;
            }

            if (key->code == Keyboard::Key::Escape)
            {
                currentScreen = 0;
                selectedIndex = 0;
                clearInputs();
            }
        }

        if (const auto* typed = event.getIf<Event::TextEntered>())
        {
            if (typed->unicode == 8)
            {
                if (activeField == 0 && !usernameInput.empty())
                    usernameInput.pop_back();
                if (activeField == 1 && !passwordInput.empty())
                    passwordInput.pop_back();
                if (activeField == 2 && !emailInput.empty())
                    emailInput.pop_back();
            }
            else if (typed->unicode >= 32 && typed->unicode < 127)
            {
                char c = (char)typed->unicode;
                if (activeField == 0) usernameInput += c;
                if (activeField == 1) passwordInput += c;
                if (activeField == 2) emailInput += c;
            }
        }
    }
}


// DRAW FUNCTIONS


// Draws one labeled input box - same style as  menu items
void Auth::drawField(RenderWindow& window,
    float x, float y, float w, float h,
    string label, string value,
    bool isActive, bool hideText)
{
    // Box outline 
    RectangleShape box;
    box.setSize({ w, h });
    box.setPosition({ x, y });
    box.setFillColor(Color(0, 0, 0, 0));   // transparent fill

    if (isActive)
    {
        // Active field glows cyan like our selected menu item
        box.setOutlineColor(Color(0, 200, 255));
        box.setOutlineThickness(2);
    }
    else
    {
        // Inactive field is dimmer
        box.setOutlineColor(Color(80, 80, 80));
        box.setOutlineThickness(1);
    }
    window.draw(box);

    // --- Label above the box ---
    Text labelText(*font, label, 18);
    labelText.setFillColor(isActive ? Color(0, 200, 255) : Color(160, 160, 160));
    labelText.setPosition({ x, y - 28 });
    window.draw(labelText);

    // --- The typed text inside the box ---
    // For password fields replace every character with '*'
    string display = "";
    if (hideText)
    {
        for (int i = 0; i < (int)value.size(); i++)
            display += '*';
    }
    else
    {
        display = value;
    }

    // Add blinking cursor to active field
    // We use the same clock trick as used for blinking "PRESS ENTER"
    // but we don't have clock here  we just always show cursor for simplicity
    if (isActive) display += "_";

    Text inputText(*font, display, 24);
    inputText.setFillColor(Color::White);
    inputText.setPosition({ x + 12, y + h / 2 - 16 });
    window.draw(inputText);
}

//  SUB-MENU DRAW 
void Auth::drawSubMenu(RenderWindow& window, float t)
{
    // Title
    Text title(*font, "LOGIN / REGISTER", 60);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 255, 255));
    title.setOutlineColor(Color(255, 140, 0));
    title.setOutlineThickness(4);
    title.setPosition({ (windowW - tb.size.x) / 2, windowH / 4 });
    window.draw(title);

    // Two options  exact same logic as your drawMenu options
    const char* options[2] = { "Login", "Register" };
    int menuFontSize = 44;
    float startY = windowH / 2;
    float spacing = 80;

    for (int i = 0; i < 2; i++)
    {
        float y = startY + i * spacing;

        Text item(*font, options[i], menuFontSize);
        FloatRect ib = item.getLocalBounds();
        float x = (windowW - ib.size.x) / 2;

        if (i == selectedIndex)
        {
            float p = sin(t * 5.0f) * 0.5f + 0.5f;
            int g = 200 + (int)(p * 55);

            item.setFillColor(Color(0, g, 255));
            item.setOutlineColor(Color(0, 50, 120));
            item.setOutlineThickness(2);

            // Left arrow
            Text leftArrow(*font, ">>", 30);
            leftArrow.setFillColor(Color(255, 200, 0));
            leftArrow.setPosition({ x - 90, y + 7 });
            window.draw(leftArrow);

            // Right arrow
            Text rightArrow(*font, "<<", 30);
            rightArrow.setFillColor(Color(255, 200, 0));
            rightArrow.setPosition({ x + ib.size.x + 55, y + 7 });
            window.draw(rightArrow);

            // Glow behind selected item
            RectangleShape glow;
            glow.setSize({ ib.size.x + 40, 55 });
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

    // Escape hint at bottom
    Text hint(*font, "ESC = Back to Main Menu", 20);
    hint.setFillColor(Color(80, 80, 80));
    FloatRect hb = hint.getLocalBounds();
    hint.setPosition({ (windowW - hb.size.x) / 2, windowH - 60 });
    window.draw(hint);
}

// ---- LOGIN FORM DRAW ----
void Auth::drawLoginForm(RenderWindow& window, float t)
{
    // Title
    Text title(*font, "LOGIN", 60);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 255, 255));
    title.setOutlineColor(Color(255, 140, 0));
    title.setOutlineThickness(4);
    title.setPosition({ (windowW - tb.size.x) / 2, windowH / 6 });
    window.draw(title);

    // Field dimensions
    float fieldW = 500;
    float fieldH = 55;
    float fieldX = (windowW - fieldW) / 2;

    // Username field
    drawField(window,fieldX, windowH / 2 - 80, fieldW, fieldH,"USERNAME", usernameInput,
        activeField == 0, false);   // not a password field

    // Password field
    drawField(window,
        fieldX, windowH / 2 + 30, fieldW, fieldH,"PASSWORD", passwordInput,
        activeField == 1, true);    // IS a password field - shows ***

    // Message (error or success)
    if (!message.empty())
    {
        Text msg(*font, message, 22);
        FloatRect mb = msg.getLocalBounds();
        msg.setFillColor(messageIsError ? Color(255, 80, 80) : Color(80, 255, 120));
        msg.setPosition({ (windowW - mb.size.x) / 2, windowH / 2 + 120 });
        window.draw(msg);
    }

    // Instructions
    Text hint(*font, "DOWN/UP = switch field   ENTER = submit   ESC = back", 18);
    hint.setFillColor(Color(80, 80, 80));
    FloatRect hb = hint.getLocalBounds();
    hint.setPosition({ (windowW - hb.size.x) / 2, windowH - 60 });
    window.draw(hint);
}

// ---- REGISTER FORM DRAW ----
void Auth::drawRegisterForm(RenderWindow& window, float t)
{
    // Title
    Text title(*font, "REGISTER", 60);
    FloatRect tb = title.getLocalBounds();
    title.setFillColor(Color(255, 255, 255));
    title.setOutlineColor(Color(255, 140, 0));
    title.setOutlineThickness(4);
    title.setPosition({ (windowW - tb.size.x) / 2, windowH / 8 });
    window.draw(title);

    float fieldW = 500;
    float fieldH = 55;
    float fieldX = (windowW - fieldW) / 2;
    float startY = windowH / 3;
    float gap = 110;

    // Username field
    drawField(window,
        fieldX, startY, fieldW, fieldH,
        "USERNAME", usernameInput,
        activeField == 0, false);

    // Password field
    drawField(window,
        fieldX, startY + gap, fieldW, fieldH,
        "PASSWORD  (min 6 characters)", passwordInput,
        activeField == 1, true);

    // Email field
    drawField(window,
        fieldX, startY + gap * 2, fieldW, fieldH,
        "EMAIL", emailInput,
        activeField == 2, false);   // email is not hidden

    // Message
    if (!message.empty())
    {
        Text msg(*font, message, 22);
        FloatRect mb = msg.getLocalBounds();
        msg.setFillColor(messageIsError ? Color(255, 80, 80) : Color(80, 255, 120));
        msg.setPosition({ (windowW - mb.size.x) / 2, startY + gap * 3 });
        window.draw(msg);
    }

    // Instructions
    Text hint(*font, "DOWN/UP = switch field   ENTER = submit   ESC = back", 18);
    hint.setFillColor(Color(80, 80, 80));
    FloatRect hb = hint.getLocalBounds();
    hint.setPosition({ (windowW - hb.size.x) / 2, windowH - 60 });
    window.draw(hint);
}

// ---- MAIN DRAW - SnowBros calls this every frame ----
void Auth::draw(RenderWindow& window)
{
    window.clear(Color::Black);

    float t = clock.getElapsedTime().asSeconds();

    // Snowflakes  copied exactly from our drawMenu to looks consistent
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

    // Top and bottom lines - same as  menu
    RectangleShape topLine;
    topLine.setSize({ windowW, 3 });
    topLine.setFillColor(Color(0, 200, 255));
    topLine.setPosition({ 0, 0 });
    window.draw(topLine);

    RectangleShape botLine;
    botLine.setSize({ windowW, 3 });
    botLine.setFillColor(Color(0, 200, 255));
    botLine.setPosition({ 0, windowH - 3 });
    window.draw(botLine);

    // Draw the correct screen based on currentScreen
    if (currentScreen == 0) drawSubMenu(window, t);
    else if (currentScreen == 1) drawLoginForm(window, t);
    else if (currentScreen == 2) drawRegisterForm(window, t);

    window.display();
}