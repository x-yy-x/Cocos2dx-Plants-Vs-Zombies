#include "Jalapeno.h"
#include "Zombie.h"
#include "Sun.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Jalapeno::IMAGE_FILENAME = "jalapeno_spritesheet.png";
const std::string Jalapeno::EXPLOSION_IMAGE = "explosion.png";
const cocos2d::Rect Jalapeno::INITIAL_PIC_RECT = Rect(0, 0, 128, 128);
const cocos2d::Size Jalapeno::OBJECT_SIZE = Size(128, 128);
const float Jalapeno::ATTACK_RANGE = 0.0f;  // Not used for bomb
const int Jalapeno::EXPLOSION_DAMAGE = 1500;
const int Jalapeno::EXPLOSION_RADIUS = 1;  // 3x3 grid (radius of 1 from center)

// Protected constructor
Jalapeno::Jalapeno()
    : BombPlant()
    , _idleAnimationDuration(0.0f)
{
    _explosionDamage = EXPLOSION_DAMAGE;
    _explosionRadius = EXPLOSION_RADIUS;
    CCLOG("Jalapeno created.");
}

// ------------------------------------------------------------------------
// 1. Jalapeno initialization
// ------------------------------------------------------------------------
bool Jalapeno::init()
{
    if (!BombPlant::init())
    {
        return false;
    }

    if (!Sprite::initWithFile(IMAGE_FILENAME, INITIAL_PIC_RECT))
    {
        return false;
    }

    _maxHealth = 300;
    _currentHealth = 300;
    _cooldownInterval = 0.0f;  // No cooldown needed
    _accumulatedTime = 0.0f;
    _idleAnimationDuration = 0.0f;

    this->setAnimation();
    this->scheduleUpdate();

    return true;
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
Jalapeno* Jalapeno::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Jalapeno>(globalPos);
}

// ------------------------------------------------------------------------
// 3. Jalapeno animation (4x4 sprite sheet, plays once)
// ------------------------------------------------------------------------
void Jalapeno::setAnimation()
{
    this->stopAllActions();

    const float frameWidth = 85;
    const float frameHeight = 110;

    Vector<SpriteFrame*> frames;
    _idleAnimationDuration = 8 * 0.07f;
    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 4; col++)
        {

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "jalapeno_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.07f);
    auto animate = Animate::create(animation);

    this->runAction(animate);
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void Jalapeno::update(float delta)
{
    Plant::update(delta);

    // Check if idle animation has finished
    if (!_animationFinished && _accumulatedTime >= _idleAnimationDuration)
    {
        _animationFinished = true;
        CCLOG("Jalapeno idle animation finished, ready to explode!");
    }
}

// ------------------------------------------------------------------------
// 5. Explode function
// ------------------------------------------------------------------------
void Jalapeno::explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol)
{
    if (_hasExploded)
    {
        return;
    }

    if (!_animationFinished)
    {
        return;  // Wait for animation to finish
    }

    _hasExploded = true;

    // Get zombies in explosion range (3x3 grid)
    std::vector<Zombie*> zombiesInRange = allZombiesInRow[plantRow];

    // Deal damage to all zombies in range
    // Note: zombiesInRange is a local copy, so range-for is safe here
    // But we still check isDead() to skip dying zombies
    for (auto zombie : zombiesInRange)
    {
        if (zombie && !zombie->isDead())
        {
            zombie->takeDamage(_explosionDamage);
            CCLOG("Jalapeno dealt %d damage to zombie!", _explosionDamage);
        }
    }

    // Play explosion animation
    playExplosionAnimation();
}

// ------------------------------------------------------------------------
// 6. Explosion animation
// ------------------------------------------------------------------------
void Jalapeno::playExplosionAnimation()
{
    // Stop current animation
    this->stopAllActions();

    const float frameWidth = 950;
    const float frameHeight = 165;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 4; col++)
        {

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "fire_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.07f);
    auto animate = Animate::create(animation);

    this->setScaleX(1.28);
    auto targetpos = Vec2(650, this->getPositionY());
    this->runAction(Sequence::create(MoveTo::create(0.001f,targetpos), animate, RemoveSelf::create(), nullptr));
    CCLOG("Jalapeno explosion animation played!");

    //音效还没加
    //cocos2d::AudioEngine::play2d("cherrybomb.mp3", false);
}

