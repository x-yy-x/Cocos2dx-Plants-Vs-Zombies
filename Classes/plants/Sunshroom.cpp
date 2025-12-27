#include "Sunshroom.h"
#include "Sun.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ----------------------------------------------------
// Static Configuration
// ----------------------------------------------------
const std::string Sunshroom::IMAGE_FILENAME = "sunshroom/init/1 (1).png";
const cocos2d::Rect Sunshroom::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Sunshroom::OBJECT_SIZE = Size(85.333f, 78.0f);
const float Sunshroom::SUN_PRODUCTION_INTERVAL = 15.0f;
const float Sunshroom::GROWTH_TIME = 25.0f;
const float Sunshroom::SMALL_SCALE = 0.7f;
const float Sunshroom::GROWN_SCALE = 1.0f;
const int Sunshroom::SMALL_SUN_VALUE = 15;
const int Sunshroom::GROWN_SUN_VALUE = 25;

Sunshroom::Sunshroom()
    : Plant()
    , SunProducingPlant()
    , Mushroom()
    , growth_state(GrowthState::SMALL_INIT)
    , growth_timer(0.0f)
    , current_scale(SMALL_SCALE)
{
    CCLOG("Sunshroom instance created.");
}

bool Sunshroom::init()
{
    // Initialize with 80 health and 15s production interval
    if (!initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, SUN_PRODUCTION_INTERVAL))
    {
        return false;
    }
    return true;
}

Sunshroom* Sunshroom::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Sunshroom>(globalPos);
}

// ----------------------------------------------------
// Life Cycle & State Management
// ----------------------------------------------------

void Sunshroom::update(float delta)
{
    Plant::update(delta);

    if (!this->getParent()) return;

    // 1. Sync environment state via Mushroom base class
    bool envChanged = checkDayNightChange();

    if (envChanged)
    {
        if (is_night_mode) wakeUp();
        else sleep();
    }

    // 2. Night-specific growth logic
    if (is_night_mode)
    {
        // Force wakeup if it's night but marked as sleeping
        if (growth_state == GrowthState::SLEEPING)
        {
            wakeUp();
        }

        // Maturation timer logic
        if (growth_state == GrowthState::SMALL_INIT)
        {
            growth_timer += delta;
            if (growth_timer >= GROWTH_TIME)
            {
                startGrowingSequence();
            }
        }
    }
    else // Daytime
    {
        if (growth_state != GrowthState::SLEEPING)
        {
            sleep();
        }
    }
}

void Sunshroom::wakeUp()
{
    Mushroom::activity_state = ActivityState::ACTIVE;

    // Restore state based on whether it had finished maturing before sleeping
    if (growth_timer >= GROWTH_TIME)
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
    Mushroom::activity_state = ActivityState::SLEEPING;
    setGrowthState(GrowthState::SLEEPING);
}

void Sunshroom::setGrowthState(GrowthState state)
{
    if (growth_state == state) return;

    growth_state = state;

    // Update scale based on stage
    switch (state)
    {
        case GrowthState::SMALL_INIT:
            current_scale = SMALL_SCALE;
            break;
        case GrowthState::GROWING:
        case GrowthState::GROWN:
        case GrowthState::SLEEPING:
            current_scale = GROWN_SCALE;
            break;
    }

    this->setScale(current_scale);
    setAnimation();
}

// ----------------------------------------------------
// Animation & Production
// ----------------------------------------------------

void Sunshroom::setAnimation()
{
    if (current_animation)
    {
        this->stopAction(current_animation);
        current_animation = nullptr;
    }

    Animation* animation = nullptr;

    switch (growth_state)
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
        current_animation = repeat;
        this->runAction(repeat);
    }
}

std::vector<Sun*> Sunshroom::produceSun()
{
    // Sun-shrooms (and all mushrooms) are inactive during the day
    if (isDaytime()) return {};

    if (accumulated_time >= cooldown_interval)
    {
        accumulated_time = 0.0f;
        Sun* sun = nullptr;

        if (growth_state == GrowthState::SMALL_INIT || growth_state == GrowthState::GROWING)
        {
            // Spawn small sun (15 value, 0.8 scale)
            sun = Sun::createCustomSun(this->getPosition(), 0.8f, SMALL_SUN_VALUE);
        }
        else if (growth_state == GrowthState::GROWN)
        {
            // Spawn standard sun (25 value, 1.0 scale)
            sun = Sun::createCustomSun(this->getPosition(), 1.0f, GROWN_SUN_VALUE);
        }

        if (sun) return { sun };
    }

    return {};
}

void Sunshroom::startGrowingSequence()
{
    if (current_animation)
    {
        this->stopAction(current_animation);
        current_animation = nullptr;
    }

    growth_state = GrowthState::GROWING;
    current_scale = GROWN_SCALE;

    auto scaleUp = ScaleTo::create(0.5f, GROWN_SCALE);
    auto growUpAnim = loadAnimation("sunshroom/grownup", 10, GROWN_SCALE, OBJECT_SIZE.width, OBJECT_SIZE.height);

    Action* growUpAction = growUpAnim ? (Action*)Animate::create(growUpAnim) : (Action*)DelayTime::create(1.0f);

    cocos2d::AudioEngine::play2d("plantgrow.mp3", false, 1.0f);

    auto switchToGrown = CallFunc::create(CC_CALLBACK_0(Sunshroom::onGrowthSequenceFinished, this));
    auto sequence = Sequence::create(scaleUp, growUpAction, switchToGrown, nullptr);
    this->runAction(sequence);
}

void Sunshroom::onGrowthSequenceFinished()
{
    setGrowthState(GrowthState::GROWN);
}