#include "Plant.h"
#include "Zombie.h"
#include "Bullet.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const float Plant::ATTACK_RANGE = 1000.0f;  // Default attack range

// Protected constructor
Plant::Plant()
    : is_dead(false)
    , max_health(100)
    , current_health(100)
    , cooldown_interval(1.0f)
    , accumulated_time(0.0f)
    , plant_pos(Vec2::ZERO)
{
    CCLOG("Plant created.");
}

// Destructor
Plant::~Plant()
{
    CCLOG("Plant destroyed.");
}

// Initialization function
bool Plant::init()
{
    // Call parent class initialization
    if (!GameObject::init())
    {
        return false;
    }

    // Plant-specific initialization logic
    is_dead = false;
    accumulated_time = 0.0f;

    return true;
}

// Update every frame
void Plant::update(float delta)
{
    // Skip if already dead
    if (is_dead)
    {
        return;
    }

    // Accumulate time
    accumulated_time += delta;
}

// Check if dead
bool Plant::isDead() const
{
    return is_dead;
}

// Take damage
void Plant::takeDamage(float damage)
{
    if (is_dead)
    {
        return;
    }

    current_health -= static_cast<int>(damage);
    CCLOG("Plant took %d damage, remaining health: %d", static_cast<int>(damage), current_health);

    if (current_health <= 0)
    {
        current_health = 0;
        is_dead = true;
        CCLOG("Plant is dead.");
        // Can add death effects here, remove from scene, etc.
    }
}

// Set plant position
void Plant::setPlantPosition(const cocos2d::Vec2& pos)
{
    plant_pos = pos;
    this->setPosition(pos);
}

// Helper method to initialize plant with common settings
bool Plant::initPlantWithSettings(const std::string& imageFile, 
                                  const Rect& initialRect,
                                  int maxHealth, 
                                  float cooldown)
{
    if (!Plant::init())
    {
        return false;
    }

    if (!Sprite::initWithFile(imageFile, initialRect))
    {
        CCLOG("Failed to load plant image: %s", imageFile.c_str());
        return false;
    }

    max_health = maxHealth;
    current_health = maxHealth;
    cooldown_interval = cooldown;
    accumulated_time = 0.0f;

    this->setAnimation();
    this->scheduleUpdate();

    return true;
}

// Set animation (to be implemented by subclasses)
void Plant::setAnimation()
{
    // Subclasses will implement their specific animations
    CCLOG("Plant::setAnimation() called (to be implemented by subclass).");
}
