#include "StatsComponent.h"

StatsComponent::StatsComponent(int startLives)
{
    lives = startLives;
    score = 0;
    isDead = false;
    deathAnimDone = false;
}

void StatsComponent::loseLife()
{
    lives--;
    isDead = true;
    if (lives < 0) lives = 0;
}

void StatsComponent::addLife()
{
    lives++;
}

void StatsComponent::addScore(int points)
{
    score += points;
}

void StatsComponent::setDeathDone(bool val)
{
    deathAnimDone = val;
}

void StatsComponent::revive()
{
    isDead = false;
    deathAnimDone = false;
}

int StatsComponent::getLives()
{
    return lives;
}

int StatsComponent::getScore()
{
    return score;
}

bool StatsComponent::getIsDead()
{
    return isDead;
}

bool StatsComponent::getDeathDone()
{
    return deathAnimDone;
}