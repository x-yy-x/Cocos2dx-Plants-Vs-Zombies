#include "Jalapeno.h"
#include "Zombie.h"
#include "Sun.h"
#include "GameWorld.h"
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
        CCLOG("BombPlant::init() failed!");
        return false;
    }

    if (!Sprite::initWithFile(IMAGE_FILENAME, INITIAL_PIC_RECT))
    {
        CCLOG("Sprite::initWithFile failed! Check if %s exists.", IMAGE_FILENAME.c_str());
        return false;
    }

    _maxHealth = 1000;
    _currentHealth = 1000;
    _cooldownInterval = 0.0f;
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

    _idleAnimationDuration = 8 * 0.07f;
    
    auto animation = initAnimate("jalapeno_spritesheet.png", frameWidth, frameHeight, 2, 4, 8, 0.07f);
    if (animation) {
        auto animate = Animate::create(animation);
        this->runAction(animate);
    }
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
            zombie->takeDamage(static_cast<float>(_explosionDamage));
            CCLOG("Jalapeno dealt %d damage to zombie!", _explosionDamage);
        }
    }

    // Play explosion animation
    playExplosionAnimation();

    auto gameWorld = static_cast<GameWorld*>(this->getParent());
    if (!gameWorld) return;

    gameWorld->removeIceInRow(plantRow);
}

// ------------------------------------------------------------------------
// 6. Explosion animation
// ------------------------------------------------------------------------
void Jalapeno::playExplosionAnimation()
{
    this->stopAllActions();
    cocos2d::AudioEngine::play2d("Jalapeno.mp3", false, 1.0f);
    auto explosionSprite = Sprite::create("fire_spritesheet.png");
    if (explosionSprite)
    {
        const float frameWidth = 950;
        const float frameHeight = 165;
        const int row = 2;
        const int col = 4;
        const int frameCount = 8;
        const float delay = 0.07f;

        auto animation = initAnimate("fire_spritesheet.png", frameWidth, frameHeight, row, col, frameCount, delay);
        if (animation)
        {
            auto animate = Animate::create(animation);
            auto targetpos = Vec2(650, this->getPositionY());
            explosionSprite->setScaleX(1.28f);
            explosionSprite->setPosition(targetpos);
            if (this->getParent())
                this->getParent()->addChild(explosionSprite, this->getLocalZOrder() + 1);

            auto sequence = Sequence::create(
                animate,
                FadeOut::create(0.5f),
                RemoveSelf::create(),
                nullptr
            );

            explosionSprite->runAction(sequence);
        }
        else
            explosionSprite->runAction(RemoveSelf::create());
        this->_isDead = true;
    }
    else
        this->_isDead = true;

    CCLOG("Jalapeno explosion animation played!");
}
