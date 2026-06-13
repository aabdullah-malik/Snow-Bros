#include "Platform.h"
#include <cmath>

Platform::Platform() : hitbox (0,0)  // needed in levels.cpp
{
    platformX = 0;
    platformY = 0;
    platformW = 0;
    platformH = 0;
    baseColor = Color::White;
    waveColor = Color::White;
}

Platform::Platform(float x, float y, float w, float h, Color base) : hitbox(w, h)
{
    platformX = x; platformY = y; platformW = w; platformH = h;
    baseColor = base;

    int r = (base.r + 60 > 255) ? 255 : base.r + 60;
    int g = (base.g + 80 > 255) ? 255 : base.g + 80;
    int b = (base.b + 100 > 255) ? 255 : base.b + 100;

    waveColor = Color(r, g, b);
    hitbox.update(platformX, platformY);
}

void Platform::draw(RenderWindow& window)
{
    // Base rectangle
    RectangleShape base;
    base.setSize({ platformW, platformH });
    base.setFillColor(baseColor);
    base.setPosition({ platformX, platformY });
    window.draw(base);

    // Wave layers 
    float rectWidth = 4.f;
    for (float x = platformX; x < platformX + platformW; x += rectWidth)  // draws a strip after every 4px
    {
        float cur = x - platformX;  // current distance from left side

        float sine1 = sinf(cur * 0.12f);
        float waveY1 = platformY + platformH * 0.3f + sine1 * platformH * 0.25f;
        RectangleShape wave1;
        wave1.setSize({ rectWidth, (platformY + platformH) - waveY1 });
        wave1.setFillColor(waveColor);
        wave1.setPosition({ x, waveY1 });
        window.draw(wave1);

        float sine2 = sinf(cur * 0.12f + 1.0f);
        float waveY2 = platformY + platformH * 0.45f + sine2 * platformH * 0.2f;
        Color w2(waveColor.r > 40 ? waveColor.r - 40 : 0,
            waveColor.g > 40 ? waveColor.g - 40 : 0,
            waveColor.b > 40 ? waveColor.b - 40 : 0);
        RectangleShape wave2;
        wave2.setSize({ rectWidth, (platformY + platformH) - waveY2 });
        wave2.setFillColor(w2);
        wave2.setPosition({ x, waveY2 });
        window.draw(wave2);
    }

    // Snow cap
    RectangleShape snowCap;
    snowCap.setSize({ platformW, platformH * 0.22f });
    snowCap.setFillColor(Color(240, 245, 255));
    snowCap.setPosition({ platformX, platformY });
    window.draw(snowCap);

    for (float x = platformX + 4; x < platformX + platformW - 4; x += 15)
    {
        CircleShape dot(2);
        dot.setFillColor(Color(waveColor.r, waveColor.g, waveColor.b, 160));
        dot.setPosition({ x, platformY + 4 });
        window.draw(dot);
    }

    // Top / bottom edges
    RectangleShape topEdge;
    topEdge.setSize({ platformW, 2 });
    topEdge.setFillColor(Color(255, 255, 255));
    topEdge.setPosition({ platformX, platformY });
    window.draw(topEdge);

    RectangleShape botEdge;
    botEdge.setSize({ platformW, 2 });
    botEdge.setFillColor(Color(baseColor.r > 30 ? baseColor.r - 30 : 0,
        baseColor.g > 30 ? baseColor.g - 30 : 0,
        baseColor.b > 30 ? baseColor.b - 30 : 0));
    botEdge.setPosition({ platformX, platformY + platformH - 2 });
    window.draw(botEdge);
}

void Platform::drawDebug(RenderWindow& window)
{
    hitbox.draw(window, Color::Blue);
}

FloatRect Platform::getRect()
{
    return FloatRect({ platformX, platformY }, { platformW, platformH });
}
