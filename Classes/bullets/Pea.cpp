#include "Pea.h"

USING_NS_CC;

// ----------------------------------------------------
// Static Configuration
// ----------------------------------------------------
const std::string Pea::IMAGE_FILENAME = "pea.png";
const cocos2d::Rect Pea::INITIAL_PIC_RECT = Rect::ZERO;
const float Pea::DEFAULT_SPEED = 400.0f;
const int Pea::DEFAULT_DAMAGE = 20;

/**
 * @brief Constructor for Pea using member initializer list.
 * Note: Base properties like damage and speed are initialized here to
 * ensure the object starts in a valid state.
 */
Pea::Pea()
{
    CCLOG("Pea created.");
}

/**
 * @brief Factory method for creating a Pea at a specific position.
 * @param startPos Initial world coordinates.
 * @return Autoreleased Pea instance.
 */
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

/**
 * @brief Initialization override.
 * Loads texture resources and syncs hitbox size with the sprite content size.
 * @return true if initialization was successful.
 */
bool Pea::init()
{
    // Initialize inherited Bullet properties
    if (!Bullet::init())
    {
        return false;
    }

    // Load visual asset
    if (!Sprite::initWithFile(IMAGE_FILENAME))
    {
        CCLOG("Failed to load pea image: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    // Assign gameplay values defined in static constants
    damage = DEFAULT_DAMAGE;
    current_speed = DEFAULT_SPEED;
    hitbox_size = this->getContentSize();

    return true;
}

/**
 * @brief Implements linear horizontal movement.
 * Moves the pea forward based on current_speed and frame time.
 * @param delta Time elapsed since the last frame.
 */
void Pea::updateMovement(float delta)
{
    // Standard rightward translation
    float newX = getPositionX() + current_speed * delta;
    setPositionX(newX);
}