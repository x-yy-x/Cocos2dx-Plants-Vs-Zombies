#pragma once

#include "cocos2d.h"
#include "AttackingPlant.h"
#include "GameDefs.h"
#include "Zombie.h"
#include "Zomboni.h"

class SpikeWeed : public AttackingPlant
{
public:
    /**
     * @brief SpikeWeed initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static SpikeWeed* create() function
    CREATE_FUNC(SpikeWeed);

    /**
     * @brief Static planting function for SpikeWeed.
     * @param globalPos Touch position in global coordinates
     * @return SpikeWeed* Returns SpikeWeed instance on success, nullptr on failure
     */
    static SpikeWeed* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Check for zombies and attack if possible (override from AttackingPlant)
     * @param allZombiesInRow All zombies in each row
     * @param plantRow The row this plant is in
     * @return std::vector<Bullet*> Returns vector containing Pea bullet if attack happened, empty otherwise
     */
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;

    /**
     * @brief Check if the plant is a spike plant.
     * @return true for spike plants
     */
    virtual bool isSpike() const override { return true; }

    /**
     * @brief Check if this plant can be upgraded to the specified plant type.
     * @param upgradePlantName The target plant type to upgrade to
     * @return true if this plant can be upgraded to SpikeRock
     */
    virtual bool canBeUpgradedTo(PlantName upgradePlantName) const override { 
        return upgradePlantName == PlantName::SPIKEROCK; 
    }

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;

    const int _cooldownInterval = 2;
    
    /**
     * @brief Set up animation frames
     */
    virtual void setAnimation() override;

    // Protected constructor
    SpikeWeed();

};
