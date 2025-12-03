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
    : _isDead(false)
    , _maxHealth(100)
    , _currentHealth(100)
    , _cooldownInterval(1.0f)
    , _accumulatedTime(0.0f)
    , _plantPos(Vec2::ZERO)
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
    _isDead = false;
    _accumulatedTime = 0.0f;

    return true;
}

// Update every frame
void Plant::update(float delta)
{
    // Skip if already dead
    if (_isDead)
    {
        return;
    }

    // Accumulate time
    _accumulatedTime += delta;
}

// Check if dead
bool Plant::isDead() const
{
    return _isDead;
}

// Take damage
void Plant::takeDamage(int damage)
{
    if (_isDead)
    {
        return;
    }

    _currentHealth -= damage;
    CCLOG("Plant took %d damage, remaining health: %d", damage, _currentHealth);

    if (_currentHealth <= 0)
    {
        _currentHealth = 0;
        _isDead = true;
        CCLOG("Plant is dead.");
        // Can add death effects here, remove from scene, etc.
    }
}

// Virtual attack method
Bullet* Plant::attack()
{
    // Default implementation does nothing
    return nullptr;
}

// Set plant position
void Plant::setPlantPosition(const cocos2d::Vec2& pos)
{
    _plantPos = pos;
    this->setPosition(pos);
}

// Detect and handle enemies within range (not implemented yet)
void Plant::encounterEnemy(const std::vector<Zombie*>& zombies)
{
    // Will implement enemy detection and attack logic here later
    // For example: find enemies within range and execute attack
}

// Set animation (to be implemented by subclasses)
void Plant::setAnimation()
{
    // Subclasses will implement their specific animations
    CCLOG("Plant::setAnimation() called (to be implemented by subclass).");
}
