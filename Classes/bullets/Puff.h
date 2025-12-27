#pragma once

#include "cocos2d.h"
#include "Bullet.h"

/**
 * @class Puff
 * @brief Represents a short-range projectile fired by the Puff-shroom.
 * Unlike standard bullets, the Puff has a limited travel distance defined by its lifetime.
 */
class Puff : public Bullet
{
public:
    /**
     * @brief Initializes the Puff bullet.
     * Sets up texture resources and initializes the lifetime counter.
     * @return true if initialization was successful.
     */
    virtual bool init() override;

    /**
     * @brief Static factory method to create a Puff at a specific position.
     * @param startPos The initial spawning coordinates.
     * @return A pointer to the created Puff instance.
     */
    static Puff* create(const cocos2d::Vec2& startPos);

    /**
     * @brief Standard Cocos2d-x macro for default instantiation.
     */
    CREATE_FUNC(Puff);

private:
    /**
     * @brief Private constructor using member initializer list.
     * Initializes the life_time counter to zero.
     */
    Puff();

    /**
     * @brief Updates movement and manages the bullet's lifespan.
     * The bullet will automatically deactivate once life_time exceeds MAX_LIFETIME.
     * @param delta Time elapsed since the last frame.
     */
    virtual void updateMovement(float delta) override;

    // ----------------------------------------------------
    // Static Configuration Constants
    // ----------------------------------------------------

    static const std::string IMAGE_FILENAME;    // Sprite resource path
    static const cocos2d::Rect INITIAL_PIC_RECT; // Texture source rectangle
    static const float DEFAULT_SPEED;            // Pixels per second
    static const int DEFAULT_DAMAGE;             // Health points deducted on hit
    static const float MAX_LIFETIME;             // Maximum duration before deactivation

    // ----------------------------------------------------
    // Member Variables
    // ----------------------------------------------------

    float life_time; // Accumulator for the bullet's current active duration
};