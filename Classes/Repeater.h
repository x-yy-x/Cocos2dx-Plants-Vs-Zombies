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
     * @param globalPos Touch position in global coordinates
     * @return Repeater* Returns Repeater instance on success, nullptr on failure
     */
    static Repeater* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Check for zombies and attack with two peas (override from PeaShooter)
     * @param zombiesInRow All zombies in the same row
     * @return std::vector<Bullet*> Returns vector containing two Pea bullets if attack happened
     */
    virtual std::vector<Bullet*> checkAndAttack(const std::vector<Zombie*>& zombiesInRow) override;

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;

    /**
     * @brief Set up animation frames for Repeater
     */
    virtual void setAnimation() override;

    // Protected constructor
    Repeater();
};

