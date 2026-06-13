#pragma once

class StatsComponent
{
private:
    int lives;
    int score;
    bool isDead;
    bool deathAnimDone;

public:
    StatsComponent(int startLives);

    void loseLife();
    void addLife();
    void addScore(int points);
    void setDeathDone(bool val);
    void revive();

    int  getLives();
    int  getScore();
    bool getIsDead();
    bool getDeathDone();
};