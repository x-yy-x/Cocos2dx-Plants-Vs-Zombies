#include "Puffshroom.h"
#include "Puff.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

const std::string Puffshroom::IMAGE_FILENAME = "puffshroom/sleep/1 (1).png";
const cocos2d::Rect Puffshroom::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Puffshroom::OBJECT_SIZE = Size(85.333f, 78.0f);
const float Puffshroom::ATTACK_COOLDOWN = 1.5f;
const int Puffshroom::DETECTION_RANGE = 3;

Puffshroom::Puffshroom()
    : Plant()
    , AttackingPlant()
    , Mushroom()
{
    CCLOG("Puffshroom created.");
}

bool Puffshroom::init()
{
    // Initialize basic plant properties
    if (!initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, ATTACK_COOLDOWN))
    {
        return false;
    }
    // State initialization is handled by Mushroom in the first update frame
    return true;
}

Puffshroom* Puffshroom::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Puffshroom>(globalPos);
}

void Puffshroom::update(float delta)
{
    Plant::update(delta);

    if (!this->getParent()) return;

    // Use Mushroom base class logic to check for environment changes
    bool envChanged = checkDayNightChange();

    // If environment changes, trigger response
    if (envChanged)
    {
        if (_isNightMode) wakeUp();
        else sleep();
    }
}

void Puffshroom::wakeUp()
{
    if (_activityState != ActivityState::ACTIVE)
    {
        _activityState = ActivityState::ACTIVE;
        setAnimation();
    }
}

void Puffshroom::sleep()
{
    if (_activityState != ActivityState::SLEEPING)
    {
        _activityState = ActivityState::SLEEPING;
        setAnimation();
    }
}

void Puffshroom::setAnimation()
{
    this->stopAllActions();
    _currentAnimation = nullptr;

    Animation* animation = nullptr;

    // Reuse Mushroom::loadAnimation with previous cropping parameters
    if (_activityState == ActivityState::SLEEPING)
    {
        this->setTexture("puffshroom/sleep/1 (1).png");
        // Sleep animation: 17 frames, no scaling, crop width minus 33
        animation = loadAnimation("puffshroom/sleep", 17, 1.0f,
            OBJECT_SIZE.width, OBJECT_SIZE.height, // 原始尺寸
            0.0f, 0.0f, OBJECT_SIZE.width - 33.0f, OBJECT_SIZE.height); // 裁剪参数
    }
    else
    {
        this->setTexture("puffshroom/init/1 (1).png");
        // Active animation: 14 frames, Y offset 7, crop width minus 33
        animation = loadAnimation("puffshroom/init", 14, 1.0f,
            OBJECT_SIZE.width, OBJECT_SIZE.height, // 原始尺寸
            0.0f, 7.0f, OBJECT_SIZE.width - 33.0f, OBJECT_SIZE.height); // 裁剪参数
    }

    if (animation)
    {
        auto animate = Animate::create(animation);
        auto repeat = RepeatForever::create(animate);
        _currentAnimation = repeat;
        this->runAction(repeat);
    }
}

std::vector<Bullet*> Puffshroom::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    // Don't attack when sleeping during day
    if (isDaytime())
    {
        return {};
    }

    // Reuse previous attack logic
    float plantX = this->getPositionX();
    float maxRange = plantX + (CELLSIZE.width * DETECTION_RANGE);
    bool zombieDetected = false;

    auto& zombiesInThisRow = allZombiesInRow[plantRow];
    for (auto zombie : zombiesInThisRow)
    {
        if (zombie && !zombie->isDead())
        {
            float zombieX = zombie->getPositionX();
            if (zombieX > plantX && zombieX <= maxRange)
            {
                zombieDetected = true;
                break;
            }
        }
    }

    if (!zombieDetected) return {};

    if (_accumulatedTime >= _cooldownInterval)
    {
        _accumulatedTime = 0.0f;
        Vec2 spawnPos = this->getPosition() + Vec2(10.0f, -5.0f);

        Puff* puff = Puff::create(spawnPos);
        if (puff)
        {
            cocos2d::AudioEngine::play2d("puff.mp3", false, 1.0f);
            return { puff };
        }
    }

    return {};
}