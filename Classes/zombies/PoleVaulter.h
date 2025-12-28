
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
 * @brief PoleVaulter class, inherits from Zombie.
 * This is a zombie that can pole vault over plants using its pole.
 * It has multiple states including running, jumping, walking, and eating.
 * After jumping over a plant, it will attack the next plant it encounters.
 */
class PoleVaulter : public Zombie
{
public:
    enum class ZombieState
    {
        DYING,
        WALKING,
        EATING,
        RUNNING,
        JUMPING
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
     * @brief Check and handle plant encounters
     * @param plants Vector of all plants in the scene
     */
    virtual void encounterPlant(const std::vector<Plant*>& plants) override;

    virtual float getCoinDropBonus() const override { return 1.2f; }

private:
    /**
     * @brief Protected constructor
     */
    PoleVaulter();

    /**
     * @brief Virtual destructor with proper cleanup
     */
    virtual ~PoleVaulter();

    /**
     * @brief Initialize running animation
     */
    void initRunningAnimation();

    /**
     * @brief Initialize jumping animation
     */
    void initJumpingAnimation();

    /**
     * @brief Initialize walking animation
     */
    void initWalkAnimation();

    /**
     * @brief Initialize eating animation
     */
    void initEatAnimation();

    /**
     * @brief Set animation based on current state
     */
    virtual void setAnimationForState() override;

    /**
     * @brief Start jumping/pole vaulting sequence
     */
    void startJumping();

    // ----------------------------------------------------
    // Constants
    // ----------------------------------------------------
    static const float RUNNING_SPEED;        // Running speed of PoleVaulter

    // ----------------------------------------------------
    // Animation actions
    // ----------------------------------------------------
    cocos2d::RepeatForever* walk_action;     // Walking animation action
    cocos2d::RepeatForever* eat_action;      // Eating animation action
    cocos2d::RepeatForever* run_action;      // Running animation action
    cocos2d::Animate* jump_action;           // Jumping animation action

    // ----------------------------------------------------
    // State flags
    // ----------------------------------------------------
    bool is_jumping;                         // Flag indicating if jumping
    bool has_jumped;                         // Flag indicating if already jumped once

};
