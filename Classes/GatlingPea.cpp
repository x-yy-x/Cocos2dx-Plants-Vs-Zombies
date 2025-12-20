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
    this->setScale(0.2);
    createAndRunAnimation(IMAGE_FILENAME, 530, 512, 5, 5);
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


bool GatlingPea::canUpgrade(Plant* baseplant) const
{
    return dynamic_cast<Repeater*>(baseplant) != nullptr;
}

GatlingPea* GatlingPea::upgrade(Plant* basePlant)
{
    auto pos = basePlant->getPosition();

    basePlant->takeDamage(10000.0f);

    return createPlantAtPosition<GatlingPea>(pos);
}

std::vector<Bullet*> GatlingPea::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    std::vector<Bullet*> bullets;

    // Check if any zombie is in range in current row and cooldown is ready
    if (!isZombieInRange(allZombiesInRow[plantRow]) || _accumulatedTime < _cooldownInterval)
    {
        return bullets;
    }

    _accumulatedTime = 0.0f; // Reset cooldown

    std::vector<Pea*> pea(4);
    std::vector<Vec2>spawnPos(4);
    for (int i = 0; i < 4; ++i) {
        spawnPos[i] = this->getPosition() + Vec2(30.0f, 20.0f) + i * Vec2(20.0f, 0);
        pea[i] = Pea::create(spawnPos[i]);
        if (pea[i])
            bullets.push_back(pea[i]);
    }

    if (!bullets.empty())
    {
        CCLOG("GatlingPea fired %d peas", (int)bullets.size());
    }

    return bullets;
}

