
#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include "Zombie.h"
#include "audio/include/AudioEngine.h"
#include <vector>

// Forward declaration
class Plant;

/**
 * @brief NormalZombie class, inherits from GameObject.
 * Zombies have different states: walking, eating plants, dying.
 * NormalZombie can be directly instantiated as a normal zombie.
 */
class NormalZombie : public Zombie
{
public:

    enum class ZombieState
    {
        DYING,
        WALKING,
        EATING
    };
    /**
     * @brief NormalZombie initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static NormalZombie* create() function
    CREATE_FUNC(NormalZombie);

    /**
     * @brief Static factory method to create a zombie with animations
     * @return NormalZombie* Created zombie instance
     */
    static NormalZombie* createZombie();


    cocos2d::Sprite* createShowcaseSprite(const cocos2d::Vec2& pos);    

private:

    // Virtual destructor
    virtual ~NormalZombie();

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

  
    // Animation actions
    cocos2d::RepeatForever* walk_action;
    cocos2d::RepeatForever* eat_action;
};
