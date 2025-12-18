#include "Puff.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Puff::IMAGE_FILENAME = "puff.png";
const cocos2d::Rect Puff::INITIAL_PIC_RECT = Rect::ZERO;
// TODO: Adjust default speed here
const float Puff::DEFAULT_SPEED = 300.0f;
// TODO: Adjust default damage here
const int Puff::DEFAULT_DAMAGE = 20;
// Maximum lifetime: enough to travel about 3 grid cells (3 * 130 = 390 pixels)
// At speed 300, time = distance/speed = 390/300 = 1.3 seconds
const float Puff::MAX_LIFETIME = 1.3f;

// Protected constructor
Puff::Puff()
{
    CCLOG("Puff created.");
}

// Initialization function
bool Puff::init()
{
    // Initialize parent Bullet
    if (!Bullet::init())
    {
        return false;
    }

    // Initialize sprite with image
    if (!Sprite::initWithFile(IMAGE_FILENAME))
    {
        CCLOG("Failed to load puff image: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    // Set default values
    _damage = DEFAULT_DAMAGE;
    _speed = DEFAULT_SPEED;
    _hitboxSize = this->getContentSize();
    _lifetime = 0.0f;

    return true;
}

// Static create method with position
Puff* Puff::create(const Vec2& startPos)
{
    Puff* pRet = new(std::nothrow) Puff();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        pRet->setPosition(startPos);
		pRet->setScale(0.5f); // Scale down if needed
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

// Update movement logic
void Puff::updateMovement(float delta)
{
    // Update lifetime
    _lifetime += delta;

    // Check if lifetime exceeded
    if (_lifetime >= MAX_LIFETIME)
    {
        // Deactivate bullet when lifetime is exceeded
        deactivate();
        return;
    }

    // Move forward (to the right)
    float newX = getPositionX() + _speed * delta;
    setPositionX(newX);
}
