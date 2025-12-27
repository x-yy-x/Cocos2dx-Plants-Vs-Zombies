#pragma once

#include "cocos2d.h"
#include "Bullet.h"

/**
 * @class Pea
 * @brief Specific bullet type representing a Pea, fired by the PeaShooter plant.
 * Inherits from the Bullet base class and implements linear forward movement.
 */
class Pea : public Bullet
{
public:
    /**
     * @brief Initializes the Pea bullet.
     * Sets the sprite frame, movement speed, and damage values based on constants.
     * @return true if the Pea was successfully initialized.
     */
    virtual bool init() override;

    /**
     * @brief Static factory method to create a Pea at a specific location.
     * @param startPos The initial world coordinates for the bullet.
     * @return A pointer to the created Pea instance, or nullptr on failure.
     */
    static Pea* create(const cocos2d::Vec2& startPos);

    /**
     * @brief Standard Cocos2d-x macro to generate a default create() method.
     */
    CREATE_FUNC(Pea);

private:
    /**
     * @brief Private constructor.
     * Ensures instances are created via the static create methods.
     */
    Pea();

    /**
     * @brief Specialized movement logic for the Pea.
     * Implements standard horizontal movement along the X-axis.
     * @param delta Time elapsed since the last frame.
     */
    virtual void updateMovement(float delta) override;

    // ----------------------------------------------------
    // Static Configuration Constants
    // ----------------------------------------------------

    static const std::string IMAGE_FILENAME;    // Resource path for the pea sprite
    static const cocos2d::Rect INITIAL_PIC_RECT; // Texture rectangle for sprite clipping
    static const float DEFAULT_SPEED;            // Constant horizontal velocity
    static const int DEFAULT_DAMAGE;             // Standard damage dealt to zombies
};