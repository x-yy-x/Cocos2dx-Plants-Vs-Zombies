#pragma once

#include "cocos2d.h"
#include "Bullet.h"

/**
 * @brief Puff bullet class, inherits from Bullet.
 * Fired by Puffshroom. Has limited lifetime.
 */
    class Puff : public Bullet
{
public:
    /**
     * @brief Initialization function
     */
    virtual bool init() override;

    // Static create method with position
    static Puff* create(const cocos2d::Vec2& startPos);

    // Implement standard CREATE_FUNC for consistency
    CREATE_FUNC(Puff);

protected:
    // Protected constructor
    Puff();

    /**
     * @brief Update movement logic (move forward with lifetime limit)
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
    static const float MAX_LIFETIME;  // Maximum lifetime in seconds (moves about 3 grid cells)

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    float _lifetime;  // Current lifetime counter
};