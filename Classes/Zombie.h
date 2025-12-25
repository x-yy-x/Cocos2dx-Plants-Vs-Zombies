
#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include "audio/include/AudioEngine.h"
#include <vector>
#include <string>

// Forward declaration
class Plant;

/**
 * @brief Zombie class, inherits from GameObject.
 * Zombies have different states: walking, eating plants, dying.
 * Zombie can be directly instantiated as a normal zombie.
 */
class Zombie : public GameObject
{
public:

    virtual bool init() = 0;

    cocos2d::Animation* initAnimate(const std::string& fileName, float frameWidth,
        float frameHeight, int row, int col, int frameCount, float delay);

    virtual void update(float delta);

    void setState(int newState);

    virtual void setAnimationForState() = 0;

    virtual void takeDamage(float damage);

    virtual void encounterPlant(const std::vector<Plant*>& plants);

    bool isDead() const { return _isDead && !_isDying; }

    virtual void startEating(Plant* plant);

    virtual void onPlantDied();


    int _currentState = 1;
    //0 dying
    //1 walking
    //2 eating
    bool _isDying = false;
    bool _isDead = false;
    int _currentHealth = 200;
    float _accumulatedTime = 0.0f;
    bool _isEating = false;
    Plant* _targetPlant = nullptr;
    float _currentSpeed = 20.0f;

    float MOVE_SPEED = 20.0f;
    float ATTACK_DAMAGE = 10.0f;
    float ATTACK_INTERVAL = 0.5f;
    int MAX_HEALTH = 200;
    float X_CORRECTION = 40.0f;
    float SIZE_CORRECTION = 100.0f;
};
