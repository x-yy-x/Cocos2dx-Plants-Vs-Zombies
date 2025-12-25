
#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include "Zombie.h"
#include "SpikeWeed.h"
#include "SpikeRock.h"
#include "IceTile.h"
#include <vector>

// Forward declaration
class Plant;

/**
 * @brief Zombie class, inherits from GameObject.
 * Zombies have different states: walking, eating plants, dying.
 * Zombie can be directly instantiated as a normal zombie.
 */
class Zomboni : public Zombie
{
public:

    enum class ZombieState
    {
        DYING,
        DRIVING,
        NOTHING,
        SPECIAL
    };


    virtual bool init() override;

    // Implement the auto-generated static Zombie* create() function
    CREATE_FUNC(Zomboni);

    /**
     * @brief Static factory method to create a zombie with animations
     * @return Zombie* Created zombie instance
     */
    static Zomboni* createZombie();

    // 选卡展示静态图
    cocos2d::Sprite* createShowcaseSprite(const cocos2d::Vec2& pos);

    /**
     * @brief Update function called every frame for movement, attack, death, etc.
     * @param delta Time delta
     */
    virtual void update(float delta) override;


    void setSpecialDeath();


protected:
    // Protected constructor
    Zomboni();

    // Virtual destructor
    virtual ~Zomboni();

    /**
     * @brief Initialize walking animation
     */
    void initDriveAnimation();

    /**
     * @brief Initialize eating animation
     */
    void initSpecialDieAnimation();


    /**
     * @brief Set animation corresponding to state
     * @param state Target state
     */
    virtual void setAnimationForState() override;

    void spawnIce();


    static const float MOVE_SPEED;
    static const float ICE_STEP;
    static const float ICE_LENGTH ;
    static const int ICE_COUNT;
    static const int MAX_HEALTH;

    float _iceAccumulate;
    int   _iceIndex;
    
    // Animation actions
    cocos2d::RepeatForever* _driveAction;
    cocos2d::Animate* _specialDieAction;
    
};
