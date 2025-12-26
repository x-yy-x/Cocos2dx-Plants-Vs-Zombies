#include "Sunflower.h"
#include "Sun.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string Sunflower::IMAGE_FILENAME = "Sunflower_spritesheet.png";
const cocos2d::Rect Sunflower::INITIAL_PIC_RECT = Rect(0.0f, 512.0f - 128.0f, 85.333f, 128.0f);
const cocos2d::Size Sunflower::OBJECT_SIZE = Size(85.333f, 128.0f);
const float Sunflower::SUN_PRODUCTION_INTERVAL = 12.0f;  // Produce sun every 24 seconds

// Protected constructor
Sunflower::Sunflower()
{
    CCLOG("Sunflower created.");
}

// ------------------------------------------------------------------------
// 1. Sunflower initialization
// ------------------------------------------------------------------------
bool Sunflower::init()
{
    return initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, SUN_PRODUCTION_INTERVAL);
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
Sunflower* Sunflower::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Sunflower>(globalPos);
}

// ------------------------------------------------------------------------
// 3. Sunflower animation
// ------------------------------------------------------------------------
void Sunflower::setAnimation()
{
    auto animation = initAnimate(IMAGE_FILENAME, 100.0f, 100.0f, 4, 6, 24, 0.07f);
    if (animation) {
        auto animate = Animate::create(animation);
        auto repeatAction = RepeatForever::create(animate);
        this->runAction(repeatAction);
    }
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void Sunflower::update(float delta)
{
    Plant::update(delta);
    // Plant::update handles cooldown logic
    // GameWorld will call produceSun() to check if sun is ready
}

// ------------------------------------------------------------------------
// 5. Produce sun
// ------------------------------------------------------------------------
std::vector<Sun*> Sunflower::produceSun()
{
    // Check if cooldown is finished
    if (_accumulatedTime >= _cooldownInterval)
    {
        // Reset cooldown
        _accumulatedTime = 0.0f;

        // Create sun at sunflower position
        Sun* sun = Sun::createFromSunflower(this->getPosition());
        CCLOG("Sunflower produced a sun!");
        return { sun };
    }

    return {};
}

