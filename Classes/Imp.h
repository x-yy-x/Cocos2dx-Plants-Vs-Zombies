
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
class Imp : public Zombie
{
public:
    /**
     * @brief Zombie state enumeration
     */
    enum class ZombieState
    {
        DYING,
        WALKING,      // Walking state
        EATING,       // Eating plant state
        FLYING
    };

    /**
     * @brief Zombie initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static Zombie* create() function
    CREATE_FUNC(Imp);

    /**
     * @brief Static factory method to create a zombie with animations
     * @return Zombie* Created zombie instance
     */
    static Imp* createZombie();

    virtual void update(float delta) override;


protected:
    // Protected constructor
    Imp();

    // Virtual destructor
    virtual ~Imp();

    /**
     * @brief Initialize walking animation
     */
    void initWalkAnimation();

    /**
     * @brief Initialize eating animation
     */
    void initEatAnimation();

    void initFlyAnimation();


    /**
     * @brief Set animation corresponding to state
     * @param state Target state
     */
    virtual void setAnimationForState() override;


    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------

    
    // Animation actions
    cocos2d::RepeatForever* _walkAction;
    cocos2d::RepeatForever* _eatAction;
    cocos2d::Animate* _flyAnimate;

    bool _isFlying;
    bool _hasBeenThrown;


};
