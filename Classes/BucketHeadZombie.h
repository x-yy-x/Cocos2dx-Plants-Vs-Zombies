
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
class BucketHeadZombie : public Zombie
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
    CREATE_FUNC(BucketHeadZombie);

    /**
     * @brief Static factory method to create a zombie with animations
     * @return Zombie* Created zombie instance
     */
    static BucketHeadZombie* createZombie();

    // 选卡展示静态图
    cocos2d::Sprite* createShowcaseSprite(const cocos2d::Vec2& pos) ;


    /**
     * @brief Apply damage to zombie and reduce health.
     * @param damage Damage value
     */
    virtual void takeDamage(float damage) override;

    /**
     * @brief Get coin drop bonus multiplier for this zombie type
     * @return Coin drop bonus multiplier (1.4f for BucketHeadZombie)
     */
    virtual float getCoinDropBonus() const override { return 1.4f; }

    /**
     * @brief Check if this zombie should play metal hit sound effect when hit by bullet
     * @return true only if this zombie still has bucket head
     */
    virtual bool playsMetalHitSound() const override { return hasBucketHead(); }

    inline bool hasBucketHead() const { return !_useNormalZombie; }

protected:

    // Virtual destructor
    virtual ~BucketHeadZombie();

    /**
     * @brief Initialize walking animation
     */
    void initWalkAnimation();

    /**
     * @brief Initialize eating animation
     */
    void initEatAnimation();

    cocos2d::RepeatForever* createNormalWalkActionFromFrame(int frameIndex);

    cocos2d::RepeatForever* createNormalEatActionFromFrame(int frameIndex);

    /**
     * @brief Set animation corresponding to state
     * @param state Target state
     */
    virtual void setAnimationForState() override;

    void onBucketBroken();

    // Animation actions
    cocos2d::RepeatForever* _walkAction;
    cocos2d::RepeatForever* _eatAction;
    cocos2d::RepeatForever* _normalWalkAction;
    cocos2d::RepeatForever* _normalEatAction;

    bool _useNormalZombie = false;
    float _bucketHealth = 1000;

};
