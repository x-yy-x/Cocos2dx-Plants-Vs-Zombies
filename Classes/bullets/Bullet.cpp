#include "Bullet.h"

USING_NS_CC;

/**
 * @brief Default constructor.
 * Initializes the projectile with default values and logs creation.
 */
Bullet::Bullet()
    : is_active(true)
    , damage(0)
    , current_speed(0.0f)
    , hitbox_size(Size::ZERO)
{
    CCLOG("Bullet created.");
}

/**
 * @brief Destructor.
 * Handles clean-up and logs destruction for debugging purposes.
 */
Bullet::~Bullet()
{
    CCLOG("Bullet destroyed.");
}

/**
 * @brief Initialization override.
 * Calls base GameObject initialization and schedules the per-frame update.
 * @return true if successfully initialized.
 */
bool Bullet::init()
{
    if (!GameObject::init())
    {
        return false;
    }

    is_active = true;
    this->scheduleUpdate();

    return true;
}

/**
 * @brief Main update loop.
 * Updates bullet position and performs a boundary check to deactivate
 * projectiles that have moved significantly off-screen.
 * @param delta Time elapsed since the last frame.
 */
void Bullet::update(float delta)
{
    if (!is_active)
    {
        return;
    }

    // Execute specialized movement logic defined by subclasses
    updateMovement(delta);

    // Visibility check: deactivate bullet if it passes the right screen edge
    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (getPositionX() > visibleSize.width + 50)
    {
        deactivate();
    }
}

/**
 * @brief Activity status getter.
 * @return true if the bullet is currently active in the game world.
 */
bool Bullet::isActive() const
{
    return is_active;
}

/**
 * @brief Deactivation logic.
 * Marks the bullet as inactive and hides it. Removal from the scene tree
 * is deferred to prevent pointer invalidation during container iteration.
 */
void Bullet::deactivate()
{
    is_active = false;
    this->setVisible(false);
    // Note: Manual removal is handled by the GameWorld manager to ensure
    // memory safety while iterating through bullet lists.
}

/**
 * @brief Damage value getter.
 * @return The integer damage value assigned to this projectile.
 */
int Bullet::getDamage() const
{
    return damage;
}

/**
 * @brief Trajectory update handler.
 * Provides a hook for subclasses to implement specific movement patterns.
 * @param delta Time elapsed since the last frame.
 */
void Bullet::updateMovement(float delta)
{
    // Implementation is provided by derived classes (e.g., Pea, Cabbage)
}