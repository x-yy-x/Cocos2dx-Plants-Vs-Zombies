#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"

/**
 * @class Bullet
 * @brief Abstract base class for all projectiles fired by plants.
 * Inherits from GameObject and manages basic properties like damage, speed, and lifecycle.
 */
class Bullet : public GameObject
{
public:
    /**
     * @brief Standard Cocos2d-x initialization.
     * Sets up the update schedule and resets the active status.
     * @return true if initialization was successful.
     */
    virtual bool init() override;

    /**
     * @brief Frame-by-frame update logic.
     * Handles movement updates and boundary checks to remove off-screen bullets.
     * @param delta Time elapsed since the last frame in seconds.
     */
    virtual void update(float delta) override;

    /**
     * @brief Checks if the bullet is currently in an active state.
     * Used by the collision manager to determine if the projectile can still hit targets.
     * @return true if the bullet is active and valid.
     */
    bool isActive() const;

    /**
     * @brief Deactivates the bullet.
     * Marks the bullet as inactive and hides it from the scene for recycling or removal.
     */
    void deactivate();

    /**
     * @brief Retrieves the damage value of the bullet.
     * @return Integer representing the amount of health points to deduct from a zombie.
     */
    int getDamage() const;

protected:
    /**
     * @brief Private constructor to enforce controlled instantiation.
     * Initializes default values for speed, damage, and state.
     */
    Bullet();

    /**
     * @brief Virtual destructor for proper memory cleanup.
     * Ensures derived classes are safely destroyed when handled via base pointers.
     */
    virtual ~Bullet();

    /**
     * @brief Defines the trajectory of the bullet.
     * Intended to be overridden by subclasses to implement specific paths (e.g., lobbed, straight).
     * @param delta Time increment for calculation.
     */
    virtual void updateMovement(float delta);

    // ----------------------------------------------------
    // Member Variables
    // ----------------------------------------------------

    bool is_active;             // Flag indicating if the bullet is alive and interacting
    int damage;                 // Damage dealt to enemies upon impact
    float current_speed;        // Movement speed of the projectile (pixels per second)
    cocos2d::Size hitbox_size;  // Dimensions of the bounding box used for collision detection
};