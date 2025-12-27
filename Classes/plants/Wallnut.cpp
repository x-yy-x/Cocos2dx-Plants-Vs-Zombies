#include "Wallnut.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string Wallnut::IMAGE_FILENAME = "wallnut_spritesheet.png";
const cocos2d::Rect Wallnut::INITIAL_PIC_RECT = Rect(0.0f, 512.0f - 128.0f, 85.333f, 128.0f);
const cocos2d::Size Wallnut::OBJECT_SIZE = Size(85.333f, 128.0f);

// Protected constructor
Wallnut::Wallnut()
{
    CCLOG("Wallnut created.");
}

// ------------------------------------------------------------------------
// 1. PeaShooter initialization
// ------------------------------------------------------------------------
bool Wallnut::init()
{
    if (!Plant::init())
    {
        return false;
    }

    if (!Sprite::initWithFile(IMAGE_FILENAME, INITIAL_PIC_RECT))
    {
        return false;
    }

    max_health = 1000;
    current_health = 1000;
    cooldown_interval = 1.5f;  // Attack once every 1.5 seconds
    accumulated_time = 0.0f;
    current_state = WallnutState::NORMAL;

    this->setAnimation();
    this->setCrackedAnimation();
    this->scheduleUpdate();
    this->runAction(normalAnimation);
    return true;
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
Wallnut* Wallnut::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Wallnut>(globalPos);
}

// ------------------------------------------------------------------------
// 3. PeaShooter animation
// ------------------------------------------------------------------------
void Wallnut::setAnimation()
{
    const float frameWidth = 100;
    const float frameHeight = 100;

    auto animation = initAnimate(IMAGE_FILENAME, frameWidth, frameHeight, 6, 6, 32, 0.07f);
    if (animation) {
        auto animate = Animate::create(animation);
        this->normalAnimation = RepeatForever::create(animate);
        normalAnimation->retain();
    }
}

void Wallnut::setCrackedAnimation()
{
    const float frameWidth = 100;
    const float frameHeight = 100;

    auto animation = initAnimate("wallnut_cracked_spritesheet.png", frameWidth, frameHeight, 6, 6, 32, 0.07f);
    if (animation) {
        auto animate = Animate::create(animation);
        this->crackedAnimation = RepeatForever::create(animate);
        crackedAnimation->retain();
    }
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void Wallnut::update(float delta)
{
    Plant::update(delta);

    WallnutState newState = (current_health >= 300)
        ? WallnutState::NORMAL
        : WallnutState::CRACKED;

    // Only switch animation when state changes
    if (newState != current_state)
    {
        current_state = newState;

        this->stopAllActions();

        if (current_state == WallnutState::NORMAL)
            this->runAction(normalAnimation);
        else
            this->runAction(crackedAnimation);
    }
}

// ------------------------------------------------------------------------
// 5. Wallnut does not attack
// ------------------------------------------------------------------------
std::vector<Bullet*> Wallnut::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    // Wallnut is a defensive plant, does not attack
    return std::vector<Bullet*>();
}

