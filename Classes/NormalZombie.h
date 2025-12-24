
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

    // Showcase: 默认展示（静态图片），用于选卡场景右侧展示
    // 子类可按需覆盖，pos为建议初始位置（调用方也可重设）
    cocos2d::Sprite* createShowcaseSprite(const cocos2d::Vec2& pos);


    /**
     * @brief Set zombie state
     * @param newState New state
     */
    void setState(ZombieState newState);
    

protected:

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
    void setAnimationForState(ZombieState state);

  
    // Animation actions
    cocos2d::RepeatForever* _walkAction;
    cocos2d::RepeatForever* _eatAction;
};
