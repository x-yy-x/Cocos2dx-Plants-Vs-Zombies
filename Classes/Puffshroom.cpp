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
    // 初始化植物基本属性
    if (!initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, ATTACK_COOLDOWN))
    {
        return false;
    }
    // 状态初始化交给 Mushroom 在 update 第一帧处理
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

    // 使用 Mushroom 基类逻辑检查环境变化
    bool envChanged = checkDayNightChange();

    // 如果环境变化，触发响应
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

    // 复用 Mushroom::loadAnimation，传入之前的裁剪参数
    if (_activityState == ActivityState::SLEEPING)
    {
        this->setTexture("puffshroom/sleep/1 (1).png");
        // sleep动画: 17帧, 不缩放, 裁剪宽度减去33
        animation = loadAnimation("puffshroom/sleep", 17, 1.0f,
            OBJECT_SIZE.width, OBJECT_SIZE.height, // 原始尺寸
            0.0f, 0.0f, OBJECT_SIZE.width - 33.0f, OBJECT_SIZE.height); // 裁剪参数
    }
    else
    {
        this->setTexture("puffshroom/init/1 (1).png");
        // active动画: 14帧, Y偏移7, 裁剪宽度减去33
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
    // 白天睡觉不攻击
    if (isDaytime())
    {
        return {};
    }

    // 复用之前的攻击逻辑
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