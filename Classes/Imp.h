
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
        WALKING,      // Walking state
        EATING,       // Eating plant state
        DYING,         // Dying state
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


    /**
     * @brief Set zombie state
     * @param newState New state
     */
    void setState(ZombieState newState);



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
    void setAnimationForState(ZombieState state);


    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    ZombieState _currentState;       // Current state
    
    // Animation actions
    cocos2d::RepeatForever* _walkAction;
    cocos2d::RepeatForever* _eatAction;
    cocos2d::Animate* _flyAnimate;

    bool _isFlying;
    bool _hasBeenThrown;

    static const float MOVE_SPEED;
};
