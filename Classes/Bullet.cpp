#include "Bullet.h"

USING_NS_CC;

// Protected constructor
Bullet::Bullet()
    : _isActive(true)
    , _damage(0)
    , _speed(0.0f)
    , _hitboxSize(Size::ZERO)
{
    CCLOG("Bullet created.");
}

// Destructor
Bullet::~Bullet()
{
    CCLOG("Bullet destroyed.");
}

// Initialization function
bool Bullet::init()
{
    if (!GameObject::init())
    {
        return false;
    }

    _isActive = true;
    this->scheduleUpdate();

    return true;
}

// Update function
void Bullet::update(float delta)
{
    if (!_isActive)
    {
        return;
    }

    // Perform movement
    updateMovement(delta);

    // Check if out of screen (simple check)
    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (getPositionX() > visibleSize.width + 50)
    {
        deactivate();
    }
}

// Check if active
bool Bullet::isActive() const
{
    return _isActive;
}

// Deactivate bullet
void Bullet::deactivate()
{
    _isActive = false;
    this->setVisible(false);
    // Do NOT removeFromParent here, as it might cause immediate destruction
    // while the pointer is still in GameWorld::_bullets list.
    // Removal will be handled in GameWorld::removeInactiveBullets.
}

// Get damage
int Bullet::getDamage() const
{
    return _damage;
}

// Default movement (can be overridden)
void Bullet::updateMovement(float delta)
{
    // Base class does nothing, subclasses implement movement
}
