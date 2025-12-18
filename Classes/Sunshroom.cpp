#include "Sunshroom.h"
#include "Sun.h"
#include "GameWorld.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string Sunshroom::IMAGE_FILENAME = "sunshroom/init/1 (1).png";
const cocos2d::Rect Sunshroom::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Sunshroom::OBJECT_SIZE = Size(85.333f, 78.0f);
const float Sunshroom::SUN_PRODUCTION_INTERVAL = 15.0f;  // Produce sun every 24 seconds
const float Sunshroom::GROWTH_TIME = 25.0f;              // Time to grow from small to grown (5 seconds)
const float Sunshroom::SMALL_SCALE = 0.7f;              // Scale factor for small Sunshroom
const float Sunshroom::GROWN_SCALE = 1.0f;              // Scale factor for grown Sunshroom
const int Sunshroom::SMALL_SUN_VALUE = 15;              // Sun value for small Sunshroom
const int Sunshroom::GROWN_SUN_VALUE = 25;              // Sun value for grown Sunshroom

// Protected constructor
Sunshroom::Sunshroom()
    : _growthState(GrowthState::SMALL_INIT)
    , _growthTimer(0.0f)
    , _currentScale(SMALL_SCALE)
    , _currentAnimation(nullptr)
    , _isNightMode(false)
    , _isInitialized(false)
{
    CCLOG("Sunshroom created.");
}

// ------------------------------------------------------------------------
// 1. Sunshroom initialization
// ------------------------------------------------------------------------
bool Sunshroom::init()
{
    if (!initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 25, SUN_PRODUCTION_INTERVAL))
    {
        return false;
    }

    // Initial state will be set in first update when we can access GameWorld
    return true;
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
Sunshroom* Sunshroom::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Sunshroom>(globalPos);
}

// ------------------------------------------------------------------------
// 3. Sunshroom animation
// ------------------------------------------------------------------------
void Sunshroom::setAnimation()
{
    // Stop current animation if any
    if (_currentAnimation)
    {
        this->stopAction(_currentAnimation);
        _currentAnimation = nullptr;
    }

    Animation* animation = nullptr;

    switch (_growthState)
    {
    case GrowthState::SMALL_INIT:
        // Small init animation - 10 frames
        animation = loadAnimation("sunshroom/init", 10, SMALL_SCALE);
        break;

    case GrowthState::GROWING:
        // Growing animation - 10 frames, normal scale
        animation = loadAnimation("sunshroom/grownup", 10, GROWN_SCALE);
        break;

    case GrowthState::GROWN:
        // Grown state - play grownup animation (looping)
        animation = loadAnimation("sunshroom/grownup", 10, GROWN_SCALE);
        break;

    case GrowthState::SLEEPING:
        // Sleeping animation - 14 frames
        animation = loadAnimation("sunshroom/sleep", 14, GROWN_SCALE);
        break;
    }

    if (animation)
    {
        auto animate = Animate::create(animation);
        auto repeat = RepeatForever::create(animate);
        _currentAnimation = repeat;
        this->runAction(repeat);
        CCLOG("Sunshroom animation created for state %d", static_cast<int>(_growthState));
    }
    else
    {
        CCLOG("Failed to create Sunshroom animation for state %d", static_cast<int>(_growthState));
    }
}

// ------------------------------------------------------------------------
// 3.5. Load animation helper function
// ------------------------------------------------------------------------
Animation* Sunshroom::loadAnimation(const std::string& folderPath, int frameCount, float scale)
{
    Vector<SpriteFrame*> frames;

    for (int i = 1; i <= frameCount; ++i)
    {
        std::string filename = StringUtils::format("%s/1 (%d).png", folderPath.c_str(), i);
        auto frame = SpriteFrame::create(filename, Rect(0, 0, OBJECT_SIZE.width, OBJECT_SIZE.height));
        if (frame)
        {
            frames.pushBack(frame);
        }
        else
        {
            CCLOG("Failed to load frame: %s", filename.c_str());
        }
    }

    if (!frames.empty())
    {
        return Animation::createWithSpriteFrames(frames, 0.1f); // 0.1 seconds per frame
    }

    return nullptr;
}

// ------------------------------------------------------------------------
// 3.6. Set growth state
// ------------------------------------------------------------------------
void Sunshroom::setGrowthState(GrowthState state)
{
    if (_growthState == state)
        return;

    _growthState = state;

    // Update scale based on state
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

    CCLOG("Sunshroom changed to state %d", static_cast<int>(_growthState));
}

// ------------------------------------------------------------------------
// 3.7. Wake up function
// ------------------------------------------------------------------------
void Sunshroom::wakeUp()
{
    if (_growthState == GrowthState::SLEEPING)
    {
        // Determine appropriate state based on growth timer
        if (_growthTimer >= GROWTH_TIME)
        {
            setGrowthState(GrowthState::GROWN);
        }
        else
        {
            setGrowthState(GrowthState::SMALL_INIT);
        }
    }
}

// ------------------------------------------------------------------------
// 3.8. Sleep function
// ------------------------------------------------------------------------
void Sunshroom::sleep()
{
    if (_growthState != GrowthState::SLEEPING)
    {
        setGrowthState(GrowthState::SLEEPING);
    }
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void Sunshroom::update(float delta)
{
    Plant::update(delta);

    // Only update if we have a parent (i.e., we're actually planted, not just a preview)
    if (!this->getParent())
    {
        return; // Preview plants don't update
    }

    // Update day/night mode from GameWorld
    updateDayNightMode();

    // Initialize on first update (when we can access GameWorld)
    if (!_isInitialized)
    {
        _isInitialized = true;

        // Set initial state based on current mode
        if (_isNightMode)
        {
            // Night mode: start small and grow
            this->setScale(SMALL_SCALE);
            setGrowthState(GrowthState::SMALL_INIT);
            CCLOG("Sunshroom initialized in NIGHT mode (SMALL_INIT)");
        }
        else
        {
            // Day mode: start sleeping
            this->setScale(GROWN_SCALE);
            setGrowthState(GrowthState::SLEEPING);
            CCLOG("Sunshroom initialized in DAY mode (SLEEPING)");
        }
        return; // Skip rest of update on initialization frame
    }

    // Handle growth timer and state transitions
    if (!isDaytime()) // Night time
    {
        // Wake up if sleeping
        if (_growthState == GrowthState::SLEEPING)
        {
            wakeUp();
        }

        // Handle growth progression
        if (_growthState == GrowthState::SMALL_INIT)
        {
            _growthTimer += delta;
            if (_growthTimer >= GROWTH_TIME)
            {
                // Start three-stage growing animation:
                // 1. Scale up animation (0.7 -> 1.0)
                // 2. Play grownup animation
                // 3. Switch to GROWN state
                startGrowingSequence();
            }
        }
    }
    else // Day time
    {
        // Go to sleep if not already sleeping
        if (_growthState != GrowthState::SLEEPING)
        {
            sleep();
        }
    }
}

// ------------------------------------------------------------------------
// 5. Produce sun
// ------------------------------------------------------------------------
std::vector<Sun*> Sunshroom::produceSun()
{
    // Only produce sun at night, sleep during day
    if (isDaytime())
    {
        // During daytime, Sunshroom sleeps and doesn't produce sun
        return {};
    }

    // Check if cooldown is finished (same logic as Sunflower)
    if (_accumulatedTime >= _cooldownInterval)
    {
        // Reset cooldown
        _accumulatedTime = 0.0f;

        Sun* sun = nullptr;

        // Create different sun based on growth state
        if (_growthState == GrowthState::SMALL_INIT)
        {
            // Small sunshroom produces small sun (smaller scale, 15 value)
            sun = Sun::createCustomSun(this->getPosition(), 0.8f, SMALL_SUN_VALUE);
            CCLOG("Sunshroom (small) produced a small sun! Value: %d", SMALL_SUN_VALUE);
        }
        else if (_growthState == GrowthState::GROWN)
        {
            // Grown sunshroom produces normal sun (normal scale, 25 value)
            sun = Sun::createCustomSun(this->getPosition(), 1.0f, GROWN_SUN_VALUE);
            CCLOG("Sunshroom (grown) produced a normal sun! Value: %d", GROWN_SUN_VALUE);
        }
        else if (_growthState == GrowthState::GROWING)
        {
            // During growing animation, produce small sun
            sun = Sun::createCustomSun(this->getPosition(), 0.8f, SMALL_SUN_VALUE);
            CCLOG("Sunshroom (growing) produced a small sun! Value: %d", SMALL_SUN_VALUE);
        }

        return { sun };
    }

    return {};
}

// ------------------------------------------------------------------------
// 6. Check if it's daytime
// ------------------------------------------------------------------------
bool Sunshroom::isDaytime() const
{
    return !_isNightMode;
}

// ------------------------------------------------------------------------
// 7. Update day/night mode from GameWorld
// ------------------------------------------------------------------------
bool Sunshroom::updateDayNightMode()
{
    Scene* runningScene = Director::getInstance()->getRunningScene();
    auto gameWorld = dynamic_cast<GameWorld*>(runningScene);

    if (gameWorld)
    {
        bool newMode = gameWorld->isNightMode();
        bool changed = (newMode != _isNightMode);
        _isNightMode = newMode;

        if (!_isInitialized)
        {
            CCLOG("Sunshroom INIT SUCCESS - Mode: %s", _isNightMode ? "NIGHT" : "DAY");
            return true;
        }

        if (changed)
        {
            CCLOG("Sunshroom mode CHANGED to: %s", _isNightMode ? "NIGHT" : "DAY");
        }

        return changed;
    }
    else
    {
        if (!_isInitialized)
        {
            CCLOG("Sunshroom INIT FAILED - Running Scene is NOT GameWorld.");
        }
    }
    return false;
}

// ------------------------------------------------------------------------
// 8. Start growing sequence
// ------------------------------------------------------------------------
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
    auto growUpAnimation = loadAnimation("sunshroom/grownup", 10, GROWN_SCALE);
    Action* growUpAction = nullptr;

    if (growUpAnimation)
    {
        growUpAction = Animate::create(growUpAnimation);
    }
    else
    {
        growUpAction = DelayTime::create(1.0f);
    }

    auto switchToGrown = CallFunc::create(CC_CALLBACK_0(Sunshroom::onGrowthSequenceFinished, this));
    auto sequence = Sequence::create(scaleUp, growUpAction, switchToGrown, nullptr);
    this->runAction(sequence);

    CCLOG("Sunshroom started growing sequence.");
}


// ------------------------------------------------------------------------
// 8.2. CallFunc callback: growth sequence finished
// ------------------------------------------------------------------------
void Sunshroom::onGrowthSequenceFinished()
{
    setGrowthState(GrowthState::GROWN);
    CCLOG("Sunshroom growth sequence finished. State switched to GROWN.");
}