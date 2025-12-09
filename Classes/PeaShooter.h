#pragma once

#include "cocos2d.h"
#include "AttackingPlant.h"
#include "GameDefs.h"
#include "Pea.h"

class PeaShooter : public AttackingPlant
{
public:
    /**
     * @brief PeaShooter initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static PeaShooter* create() function
    CREATE_FUNC(PeaShooter);

    /**
     * @brief Static planting function for PeaShooter.
     * @param globalPos Touch position in global coordinates
     * @return PeaShooter* Returns PeaShooter instance on success, nullptr on failure
     */
    static PeaShooter* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Check for zombies and attack if possible (override from AttackingPlant)
     * @param zombiesInRow All zombies in the same row
     * @return std::vector<Bullet*> Returns vector containing Pea bullet if attack happened, empty otherwise
     */
    virtual std::vector<Bullet*> checkAndAttack(const std::vector<Zombie*>& zombiesInRow) override;

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_RANGE;  // PeaShooter's attack range

    /**
     * @brief Set up animation frames
     */
    virtual void setAnimation() override;

    // Protected constructor
    PeaShooter();
};
