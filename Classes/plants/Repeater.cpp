#include "Repeater.h"
#include "Pea.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Repeater::IMAGE_FILENAME = "repeater_spritesheet.png";
const cocos2d::Rect Repeater::INITIAL_PIC_RECT = Rect(0.0f, 0.0f, 85.333f, 85.333f);

// Protected constructor
Repeater::Repeater()
{
    CCLOG("Repeater created.");
}

// ------------------------------------------------------------------------
// 1. Repeater initialization (uses different image than PeaShooter)
// ------------------------------------------------------------------------
bool Repeater::init()
{
    return initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, 1.5f);
}

// ------------------------------------------------------------------------
// 2. Repeater animation (4 rows x 6 columns)
// ------------------------------------------------------------------------
void Repeater::setAnimation()
{
    auto animation = initAnimate(IMAGE_FILENAME, 100.0f, 100.0f, 4, 6, 24, 0.07f);
    if (animation) {
        auto animate = Animate::create(animation);
        auto repeatAction = RepeatForever::create(animate);
        this->runAction(repeatAction);
    }
}

// ------------------------------------------------------------------------
// 3. Check and attack logic - shoots TWO peas simultaneously
// ------------------------------------------------------------------------
std::vector<Bullet*> Repeater::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    std::vector<Bullet*> bullets;

    // Check if any zombie is in range in current row and cooldown is ready
    if (!isZombieInRange(allZombiesInRow[plantRow]) || _accumulatedTime < _cooldownInterval)
    {
        return bullets;
    }

    _accumulatedTime = 0.0f; // Reset cooldown
    
    // Create first pea
    Vec2 spawnPos1 = this->getPosition() + Vec2(30.0f, 20.0f);
    Pea* firstPea = Pea::create(spawnPos1);
    
    // Create second pea with horizontal offset (same height)
    Vec2 spawnPos2 = this->getPosition() + Vec2(70.0f, 20.0f);
    Pea* secondPea = Pea::create(spawnPos2);
    
    if (firstPea)
    {
        bullets.push_back(firstPea);
    }
    
    if (secondPea)
    {
        bullets.push_back(secondPea);
    }
    
    if (!bullets.empty())
    {
        CCLOG("Repeater fired %d peas", static_cast<int>(bullets.size()));
    }
    
    return bullets;
}

