#include "Puff.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------

const std::string Puff::IMAGE_FILENAME = "puff.png";
const cocos2d::Rect Puff::INITIAL_PIC_RECT = Rect::ZERO;

// Constant horizontal velocity
const float Puff::DEFAULT_SPEED = 300.0f;

// Damage dealt upon impact
const int Puff::DEFAULT_DAMAGE = 20;

/** * Maximum lifetime: calibrated to cover approximately 3 grid cells.
 * Calculated as distance (390px) / speed (300px/s) = 1.3s.
 */
const float Puff::MAX_LIFETIME = 1.3f;

/**
 * @brief Constructor using member initializer list.
 * Initializes life_time to ensure a clean start for the counter.
 */
Puff::Puff()
    : life_time(0.0f)
{
    CCLOG("Puff created.");
}

/**
 * @brief Initialization override.
 * Configures the sprite, base bullet attributes, and hitbox.
 * @return true if successfully initialized.
 */
bool Puff::init()
{
    // Initialize base Bullet class components
    if (!Bullet::init())
    {
        return false;
    }

    // Load the texture for the puff projectile
    if (!Sprite::initWithFile(IMAGE_FILENAME))
    {
        CCLOG("Failed to load puff image: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    // Initialize gameplay stats
    damage = DEFAULT_DAMAGE;
    current_speed = DEFAULT_SPEED;
    hitbox_size = this->getContentSize();
    life_time = 0.0f;

    return true;
}

/**
 * @brief Factory method for creating an autoreleased Puff instance.
 * @param startPos Starting world coordinates.
 * @return A configured and positioned Puff instance.
 */
Puff* Puff::create(const Vec2& startPos)
{
    Puff* pRet = new(std::nothrow) Puff();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        pRet->setPosition(startPos);

        // Puff bullets are visually smaller than standard peas
        pRet->setScale(0.5f);
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
 * @brief Movement and lifespan management.
 * Translates the bullet horizontally and deactivates it once the range limit is reached.
 * @param delta Time increment for this frame.
 */
void Puff::updateMovement(float delta)
{
    // Increment the active duration
    life_time += delta;

    // Check if the projectile has exceeded its travel range/time
    if (life_time >= MAX_LIFETIME)
    {
        deactivate();
        return;
    }

    // Perform linear horizontal translation
    float newX = getPositionX() + current_speed * delta;
    setPositionX(newX);
}