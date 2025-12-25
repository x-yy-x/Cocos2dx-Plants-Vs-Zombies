#include "Sunshroom.h"
#include "Sun.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// 静态常量定义
const std::string Sunshroom::IMAGE_FILENAME = "sunshroom/init/1 (1).png";
const cocos2d::Rect Sunshroom::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Sunshroom::OBJECT_SIZE = Size(85.333f, 78.0f);
const float Sunshroom::SUN_PRODUCTION_INTERVAL = 15.0f;
const float Sunshroom::GROWTH_TIME = 25.0f;
const float Sunshroom::SMALL_SCALE = 0.7f;
const float Sunshroom::GROWN_SCALE = 1.0f;
const int Sunshroom::SMALL_SUN_VALUE = 15;
const int Sunshroom::GROWN_SUN_VALUE = 25;

// 构造函数：初始化所有父类
Sunshroom::Sunshroom()
    : Plant()             // 虚基类初始化
    , SunProducingPlant()
    , Mushroom()
    , _growthState(GrowthState::SMALL_INIT)
    , _growthTimer(0.0f)
    , _currentScale(SMALL_SCALE)
{
    CCLOG("Sunshroom created.");
}

bool Sunshroom::init()
{
    // 调用 Plant 的初始化逻辑
    if (!initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, SUN_PRODUCTION_INTERVAL))
    {
        return false;
    }
    // Mushroom 的数据已经在构造函数中初始化，具体状态将在 update 第一帧设定
    return true;
}

Sunshroom* Sunshroom::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Sunshroom>(globalPos);
}

void Sunshroom::update(float delta)
{
    // 1. 调用 Plant 的基础更新
    Plant::update(delta);

    // 预览模式不执行逻辑
    if (!this->getParent()) return;

    // 2. 利用 Mushroom 基类检查昼夜变化
    bool envChanged = checkDayNightChange();

    // 3. 处理初始化或状态切换
    if (envChanged)
    {
        if (_isNightMode)
        {
            wakeUp(); // 从 Mushroom 调用的虚函数
        }
        else
        {
            sleep();  // 从 Mushroom 调用的虚函数
        }
    }

    // 4. 夜晚特有的生长逻辑
    if (_isNightMode)
    {
        // 确保不在睡觉状态
        if (_growthState == GrowthState::SLEEPING)
        {
            wakeUp();
        }

        // 计时生长
        if (_growthState == GrowthState::SMALL_INIT)
        {
            _growthTimer += delta;
            if (_growthTimer >= GROWTH_TIME)
            {
                startGrowingSequence();
            }
        }
    }
    else // 白天
    {
        if (_growthState != GrowthState::SLEEPING)
        {
            sleep();
        }
    }
}

void Sunshroom::wakeUp()
{
    // 阳光菇醒来时，要根据生长进度决定是变小还是变大
    Mushroom::_activityState = ActivityState::ACTIVE;

    if (_growthTimer >= GROWTH_TIME)
    {
        setGrowthState(GrowthState::GROWN);
    }
    else
    {
        setGrowthState(GrowthState::SMALL_INIT);
    }
}

void Sunshroom::sleep()
{
    Mushroom::_activityState = ActivityState::SLEEPING;
    setGrowthState(GrowthState::SLEEPING);
}

void Sunshroom::setGrowthState(GrowthState state)
{
    if (_growthState == state) return;

    _growthState = state;

    // 设置缩放
    switch (state)
    {
        case GrowthState::SMALL_INIT:
            _currentScale = SMALL_SCALE;
            break;
        case GrowthState::GROWING:
        case GrowthState::GROWN:
        case GrowthState::SLEEPING:
            _currentScale = GROWN_SCALE;
            break;
    }

    this->setScale(_currentScale);
    setAnimation();
}

void Sunshroom::setAnimation()
{
    // 停止当前动作
    if (_currentAnimation)
    {
        this->stopAction(_currentAnimation);
        _currentAnimation = nullptr;
    }

    Animation* animation = nullptr;

    // 使用 Mushroom::loadAnimation 来加载动画
    // 注意：这里不需要手动传裁剪参数，因为阳光菇用的是完整图
    switch (_growthState)
    {
        case GrowthState::SMALL_INIT:
            animation = loadAnimation("sunshroom/init", 10, SMALL_SCALE, OBJECT_SIZE.width, OBJECT_SIZE.height);
            break;

        case GrowthState::GROWING:
        case GrowthState::GROWN:
            animation = loadAnimation("sunshroom/grownup", 10, GROWN_SCALE, OBJECT_SIZE.width, OBJECT_SIZE.height);
            break;

        case GrowthState::SLEEPING:
            animation = loadAnimation("sunshroom/sleep", 14, GROWN_SCALE, OBJECT_SIZE.width, OBJECT_SIZE.height);
            break;
    }

    if (animation)
    {
        auto animate = Animate::create(animation);
        auto repeat = RepeatForever::create(animate);
        _currentAnimation = repeat;
        this->runAction(repeat);
    }
}

std::vector<Sun*> Sunshroom::produceSun()
{
    // 如果是白天（Mushroom提供的检查），不生产阳光
    if (isDaytime())
    {
        return {};
    }

    // 冷却检查
    if (_accumulatedTime >= _cooldownInterval)
    {
        _accumulatedTime = 0.0f;
        Sun* sun = nullptr;

        if (_growthState == GrowthState::SMALL_INIT || _growthState == GrowthState::GROWING)
        {
            sun = Sun::createCustomSun(this->getPosition(), 0.8f, SMALL_SUN_VALUE);
            CCLOG("Small Sunshroom produced sun: %d", SMALL_SUN_VALUE);
        }
        else if (_growthState == GrowthState::GROWN)
        {
            sun = Sun::createCustomSun(this->getPosition(), 1.0f, GROWN_SUN_VALUE);
            CCLOG("Grown Sunshroom produced sun: %d", GROWN_SUN_VALUE);
        }

        if (sun) return { sun };
    }

    return {};
}

void Sunshroom::startGrowingSequence()
{
    if (_currentAnimation)
    {
        this->stopAction(_currentAnimation);
        _currentAnimation = nullptr;
    }

    _growthState = GrowthState::GROWING;
    _currentScale = GROWN_SCALE;

    auto scaleUp = ScaleTo::create(0.5f, GROWN_SCALE);
    // 复用基类加载函数
    auto growUpAnimation = loadAnimation("sunshroom/grownup", 10, GROWN_SCALE, OBJECT_SIZE.width, OBJECT_SIZE.height);

    Action* growUpAction = growUpAnimation ? (Action*)Animate::create(growUpAnimation) : (Action*)DelayTime::create(1.0f);

    cocos2d::AudioEngine::play2d("plantgrow.mp3", false, 1.0f);

    auto switchToGrown = CallFunc::create(CC_CALLBACK_0(Sunshroom::onGrowthSequenceFinished, this));
    auto sequence = Sequence::create(scaleUp, growUpAction, switchToGrown, nullptr);
    this->runAction(sequence);
}

void Sunshroom::onGrowthSequenceFinished()
{
    setGrowthState(GrowthState::GROWN);
}