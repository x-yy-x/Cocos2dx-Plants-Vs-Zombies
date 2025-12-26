#include "TwinSunflower.h"
#include "Sun.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string TwinSunflower::IMAGE_FILENAME = "twinsunflower_spritesheet.png";
const cocos2d::Rect TwinSunflower::INITIAL_PIC_RECT = Rect(0.0f, 512.0f - 128.0f, 85.333f, 128.0f);
const cocos2d::Size TwinSunflower::OBJECT_SIZE = Size(85.333f, 128.0f);
const float TwinSunflower::SUN_PRODUCTION_INTERVAL = 15.0f;  // Produce sun every 24 seconds

// Protected constructor
TwinSunflower::TwinSunflower()
{
    CCLOG("TwinSunflower created.");
}

// ------------------------------------------------------------------------
// 1. TwinSunflower initialization
// ------------------------------------------------------------------------
bool TwinSunflower::init()
{
    return initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, SUN_PRODUCTION_INTERVAL);
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
TwinSunflower* TwinSunflower::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<TwinSunflower>(globalPos);
}

// ------------------------------------------------------------------------
// 3. TwinSunflower animation
// ------------------------------------------------------------------------
void TwinSunflower::setAnimation()
{
    auto animation = initAnimate(IMAGE_FILENAME, 103.75f, 105.0f, 4, 5, 20, 0.07f);
    if (animation) {
        auto animate = Animate::create(animation);
        auto repeatAction = RepeatForever::create(animate);
        this->runAction(repeatAction);
    }
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void TwinSunflower::update(float delta)
{
    Plant::update(delta);
    // Plant::update handles cooldown logic
    // GameWorld will call produceSun() to check if sun is ready
}

// ------------------------------------------------------------------------
// 5. Produce sun
// ------------------------------------------------------------------------
std::vector<Sun*> TwinSunflower::produceSun()
{
    // Check if cooldown is finished
    if (_accumulatedTime >= _cooldownInterval)
    {
        // Reset cooldown
        _accumulatedTime = 0.0f;

        // Create sun at sunflower position
        Sun* sun1 = Sun::createFromSunflower(this->getPosition() + Vec2(20, 0));
        Sun* sun2 = Sun::createFromSunflower(this->getPosition() - Vec2(20, 0));

        CCLOG("TwinSunflower produced two sun!");
        return { sun1,sun2 };
    }

    return {};
}