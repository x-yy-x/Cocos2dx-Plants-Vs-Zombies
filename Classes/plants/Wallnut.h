#pragma once

#include "cocos2d.h"
#include "AttackingPlant.h"
#include "GameDefs.h"

class Wallnut : public AttackingPlant
{
public:
    /**
     * @brief PeaShooter initialization function
     */
    virtual bool init() override;

    enum class WallnutState
    {
        NORMAL,
        CRACKED
    };

    // Implement the auto-generated static PeaShooter* create() function
    CREATE_FUNC(Wallnut);

    /**
     * @brief Static planting function for PeaShooter.
     * @param globalPos Touch position in global coordinates
     * @return PeaShooter* Returns PeaShooter instance on success, nullptr on failure
     */
    static Wallnut* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Wallnut does not attack (override from AttackingPlant)
     * @return Always returns empty vector
     */
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_RANGE;  // PeaShooter's attack range

    cocos2d::RepeatForever* normalAnimation;
    cocos2d::RepeatForever* crackedAnimation;

    WallnutState _currentState;

    /**
     * @brief Set up animation frames
     */
    virtual void setAnimation() override;

    void setCrackedAnimation();

    // Protected constructor
    Wallnut();
};
