
#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include "Zombie.h"
#include <vector>

// Forward declaration
class Plant;

/**
 * @brief Zombie class, inherits from GameObject.
 * Zombies have different states: walking, eating plants, dying.
 * Zombie can be directly instantiated as a normal zombie.
 */
class Gargantuar : public Zombie
{
public:

    enum class ZombieState
    {
        DYING,
        WALKING,
        SMASHING,
        THROWING
    };

    /**
     * @brief Zombie initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static Zombie* create() function
    CREATE_FUNC(Gargantuar);

    /**
     * @brief Static factory method to create a zombie with animations
     * @return Zombie* Created zombie instance
     */
    static Gargantuar* createZombie();

    // 选卡展示静态图
    cocos2d::Sprite* createShowcaseSprite(const cocos2d::Vec2& pos);

    /**
     * @brief Update function called every frame for movement, attack, death, etc.
     * @param delta Time delta
     */
    //virtual void update(float delta) override;



    /**
     * @brief Check and handle plant encounters
     * @param plants Vector of all plants in the scene
     */
    virtual void encounterPlant(const std::vector<Plant*>& plants) override;

    /**
     * @brief Get coin drop bonus multiplier for this zombie type
     * @return Coin drop bonus multiplier (2.0f for Gargantuar)
     */
    virtual float getCoinDropBonus() const override { return 2.0f; }


protected:
    // Protected constructor
    Gargantuar();

    // Virtual destructor
    virtual ~Gargantuar();

    /**
     * @brief Initialize walking animation
     */
    void initWalkAnimation();

    /**
     * @brief Initialize eating animation
     */
    void initSmashAnimation();


    void initThrowAnimation();

    virtual void updateMoving(float delta) override;

    virtual void updateEating(float delta) override;

    /**
     * @brief Set animation corresponding to state
     * @param state Target state
     */
    virtual void setAnimationForState() override;

    void throwImp();
  
    cocos2d::RepeatForever* _walkAction;
    cocos2d::Animate* _smashAction;
    cocos2d::Animate* _prethrowAction;
    cocos2d::Animate* _postthrowAction;
              
    bool _isThrowing;
    bool _hasthrown;
};
