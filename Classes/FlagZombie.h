
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
    /**
     * @brief Zombie state enumeration
     */
    enum class ZombieState
    {
        WALKING,      // Walking state
        EATING,       // Eating plant state
        DYING         // Dying state
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
    virtual cocos2d::Sprite* createShowcaseSprite(const cocos2d::Vec2& pos) override;

    /**
     * @brief Update function called every frame for movement, attack, death, etc.
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Get current zombie state
     * @return ZombieState Current state
     */
    ZombieState getState() const;

    /**
     * @brief Set zombie state
     * @param newState New state
     */
    void setState(ZombieState newState);

    /**
     * @brief Check if zombie is dead.
     * @return true if dead, false if alive
     */
    bool isDead() const;

    /**
     * @brief Apply damage to zombie and reduce health.
     * @param damage Damage value
     */
    void takeDamage(int damage);

    /**
     * @brief Check and handle plant encounters
     * @param plants Vector of all plants in the scene
     */
    virtual void encounterPlant(const std::vector<Plant*>& plants);

protected:
    // Protected constructor
    FlagZombie();

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
     * @brief Set up animation
     */
    virtual void setAnimation();

    /**
     * @brief Set animation corresponding to state
     * @param state Target state
     */
    void setAnimationForState(ZombieState state);

    /**
     * @brief Check collision with plants
     */
    void checkCollision(const std::vector<Plant*>& plants);

    /**
     * @brief Start eating a plant
     * @param plant Target plant to eat
     */
    void startEating(Plant* plant);

    /**
     * @brief Called when the plant being eaten dies
     */
    void onPlantDied();

    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float MOVE_SPEED;     // Movement speed
    static const float ATTACK_DAMAGE;  // Attack damage per hit
    static const float ATTACK_RANGE;   // Attack range

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    ZombieState _currentState;       // Current state
    bool _isDead;                    // Is dead flag
    int _maxHealth;                  // Maximum health
    int _currentHealth;              // Current health
    float _attackInterval;           // Attack interval
    float _accumulatedTime;          // Cooldown accumulated time
    cocos2d::Vec2 _zombiePos;        // Zombie position
    
    // Animation actions
    cocos2d::RepeatForever* _walkAction;
    cocos2d::RepeatForever* _eatAction;
    
    // Eating state
    bool _isEating;                  // Is currently eating
    Plant* _targetPlant;             // Target plant being eaten
    float _speed;                    // Current movement speed
    float _normalSpeed;              // Normal walking speed
};
