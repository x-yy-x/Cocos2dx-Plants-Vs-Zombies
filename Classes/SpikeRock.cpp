#include "SpikeRock.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string SpikeRock::IMAGE_FILENAME_FIRST = "spikerock_first_spritesheet.png";
const std::string SpikeRock::IMAGE_FILENAME_SECOND = "spikerock_second_spritesheet.png";
const std::string SpikeRock::IMAGE_FILENAME_THIRD = "spikerock_third_spritesheet.png";


const cocos2d::Rect SpikeRock::INITIAL_PIC_RECT = Rect(0.0f, 512.0f - 128.0f, 85.333f, 128.0f);


// Protected constructor
SpikeRock::SpikeRock()
{
    this->_currentState = COMPLETE;
    this->_currentHealth = 3000;
    CCLOG("SpikeRock created.");
}

// ------------------------------------------------------------------------
// 1. SpikeRock initialization
// ------------------------------------------------------------------------
bool SpikeRock::init()
{
    return initPlantWithSettings(IMAGE_FILENAME_FIRST, INITIAL_PIC_RECT, 3000, 1.5f);
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
SpikeRock* SpikeRock::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<SpikeRock>(globalPos);
}

// ------------------------------------------------------------------------
// 3. SpikeRock animation
// ------------------------------------------------------------------------
void SpikeRock::setAnimation()
{
    this->runAction(MoveBy::create(0.0001f, Vec2(0, -40)));
    
    auto animation = initAnimate(IMAGE_FILENAME_FIRST, 105.0f, 54.0f, 2, 4, 8, 0.105f);
    if (animation) {
        auto animate = Animate::create(animation);
        auto repeatAction = RepeatForever::create(animate);
        this->runAction(repeatAction);
    }
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void SpikeRock::update(float delta)
{
    Plant::update(delta);
    // Plant::update handles cooldown logic
    // GameWorld will call attack() when conditions are met
    SpikeRockState newState = COMPLETE;
    if (_currentHealth <= 2000 && _currentHealth > 1000)
        newState = DAMAGED;
    else if (_currentHealth <= 1000)
        newState = BROKEN;
    if (_currentState != newState) {
        _currentState = newState;
        this->stopAllActions();
        if (_currentState == DAMAGED) {
            auto animation = initAnimate(IMAGE_FILENAME_SECOND, 105.0f, 54.0f, 3, 3, 9, 0.105f);
            if (animation) {
                auto animate = Animate::create(animation);
                auto repeatAction = RepeatForever::create(animate);
                this->runAction(repeatAction);
            }
        } else {
            auto animation = initAnimate(IMAGE_FILENAME_THIRD, 105.0f, 54.0f, 3, 3, 9, 0.105f);
            if (animation) {
                auto animate = Animate::create(animation);
                auto repeatAction = RepeatForever::create(animate);
                this->runAction(repeatAction);
            }
        }
    }

}

// ------------------------------------------------------------------------
// 5. Check and attack logic (override from AttackingPlant)
// ------------------------------------------------------------------------
std::vector<Bullet*> SpikeRock::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    std::vector<Bullet*> empty; 

    _accumulatedTime += Director::getInstance()->getDeltaTime();

    if (_accumulatedTime < _cooldownInterval)
        return empty;
    _accumulatedTime = 0.0f;


    Rect spikeRect = this->getBoundingBox();
    spikeRect.origin.x += 45;
    spikeRect.size.width -= 90;

    for (auto zombie : allZombiesInRow[plantRow])
    {
        if (!zombie || zombie->isDead())
            continue;
        
        if (spikeRect.intersectsRect(zombie->getBoundingBox()))
        {
            auto z = dynamic_cast<Zomboni*>(zombie);
            if (z && !z->hasBeenAttackedBySpike()) {
                z->setSpecialDeath();
                this->takeDamage(1000);
            }
            else {
                cocos2d::AudioEngine::play2d("bullet_hit.mp3");
                zombie->takeDamage(20);
            }
            
        }
    }
    return empty;
}
