#pragma once

#include "cocos2d.h"
#include "PeaShooter.h"
#include "GameDefs.h"

/**
 * @brief Repeater class - shoots two peas at a time
 * Inherits from PeaShooter
 */
class Repeater : public PeaShooter
{
public:
    /**
     * @brief Repeater initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static Repeater* create() function
    CREATE_FUNC(Repeater);

    /**
     * @brief Static planting function for Repeater
     */
    static Repeater* plantAtPosition(const cocos2d::Vec2& globalPos)
    {
        return createPlantAtPosition<Repeater>(globalPos);
    }

    /**
     * @brief Check for zombies and attack with two peas (override from PeaShooter)
     * @param allZombiesInRow All zombies in each row
     * @param plantRow The row this plant is in
     * @return std::vector<Bullet*> Returns vector containing two Pea bullets if attack happened
     */
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;

    /**
     * @brief Set up animation frames for Repeater
     */
    virtual void setAnimation() override;

    // Protected constructor
    Repeater();
};

