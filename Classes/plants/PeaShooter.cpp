#include "PeaShooter.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string PeaShooter::IMAGE_FILENAME = "peashooter_spritesheet.png";
const cocos2d::Rect PeaShooter::INITIAL_PIC_RECT = Rect(0.0f, 512.0f - 128.0f, 85.333f, 128.0f);
const cocos2d::Size PeaShooter::OBJECT_SIZE = Size(85.333f, 128.0f);
const float PeaShooter::ATTACK_RANGE = 2000.0f;

// Protected constructor
PeaShooter::PeaShooter()
{
    CCLOG("PeaShooter created.");
}

// ------------------------------------------------------------------------
// 1. PeaShooter initialization
// ------------------------------------------------------------------------
bool PeaShooter::init()
{
    return initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, 1.5f);
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
PeaShooter* PeaShooter::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<PeaShooter>(globalPos);
}

// ------------------------------------------------------------------------
// 3. PeaShooter animation
// ------------------------------------------------------------------------
void PeaShooter::setAnimation()
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
void PeaShooter::update(float delta)
{
    Plant::update(delta);
    // Plant::update handles cooldown logic
    // GameWorld will call attack() when conditions are met
}

// ------------------------------------------------------------------------
// 5. Check and attack logic (override from AttackingPlant)
// ------------------------------------------------------------------------
std::vector<Bullet*> PeaShooter::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    std::vector<Bullet*> bullets;

    // Check if any zombie is in range in current row (to the right)
    if (!isZombieInRange(allZombiesInRow[plantRow]))
    {
        return bullets;
    }

    // Check if cooldown is ready
    if (accumulated_time >= cooldown_interval)
    {
        accumulated_time = 0.0f; // Reset cooldown
        
        // Create a new Pea bullet at the plant's position
        // Offset slightly to spawn from the "mouth"
        Vec2 spawnPos = this->getPosition() + Vec2(30.0f, 20.0f); 
        
        Pea* pea = Pea::create(spawnPos);
        if (pea)
        {
             bullets.push_back(pea);
             CCLOG("PeaShooter fired a pea at %f, %f", spawnPos.x, spawnPos.y);
        }
    }
    return bullets;
}
