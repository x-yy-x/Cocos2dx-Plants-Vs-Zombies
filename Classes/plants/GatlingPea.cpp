#include "GatlingPea.h"
#include "Bullet.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string GatlingPea::IMAGE_FILENAME = "gatlingpea_spritesheet.png";
const cocos2d::Rect GatlingPea::INITIAL_PIC_RECT = Rect(0.0f, 512.0f - 128.0f, 85.333f, 128.0f);
const cocos2d::Size GatlingPea::OBJECT_SIZE = Size(85.333f, 128.0f);

// Protected constructor
GatlingPea::GatlingPea()
{
    CCLOG("GatlingPea created.");
}

// ------------------------------------------------------------------------
// 1. GatlingPea initialization
// ------------------------------------------------------------------------
bool GatlingPea::init()
{
    return initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 100, 1.5f);
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
GatlingPea* GatlingPea::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<GatlingPea>(globalPos);
}

// ------------------------------------------------------------------------
// 3. GatlingPea animation
// ------------------------------------------------------------------------
void GatlingPea::setAnimation()
{
    this->setScale(0.2f);
    
    auto animation = initAnimate(IMAGE_FILENAME, 530.0f, 512.0f, 5, 5, 25, 0.07f);
    if (animation) {
        auto animate = Animate::create(animation);
        auto repeatAction = RepeatForever::create(animate);
        this->runAction(repeatAction);
    }
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void GatlingPea::update(float delta)
{
    Plant::update(delta);
    // Plant::update handles cooldown logic
    // GameWorld will call produceSun() to check if sun is ready
}


std::vector<Bullet*> GatlingPea::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    std::vector<Bullet*> bullets;

    // Check if any zombie is in range in current row and cooldown is ready
    if (!isZombieInRange(allZombiesInRow[plantRow]) || accumulated_time < cooldown_interval)
    {
        return bullets;
    }

    accumulated_time = 0.0f; // Reset cooldown

    std::vector<Pea*> pea(4);
    std::vector<Vec2>spawnPos(4);
    for (int i = 0; i < 4; ++i) {
        spawnPos[i] = this->getPosition() + Vec2(30.0f, 20.0f) + static_cast<float>(i) * Vec2(20.0f, 0.0f);
        pea[i] = Pea::create(spawnPos[i]);
        if (pea[i])
            bullets.push_back(pea[i]);
    }

    if (!bullets.empty())
    {
        CCLOG("GatlingPea fired %d peas", static_cast<int>(bullets.size()));
    }

    return bullets;
}

