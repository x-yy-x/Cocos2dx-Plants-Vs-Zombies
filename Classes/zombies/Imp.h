
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
 * @brief Imp class, inherits from GameObject.
 * Imps have different states: walking, eating plants, dying.
 * Imp can be directly instantiated as a normal zombie.
 */
class Imp : public Zombie
{
public:
    enum class ZombieState
    {
        DYING,
        WALKING,
        EATING,
        FLYING
    };

    /**
     * @brief Zombie initialization function
     */
    virtual bool init() override;

    // Implement the static create() function
    CREATE_FUNC(Imp);

    /**
     * @brief Static factory method to create a zombie with animations
     * @return Imp* Created zombie instance
     */
    static Imp* createZombie();

    virtual void updateMoving(float delta) override;

    /**
     * @brief Check and handle plant encounters
     * @param plants Vector of all plants in the scene
     */
    void encounterPlant(const std::vector<Plant*>& plants) override;

    /**
     * @brief Get coin drop bonus multiplier for this zombie type
     * @return Coin drop bonus multiplier (1.2f for Imp)
     */
    inline virtual float getCoinDropBonus() const override { return 1.2f; }

private:
    // Protected constructor
    Imp();

    // Virtual destructor
    virtual ~Imp();

    /**
     * @brief Initialize animation
     */
    void initWalkAnimation();

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
    cocos2d::RepeatForever* walk_action;
    cocos2d::RepeatForever* eat_action;
    cocos2d::Animate* fly_action;

    bool is_flying;
};
