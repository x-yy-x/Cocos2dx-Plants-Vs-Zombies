
#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include "Zombie.h"
#include "SpikeWeed.h"
#include <vector>

// Forward declaration
class Plant;

/**
 * @brief Zombie class, inherits from GameObject.
 * Zombies have different states: walking, eating plants, dying.
 * Zombie can be directly instantiated as a normal zombie.
 */
class FlagZombie : public Zombie
{
public:

    enum class ZombieState
    {
        DYING,
        WALKING,
        EATING
    };

    /**
     * @brief Zombie initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static Zombie* create() function
    CREATE_FUNC(FlagZombie);

    /**
     * @brief Static factory method to create a zombie with animations
     * @return Zombie* Created zombie instance
     */
    static FlagZombie* createZombie();

    // 选卡展示静态图
    virtual cocos2d::Sprite* createShowcaseSprite(const cocos2d::Vec2& pos) ;



protected:

    // Virtual destructor
    virtual ~FlagZombie();

    /**
     * @brief Initialize walking animation
     */
    void initWalkAnimation();

    /**
     * @brief Initialize eating animation
     */
    void initEatAnimation();


    /**
     * @brief Set animation corresponding to state
     * @param state Target state
     */
    virtual void setAnimationForState() override;



    cocos2d::RepeatForever* _walkAction;
    cocos2d::RepeatForever* _eatAction;

};
