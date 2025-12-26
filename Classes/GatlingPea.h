#pragma once

#include "cocos2d.h"
#include "AttackingPlant.h"
#include "UpgradedPlant.h"
#include "GameDefs.h"
#include "Repeater.h"

// Forward declaration
class Sun;

class GatlingPea : public AttackingPlant, public UpgradedPlant
{
public:
    /**
     * @brief PeaShooter initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static PeaShooter* create() function
    CREATE_FUNC(GatlingPea);

    /**
     * @brief Static planting function for PeaShooter.
     * @param globalPos Touch position in global coordinates
     * @return PeaShooter* Returns PeaShooter instance on success, nullptr on failure
     */
    static GatlingPea* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Check if this plant can be upgraded to the specified plant type.
     * @param upgradePlantName The target plant type to upgrade to
     * @return false (GatlingPea cannot be upgraded further)
     */
    
    /**
     * @brief Get plant category (explicit override to resolve inheritance dominance warning)
     */
    virtual PlantCategory getCategory() const override { return PlantCategory::ATTACKING; }
    virtual bool canBeUpgradedTo(PlantName upgradePlantName) const override { 
        return false; 
    }

    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;

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
    GatlingPea();
};
