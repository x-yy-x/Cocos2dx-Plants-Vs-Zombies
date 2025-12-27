#pragma once

#include "cocos2d.h"
#include "AttackingPlant.h"
#include "GameDefs.h"
#include "Pea.h"

/**
 * @brief ThreePeater class - shoots peas in three lanes simultaneously
 * Inherits from AttackingPlant
 */
class ThreePeater : public AttackingPlant
{
public:
    /**
     * @brief ThreePeater initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static ThreePeater* create() function
    CREATE_FUNC(ThreePeater);

    /**
     * @brief Static planting function for ThreePeater
     */
    static ThreePeater* plantAtPosition(const cocos2d::Vec2& globalPos)
    {
        return createPlantAtPosition<ThreePeater>(globalPos);
    }

    /**
     * @brief Check for zombies and attack with three peas in three lanes
     * @param allZombiesInRow All zombies in each row
     * @param plantRow The row this plant is in
     * @return std::vector<Bullet*> Returns vector containing up to 3 Pea bullets (one per lane)
     */
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;

private:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;

    /**
     * @brief Set up animation frames for ThreePeater
     */
    virtual void setAnimation() override;

    // Protected constructor
    ThreePeater();
};

