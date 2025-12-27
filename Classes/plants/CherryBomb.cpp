#include "CherryBomb.h"
#include "Zombie.h"
#include "Sun.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ----------------------------------------------------
// Static definitions
// ----------------------------------------------------
const std::string CherryBomb::IMAGE_FILENAME = "cherry_bomb_spritesheet.png";
const std::string CherryBomb::EXPLOSION_IMAGE = "explosion.png";
const cocos2d::Rect CherryBomb::INITIAL_PIC_RECT = Rect(0, 0, 128, 128);
const cocos2d::Size CherryBomb::OBJECT_SIZE = Size(128, 128);
const float CherryBomb::ATTACK_RANGE = 0.0f;
const int CherryBomb::EXPLOSION_DAMAGE = 1500;
const int CherryBomb::EXPLOSION_RADIUS = 1;

CherryBomb::CherryBomb()
    : BombPlant()
    , idle_animation_duration(0.0f)
{
    explosion_damage = EXPLOSION_DAMAGE;
    explosion_radius = EXPLOSION_RADIUS;
}

// ----------------------------------------------------
// Initialization & Planting
// ----------------------------------------------------

bool CherryBomb::init()
{
    if (!BombPlant::init()) return false;

    if (!Sprite::initWithFile(IMAGE_FILENAME, INITIAL_PIC_RECT)) return false;

    // CherryBombs have high health to prevent being eaten during their short arming time
    max_health = 1000;
    current_health = 1000;
    cooldown_interval = 0.0f;
    accumulated_time = 0.0f;
    idle_animation_duration = 0.0f;

    this->setAnimation();
    this->scheduleUpdate();

    return true;
}

CherryBomb* CherryBomb::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<CherryBomb>(globalPos);
}

// ----------------------------------------------------
// Logic & Detonation
// ----------------------------------------------------

void CherryBomb::setAnimation()
{
    const float frameWidth = 150.0f;
    const float frameHeight = 145.0f;
    const int totalFrames = 16;
    const float frameDelay = 0.07f;

    // Set the arming time based on the total frames of the spritesheet
    idle_animation_duration = totalFrames * frameDelay; // Approx 1.12 seconds

    auto animation = initAnimate(IMAGE_FILENAME, frameWidth, frameHeight, 4, 4, totalFrames, frameDelay);
    if (animation) {
        auto animate = Animate::create(animation);
        this->runAction(animate);
    }
}

void CherryBomb::update(float delta)
{
    Plant::update(delta);

    // Transition to the explosion state once the arming animation finishes
    if (!animation_finished && accumulated_time >= idle_animation_duration)
    {
        animation_finished = true;
        CCLOG("CherryBomb is armed and ready.");
    }
}

void CherryBomb::explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol)
{
    if (has_exploded || !animation_finished) return;

    has_exploded = true;

    // Detect all zombies within a 3x3 grid centered on this plant
    std::vector<Zombie*> zombiesInRange = getZombiesInRange(allZombiesInRow, plantRow, plantCol);

    for (auto zombie : zombiesInRange)
    {
        if (zombie && !zombie->isDead())
        {
            zombie->takeDamage(static_cast<float>(explosion_damage));
        }
    }

    playExplosionAnimation();
}



void CherryBomb::playExplosionAnimation()
{
    this->stopAllActions();

    // Create a temporary sprite for the explosion visual effect
    auto explosionSprite = Sprite::create(EXPLOSION_IMAGE);
    if (explosionSprite)
    {
        explosionSprite->setPosition(this->getPosition());
        if (this->getParent())
        {
            // Place explosion on a layer above plants
            this->getParent()->addChild(explosionSprite, PLANT_LAYER + 1);
        }

        auto fadeOut = FadeOut::create(0.5f);
        auto removePlant = CallFunc::create([this]() {
            this->is_dead = true; // Mark plant for cleanup in the next update cycle
            });
        auto removeSprite = RemoveSelf::create();

        explosionSprite->runAction(Sequence::create(fadeOut, removeSprite, nullptr));
        this->runAction(removePlant);
    }
    else
    {
        this->is_dead = true;
    }

    // Audio feedback
    cocos2d::AudioEngine::play2d("cherrybomb.mp3", false);
}