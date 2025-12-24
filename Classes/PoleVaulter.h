
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
class PoleVaulter : public Zombie
{
public:
    /**
     * @brief Zombie state enumeration
     */
    enum  class ZombieState
    {
        RUNNING,
        JUMPING,
        WALKING,      // Walking state
        EATING,       // Eating plant state
        DYING         // Dying state
    };

    /**
     * @brief Zombie initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static Zombie* create() function
    CREATE_FUNC(PoleVaulter);

    /**
     * @brief Static factory method to create a zombie with animations
     * @return Zombie* Created zombie instance
     */
    static PoleVaulter* createZombie();

    cocos2d::Sprite* createShowcaseSprite(const cocos2d::Vec2& pos) ;

    /**
     * @brief Update function called every frame for movement, attack, death, etc.
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Set zombie state
     * @param newState New state
     */
    void setState(ZombieState newState);

    /**
     * @brief Check and handle plant encounters
     * @param plants Vector of all plants in the scene
     */
    virtual void encounterPlant(const std::vector<Plant*>& plants) override;


protected:
    PoleVaulter();

    virtual ~PoleVaulter();

    void initRunningAnimation();

    void initJumpingAnimation();

    void initWalkAnimation();

    void initEatAnimation();

    void setAnimationForState(ZombieState state);

    void startJumping();

    static const float RUNNING_SPEED;

    ZombieState _currentState;
    
    cocos2d::RepeatForever* _walkAction;
    cocos2d::RepeatForever* _eatAction;
    cocos2d::RepeatForever* _runAction;
    cocos2d::Animate* _jumpAction;

    bool _isJumping;
    bool _hasJumped;

};
