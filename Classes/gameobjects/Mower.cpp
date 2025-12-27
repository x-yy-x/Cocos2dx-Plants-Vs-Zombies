#include "Mower.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ---------------------------------------------------------
// Factory Method
// ---------------------------------------------------------
Mower* Mower::create()
{
    Mower* m = new (std::nothrow) Mower();
    if (m && m->init())
    {
        m->autorelease();
        return m;
    }
    CC_SAFE_DELETE(m);
    return nullptr;
}

// ---------------------------------------------------------
// Initialization
// ---------------------------------------------------------
bool Mower::init()
{
    // Load the visual asset for the lawnmower
    if (!Sprite::initWithFile("mower.png"))
    {
        return false;
    }

    moving = false;
    return true;
}

// ---------------------------------------------------------
// Logic Implementation
// ---------------------------------------------------------
void Mower::start()
{
    // Prevent multiple triggers if already active
    if (moving) return;

    moving = true;

    // Play the activation sound effect
    cocos2d::AudioEngine::play2d("Lawnmower.ogg", false, 1.0f);

    // Calculate movement parameters
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // Distance to travel: visible width plus a buffer for off-screen cleanup
    float targetDistance = visibleSize.width + 300.0f;
    float movementSpeed = 900.0f; // Velocity in pixels per second
    float duration = targetDistance / movementSpeed;

    // Execute the linear movement action
    auto moveAction = MoveBy::create(duration, Vec2(targetDistance, 0));
    this->runAction(moveAction);

    CCLOG("Mower activated and moving across the row.");
}