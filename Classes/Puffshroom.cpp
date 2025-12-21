#include "Puffshroom.h"
#include "Puff.h"
#include "GameWorld.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Puffshroom::IMAGE_FILENAME = "puffshroom/sleep/1 (1).png";
const cocos2d::Rect Puffshroom::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Puffshroom::OBJECT_SIZE = Size(85.333f, 78.0f);
const float Puffshroom::ATTACK_COOLDOWN = 1.5f;  // Attack every 1.5 seconds
const int Puffshroom::DETECTION_RANGE = 3;       // Detect zombies in 3 grid cells ahead

// Protected constructor
Puffshroom::Puffshroom()
    : AttackingPlant()
    , _activityState(ActivityState::ACTIVE)
    , _isNightMode(false)
    , _isInitialized(false)
    , _currentAnimation(nullptr)
{
    CCLOG("Puffshroom created.");
}

// ------------------------------------------------------------------------
// 1. Puffshroom initialization
// ------------------------------------------------------------------------
bool Puffshroom::init()
{
    // Initialize with basic plant settings (like Sunshroom does)
    // Use a neutral init image as default, animation will be set in first update
    if (!initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, ATTACK_COOLDOWN))
    {
        return false;
    }

    // Set initial state - will be determined in first update based on day/night mode
    _activityState = ActivityState::ACTIVE;
    _isNightMode = false;
    _isInitialized = false;
    _currentAnimation = nullptr;

    return true;
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
Puffshroom* Puffshroom::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Puffshroom>(globalPos);
}

// Puffshroom.cpp - 3. Puffshroom update

void Puffshroom::update(float delta)
{
    Plant::update(delta);

    if (!this->getParent()) return;

    // Update current day/night mode and detect changes
    bool modeChanged = updateDayNightMode();

    // Desired state: sleep in daytime, active at night
    ActivityState desiredState = isDaytime() ? ActivityState::SLEEPING : ActivityState::ACTIVE;

    // First-time init: force switch to desired state once
    if (!_isInitialized)
    {
        _isInitialized = true;
        setActivityState(desiredState);
        return;
    }

    // Only switch when mode actually changed
    if (modeChanged && _activityState != desiredState)
    {
        setActivityState(desiredState);
    }
}

// ------------------------------------------------------------------------
// 4. Check if daytime
// ------------------------------------------------------------------------
bool Puffshroom::isDaytime() const
{
    return !_isNightMode;
}

// ------------------------------------------------------------------------
// 5. Update day/night mode from GameWorld
// ------------------------------------------------------------------------
bool Puffshroom::updateDayNightMode()
{
    Scene* runningScene = Director::getInstance()->getRunningScene();

    // Try to cast to GameWorld
    auto gameWorld = dynamic_cast<GameWorld*>(runningScene);

    if (gameWorld)
    {
        bool newMode = gameWorld->isNightMode();
        bool changed = (newMode != _isNightMode);
        _isNightMode = newMode;

        // Log initial mode
        if (!_isInitialized)
        {
            CCLOG("Puffshroom INIT SUCCESS - Mode: %s", _isNightMode ? "NIGHT" : "DAY");
            return true;
        }

        // Return whether mode changed
        if (changed)
        {
            CCLOG("Puffshroom mode CHANGED to: %s", _isNightMode ? "NIGHT" : "DAY");
        }
        return changed;
    }
    else
    {
        // Conversion failed, GameWorld not found or not Scene type
        if (!_isInitialized)
        {
            CCLOG("Puffshroom INIT FAILED - Running Scene is NOT GameWorld.");
        }
    }
    return false;
}

// ------------------------------------------------------------------------
// 6. Set activity state and update animation
// ------------------------------------------------------------------------
void Puffshroom::setActivityState(ActivityState state)
{
    if (_activityState == state) return;

    CCLOG("Puffshroom changing activity state: %s -> %s",
          _activityState == ActivityState::ACTIVE ? "ACTIVE" : "SLEEPING",
          state == ActivityState::ACTIVE ? "ACTIVE" : "SLEEPING");

    _activityState = state;

    // Set the appropriate animation based on new state
    setAnimation();
}

// ------------------------------------------------------------------------
// 7. Wake up (night/active state)
// ------------------------------------------------------------------------
void Puffshroom::wakeUp()
{
    CCLOG("Puffshroom waking up (night mode)");
    setActivityState(ActivityState::ACTIVE);
}

// ------------------------------------------------------------------------
// 8. Sleep (day/sleeping state)
// ------------------------------------------------------------------------
void Puffshroom::sleep()
{
    CCLOG("Puffshroom going to sleep (day mode)");
    setActivityState(ActivityState::SLEEPING);
}

// ------------------------------------------------------------------------
// 9. Set animation based on state
// ------------------------------------------------------------------------
void Puffshroom::setAnimation()
{
    // ֹͣ���е�action����ֹ������ͻ
    this->stopAllActions();
    _currentAnimation = nullptr;

    Animation* animation = nullptr;

    if (_activityState == ActivityState::SLEEPING)
    {
        // �Ƚ���˯���״̬��һ֡��ֹ����ʾinitͼƬ
        this->setTexture("puffshroom/sleep/1 (1).png");

        // Sleeping state - set sleep animation (17 frames)
        // Adjust these parameters for fine cropping of sleep animation
        animation = loadAnimation("puffshroom/sleep", 17, 1.0f,
                                 0.0f, 0.0f, OBJECT_SIZE.width-33, OBJECT_SIZE.height);
    }
    else
    {
        // �Ƚ���init��һ֡
        this->setTexture("puffshroom/init/1 (1).png");

        // Active state - play init animation (14 frames)
        animation = loadAnimation("puffshroom/init", 14, 1.0f,
                                 0.0f, 7.0f, OBJECT_SIZE.width-33, OBJECT_SIZE.height);
    }

    if (animation)
    {
        auto animate = Animate::create(animation);
        auto repeat = RepeatForever::create(animate);
        _currentAnimation = repeat;
        this->runAction(repeat);
        CCLOG("Puffshroom animation set for state: %s", 
              _activityState == ActivityState::SLEEPING ? "SLEEPING" : "ACTIVE");
    }
    else
    {
        CCLOG("Failed to load Puffshroom animation for state: %s", 
              _activityState == ActivityState::SLEEPING ? "SLEEPING" : "ACTIVE");
    }
}

// ------------------------------------------------------------------------
// 10. Load animation helper
// ------------------------------------------------------------------------
Animation* Puffshroom::loadAnimation(const std::string& folderPath, int frameCount, float scale,
                                     float offsetX, float offsetY, float cropWidth, float cropHeight)
{
    Vector<SpriteFrame*> frames;

    // Use default values if not specified
    if (cropWidth < 0) cropWidth = OBJECT_SIZE.width;
    if (cropHeight < 0) cropHeight = OBJECT_SIZE.height;

    for (int i = 1; i <= frameCount; ++i)
    {
        std::string filename = folderPath + "/1 (" + std::to_string(i) + ").png";
        auto frame = SpriteFrame::create(filename, Rect(offsetX, offsetY, cropWidth, cropHeight));
        if (frame)
        {
            frames.pushBack(frame);
        }
        else
        {
            CCLOG("Failed to load frame: %s", filename.c_str());
        }
    }

    if (frames.size() == 0)
    {
        CCLOG("No frames loaded for animation: %s", folderPath.c_str());
        return nullptr;
    }

    // Create animation with 0.1 second per frame
    auto animation = Animation::createWithSpriteFrames(frames, 0.1f);
    return animation;
}

// ------------------------------------------------------------------------
// 11. Check and attack (override from AttackingPlant)
// ------------------------------------------------------------------------
std::vector<Bullet*> Puffshroom::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    std::vector<Bullet*> bullets;

    // Only attack if active (not sleeping)
    if (_activityState == ActivityState::SLEEPING)
    {
        return bullets;
    }

    // Check for zombies in the same row within 3 grid cells ahead
    float plantX = this->getPositionX();
    float maxRange = plantX + (CELLSIZE.width * DETECTION_RANGE); // 3 grid cells ahead

    bool zombieDetected = false;
    // CRITICAL FIX: Use iterator to avoid invalidation during iteration
    auto& zombiesInThisRow = allZombiesInRow[plantRow];
    for (auto it = zombiesInThisRow.begin(); it != zombiesInThisRow.end(); ++it)
    {
        Zombie* zombie = *it;
        // Check pointer validity and skip dead/dying zombies
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

    if (!zombieDetected)
    {
        return bullets;
    }

    // Check if cooldown is ready
    if (_accumulatedTime >= _cooldownInterval)
    {
        _accumulatedTime = 0.0f; // Reset cooldown

        // Create a new Puff bullet at the plant's position
        Vec2 spawnPos = this->getPosition() + Vec2(10.0f, -5.0f);

        Puff* puff = Puff::create(spawnPos);
        if (puff)
        {
            // 参数：文件路径, 是否循环, 音量 (0.0 到 1.0)
            cocos2d::AudioEngine::play2d("puff.mp3", false, 1.0f);
            bullets.push_back(puff);
            CCLOG("Puffshroom fired a puff at %f, %f", spawnPos.x, spawnPos.y);
        }
    }

    return bullets;
}

