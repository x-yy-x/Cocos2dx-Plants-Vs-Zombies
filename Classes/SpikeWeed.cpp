#include "SpikeWeed.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string SpikeWeed::IMAGE_FILENAME = "spikeweed_spritesheet.png";
const cocos2d::Rect SpikeWeed::INITIAL_PIC_RECT = Rect(0.0f, 512.0f - 128.0f, 85.333f, 128.0f);


// Protected constructor
SpikeWeed::SpikeWeed()
{
    CCLOG("PeaShooter created.");
}

// ------------------------------------------------------------------------
// 1. SpikeWeed initialization
// ------------------------------------------------------------------------
bool SpikeWeed::init()
{
    return initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 300, 1.5f);
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
SpikeWeed* SpikeWeed::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<SpikeWeed>(globalPos);
}

// ------------------------------------------------------------------------
// 3. SpikeWeed animation
// ------------------------------------------------------------------------
void SpikeWeed::setAnimation()
{
    this->runAction(MoveBy::create(0.0001f, Vec2(0, -40)));
    createAndRunAnimation(IMAGE_FILENAME, 106.2, 43.5, 4, 5,0.07,19);
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void SpikeWeed::update(float delta)
{
    Plant::update(delta);
    // Plant::update handles cooldown logic
    // GameWorld will call attack() when conditions are met
}

// ------------------------------------------------------------------------
// 5. Check and attack logic (override from AttackingPlant)
// ------------------------------------------------------------------------
std::vector<Bullet*> SpikeWeed::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    std::vector<Bullet*> empty; // 地刺不发子弹

    _accumulatedTime += Director::getInstance()->getDeltaTime();

    if (_accumulatedTime < _cooldownInterval)
        return empty;
    _accumulatedTime = 0.0f;

    // 地刺所在格子的碰撞框
    Rect spikeRect = this->getBoundingBox();
    spikeRect.origin.x += 45;
    spikeRect.size.width -= 90;

    for (auto zombie : allZombiesInRow[plantRow])
    {
        if (!zombie || zombie->isDead())
            continue;
        // 僵尸是否踩在地刺上
        if (spikeRect.intersectsRect(zombie->getBoundingBox()))
        {
            auto z = dynamic_cast<Zomboni*>(zombie);
            if (z) {
                z->setSpecialDeath();
                this->takeDamage(1000);
            }
            else {
                zombie->takeDamage(10);
            }
            
        }
    }
    return empty;
}