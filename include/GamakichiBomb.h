#pragma once
#include "Enemy.h"

class GamakichiBomb : public Enemy
{
protected:
    static Texture sharedTexture;
    static bool textureLoaded;
    static bool showHitbox;
public:
    GamakichiBomb(float x, float y, float wW, float wH);

    static void preloadTexture();
    void setColor(int color) override;
    void loadSprite() override;
    void setupFrames() override;
    void update(float dt, Platform* platforms, int platformCount) override;
    void draw(RenderWindow& window) override;
    void die();
    int getEnemyType() const override { return 7; }
    void setAlive(bool a) { alive = a; }
    FloatRect getHitboxRect() { return hitbox.getRect(); }
    static void setShowHitbox(bool val) { showHitbox = val; }

    ~GamakichiBomb() {}
};