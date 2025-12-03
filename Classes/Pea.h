#pragma once

#include "cocos2d.h"
#include "Bullet.h"

/**
 * @brief Pea bullet class, inherits from Bullet.
 * Fired by PeaShooter.
 */
class Pea : public Bullet
{
public:
    /**
     * @brief Initialization function
     */
    virtual bool init() override;

    // Implement CREATE_FUNC manually or use macro if constructor is public/protected match
    // Since we need a custom create method for position, we can add a static create method
    static Pea* create(const cocos2d::Vec2& startPos);

    // Implement standard CREATE_FUNC for consistency
    CREATE_FUNC(Pea);

protected:
    // Protected constructor
    Pea();

    /**
     * @brief Update movement logic (move forward)
     * @param delta Time delta
     */
    virtual void updateMovement(float delta) override;

    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const float DEFAULT_SPEED;
    static const int DEFAULT_DAMAGE;
};
