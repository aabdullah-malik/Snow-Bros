#pragma once
#include "Player.h"

class Tom : public Player
{
public:
    Tom(float x, float y, float wW, float wH);
    void loadSprite() override;
    void setupFrames() override;
    ~Tom() {}
};