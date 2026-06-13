#pragma once
#include "Player.h"

class Nick : public Player
{
public:
    Nick(float x, float y, float wW, float wH);
    void loadSprite()  override;
    void setupFrames() override;
    ~Nick() {}
};
