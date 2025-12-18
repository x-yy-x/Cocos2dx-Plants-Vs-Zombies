#pragma once

#include "cocos2d.h"
#include "SunProducingPlant.h"
#include "UpgradedPlant.h"
#include "GameDefs.h"
#include "Sunflower.h"

// Forward declaration
class Sun;

class TwinSunflower : public SunProducingPlant, public UpgradedPlant
{
public:
    /**
     * @brief PeaShooter initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static PeaShooter* create() function
    CREATE_FUNC(TwinSunflower);

    /**
     * @brief Static planting function for PeaShooter.
     * @param globalPos Touch position in global coordinates
     * @return PeaShooter* Returns PeaShooter instance on success, nullptr on failure
     */
    static TwinSunflower* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Produce sun if cooldown is finished (override from SunProducingPlant)
     * @return Sun* Returns Sun instance if ready, nullptr otherwise
     */
    virtual std::vector<Sun*> produceSun() override;

    virtual bool canUpgrade(Plant* basePlant) const override;

    virtual TwinSunflower* upgrade(Plant* basePlant) override;

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_RANGE;  // PeaShooter's attack range
    static const float SUN_PRODUCTION_INTERVAL;  // Time between sun productions (24 seconds)

    /**
     * @brief Set up animation frames
     */
    virtual void setAnimation() override;

    // Protected constructor
    TwinSunflower();
};
