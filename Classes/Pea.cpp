#include "Pea.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Pea::IMAGE_FILENAME = "pea.png";
const cocos2d::Rect Pea::INITIAL_PIC_RECT = Rect::ZERO;
// TODO: Adjust default speed here
const float Pea::DEFAULT_SPEED = 400.0f; 
// TODO: Adjust default damage here
const int Pea::DEFAULT_DAMAGE = 20;

// Protected constructor
Pea::Pea()
{
    CCLOG("Pea created.");
}

// Initialization function
bool Pea::init()
{
    // Initialize parent Bullet
    if (!Bullet::init())
    {
        return false;
    }

    // Initialize sprite with image
    if (!Sprite::initWithFile(IMAGE_FILENAME))
    {
        CCLOG("Failed to load pea image: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    // Set default values
    _damage = DEFAULT_DAMAGE;
    _currentSpeed = DEFAULT_SPEED;
    _hitboxSize = this->getContentSize();

    return true;
}

// Static create method with position
Pea* Pea::create(const Vec2& startPos)
{
    Pea* pRet = new(std::nothrow) Pea();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        pRet->setPosition(startPos);
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
void Pea::updateMovement(float delta)
{
    // Move forward (to the right)
    float newX = getPositionX() + _currentSpeed * delta;
    setPositionX(newX);
}
