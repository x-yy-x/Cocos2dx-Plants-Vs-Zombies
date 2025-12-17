#include "Jalapeno.h"
#include "Zombie.h"
#include "Sun.h"
#include "GameWorld.h";
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

    _maxHealth = 300;
    _currentHealth = 300;
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
            if (frame == nullptr) {
                CCLOG("SpriteFrame::create failed for jalapeno_spritesheet.png at (%.1f, %.1f)", x, y);
                continue;
            }
            frames.pushBack(frame);
        }
    }

    if (frames.empty()) {
        CCLOG("No valid frames for Jalapeno idle animation!");
        return;
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.07f);
    if (!animation) {
        CCLOG("Animation::createWithSpriteFrames failed!");
        return;
    }
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

    auto gameWorld = dynamic_cast<GameWorld*>(this->getParent());
    if (!gameWorld) return;

    gameWorld->removeIceInRow(plantRow);
}

// ------------------------------------------------------------------------
// 6. Explosion animation
// ------------------------------------------------------------------------
void Jalapeno::playExplosionAnimation()
{
    this->stopAllActions();

    auto explosionSprite = Sprite::create("fire_spritesheet.png");
    if (explosionSprite)
    {
        explosionSprite->setPosition(this->getPosition());
        if (this->getParent())
        {
            this->getParent()->addChild(explosionSprite, this->getLocalZOrder() + 1);
        }

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
                if (frame) frames.pushBack(frame);
            }
        }
        if (!frames.empty())
        {
            auto targetpos = Vec2(650, this->getPositionY());

            auto animation = Animation::createWithSpriteFrames(frames, 0.07f);
            auto animate = Animate::create(animation);

            this->setScaleX(1.28);
            auto sequence = Sequence::create(
                MoveTo::create(0.01f, targetpos),
                animate,
                FadeOut::create(0.5f),
                RemoveSelf::create(),
                nullptr
            );

            explosionSprite->runAction(sequence);
        }
        else
        {
            explosionSprite->runAction(RemoveSelf::create());
        }
        this->_isDead = true;
    }
    else
    {
        this->_isDead = true;
    }

    CCLOG("Jalapeno explosion animation played!");
}
