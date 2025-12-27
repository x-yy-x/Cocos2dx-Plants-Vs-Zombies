#include "Puffshroom.h"
#include "Puff.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ----------------------------------------------------
// Static Configuration
// ----------------------------------------------------
const std::string Puffshroom::IMAGE_FILENAME = "puffshroom/sleep/1 (1).png";
const cocos2d::Rect Puffshroom::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Puffshroom::OBJECT_SIZE = Size(85.333f, 78.0f);
const float Puffshroom::ATTACK_COOLDOWN = 1.5f;
const int Puffshroom::DETECTION_RANGE = 3; // Measured in grid cells

Puffshroom::Puffshroom()
    : Plant()
    , AttackingPlant()
    , Mushroom()
{
    CCLOG("Puffshroom instance created.");
}

bool Puffshroom::init()
{
    // Initialize health (80) and fire rate (1.5s)
    if (!initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 80, ATTACK_COOLDOWN))
    {
        return false;
    }

    // Initial state (Sleep/Wake) is synchronized in the first update frame via Mushroom
    return true;
}

Puffshroom* Puffshroom::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Puffshroom>(globalPos);
}

// ----------------------------------------------------
// State & Animation Management
// ----------------------------------------------------

void Puffshroom::update(float delta)
{
    Plant::update(delta);

    if (!this->getParent()) return;

    // Synchronize with GameWorld day/night cycle
    bool envChanged = checkDayNightChange();

    if (envChanged)
    {
        if (is_night_mode) wakeUp();
        else sleep();
    }
}

void Puffshroom::wakeUp()
{
    if (activity_state != ActivityState::ACTIVE)
    {
        activity_state = ActivityState::ACTIVE;
        setAnimation();
    }
}

void Puffshroom::sleep()
{
    if (activity_state != ActivityState::SLEEPING)
    {
        activity_state = ActivityState::SLEEPING;
        setAnimation();
    }
}

void Puffshroom::setAnimation()
{
    this->stopAllActions();
    current_animation = nullptr;

    Animation* animation = nullptr;

    if (activity_state == ActivityState::SLEEPING)
    {
        this->setTexture("puffshroom/sleep/1 (1).png");
        /**
         * Sleep Animation: 17 frames.
         * Sliced with a 33px width reduction to remove whitespace from spritesheet.
         */
        animation = loadAnimation("puffshroom/sleep", 17, 1.0f,
            OBJECT_SIZE.width, OBJECT_SIZE.height,
            0.0f, 0.0f, OBJECT_SIZE.width - 33.0f, OBJECT_SIZE.height);
    }
    else
    {
        this->setTexture("puffshroom/init/1 (1).png");
        /**
         * Active Animation: 14 frames.
         * Adjusted with Y-offset of 7 to align properly with the ground.
         */
        animation = loadAnimation("puffshroom/init", 14, 1.0f,
            OBJECT_SIZE.width, OBJECT_SIZE.height,
            0.0f, 7.0f, OBJECT_SIZE.width - 33.0f, OBJECT_SIZE.height);
    }

    if (animation)
    {
        auto animate = Animate::create(animation);
        auto repeat = RepeatForever::create(animate);
        current_animation = repeat;
        this->runAction(repeat);
    }
}

// ----------------------------------------------------
// Attack Logic
// ----------------------------------------------------

std::vector<Bullet*> Puffshroom::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    // Puff-shrooms do not attack during the day unless awakened by a Coffee Bean
    if (isDaytime()) return {};

    float plantX = this->getPositionX();
    float maxRange = plantX + (CELLSIZE.width * DETECTION_RANGE);
    bool zombieDetected = false;

    // Scan for zombies specifically within the short detection range
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

    // Handle firing cooldown
    if (accumulated_time >= cooldown_interval)
    {
        accumulated_time = 0.0f;

        // Offset spawn position so the puff comes out of the "mouth"
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