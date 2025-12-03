#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"

/**
 * @brief Bullet base class, inherits from GameObject.
 * Represents a projectile fired by a plant.
 * Cannot be instantiated directly (abstract base class).
 */
class Bullet : public GameObject
{
public:
    /**
     * @brief Initialization function
     */
    virtual bool init() override;

    /**
     * @brief Update function called every frame
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Check if bullet is active
     * @return true if active, false otherwise
     */
    bool isActive() const;

    /**
     * @brief Deactivate the bullet (e.g. after hitting a target)
     */
    void deactivate();

    /**
     * @brief Get damage value
     * @return Damage value
     */
    int getDamage() const;

protected:
    // Protected constructor to prevent direct instantiation
    Bullet();

    // Virtual destructor
    virtual ~Bullet();

    /**
     * @brief Default movement behavior (to be implemented by subclasses if needed)
     * @param delta Time delta
     */
    virtual void updateMovement(float delta);

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    bool _isActive;           // Is bullet active
    int _damage;              // Damage value
    float _speed;             // Movement speed
    cocos2d::Size _hitboxSize; // Collision box size
};
