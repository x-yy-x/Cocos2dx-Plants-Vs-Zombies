#include "CherryBomb.h"
#include "Zombie.h"
#include "Sun.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string CherryBomb::IMAGE_FILENAME = "cherry_bomb_spritesheet.png";
const std::string CherryBomb::EXPLOSION_IMAGE = "explosion.png";
const cocos2d::Rect CherryBomb::INITIAL_PIC_RECT = Rect(0, 0, 128, 128);
const cocos2d::Size CherryBomb::OBJECT_SIZE = Size(128, 128);
const float CherryBomb::ATTACK_RANGE = 0.0f;  // Not used for bomb
const int CherryBomb::EXPLOSION_DAMAGE = 100000;
const int CherryBomb::EXPLOSION_RADIUS = 1;  // 3x3 grid (radius of 1 from center)

// Protected constructor
CherryBomb::CherryBomb()
    : BombPlant()
    , _idleAnimationDuration(0.0f)
{
    _explosionDamage = EXPLOSION_DAMAGE;
    _explosionRadius = EXPLOSION_RADIUS;
    CCLOG("CherryBomb created.");
}

// ------------------------------------------------------------------------
// 1. CherryBomb initialization
// ------------------------------------------------------------------------
bool CherryBomb::init()
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
CherryBomb* CherryBomb::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<CherryBomb>(globalPos);
}

// ------------------------------------------------------------------------
// 3. CherryBomb animation (4x4 sprite sheet, plays once)
// ------------------------------------------------------------------------
void CherryBomb::setAnimation()
{
    const float frameWidth = 150.0f;
    const float frameHeight = 145.0f;
    const int totalFrames = 16;
    
    // Calculate animation duration
    _idleAnimationDuration = totalFrames * 0.07f;  // 1.12 seconds

    // Create animation frames
    Vector<SpriteFrame*> frames;
    for (int i = 0; i < totalFrames; i++)
    {
        int row = i / 4;
        int col = i % 4;
        float x = col * frameWidth;
        float y = row * frameHeight;

        auto frame = SpriteFrame::create(IMAGE_FILENAME, Rect(x, y, frameWidth, frameHeight));
        if (frame) frames.pushBack(frame);
    }

    // Play animation once (not repeat)
    if (!frames.empty())
    {
        auto animation = Animation::createWithSpriteFrames(frames, 0.07f);
        auto animate = Animate::create(animation);
        this->runAction(animate);
    }
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void CherryBomb::update(float delta)
{
    Plant::update(delta);

    // Check if idle animation has finished
    if (!_animationFinished && _accumulatedTime >= _idleAnimationDuration)
    {
        _animationFinished = true;
        CCLOG("CherryBomb idle animation finished, ready to explode!");
    }
}

// ------------------------------------------------------------------------
// 5. Explode function
// ------------------------------------------------------------------------
void CherryBomb::explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol)
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
    std::vector<Zombie*> zombiesInRange = getZombiesInRange(allZombiesInRow, plantRow, plantCol);

    // Deal damage to all zombies in range
    for (auto zombie : zombiesInRange)
    {
        if (zombie && !zombie->isDead())
        {
            zombie->takeDamage(_explosionDamage);
            CCLOG("CherryBomb dealt %d damage to zombie!", _explosionDamage);
        }
    }

    // Play explosion animation
    playExplosionAnimation();
}

// ------------------------------------------------------------------------
// 6. Explosion animation
// ------------------------------------------------------------------------
void CherryBomb::playExplosionAnimation()
{
    // Stop current animation
    this->stopAllActions();

    // Load explosion sprite
    auto explosionSprite = Sprite::create(EXPLOSION_IMAGE);
    if (explosionSprite)
    {
        explosionSprite->setPosition(this->getPosition());
        if (this->getParent())
        {
            this->getParent()->addChild(explosionSprite, PLANT_LAYER + 1);
        }

        // Fade out and remove
        auto fadeOut = FadeOut::create(0.5f);
        auto removeCallback = CallFunc::create([this]() {
            // Mark plant as dead so it gets removed
            this->_isDead = true;
        });
        auto removeSprite = RemoveSelf::create();
        auto sequence = Sequence::create(fadeOut, removeSprite, nullptr);
        
        explosionSprite->runAction(sequence);
        this->runAction(removeCallback);
    }
    else
    {
        // If explosion sprite fails to load, just mark as dead
        this->_isDead = true;
    }

    CCLOG("CherryBomb explosion animation played!");
}

