#include "Plant.h"
#include "Zombie.h"
#include "Bullet.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const float Plant::ATTACK_RANGE = 1000.0f;  // Default attack range

// Protected constructor
Plant::Plant()
    : _isDead(false)
    , _maxHealth(100)
    , _currentHealth(100)
    , _cooldownInterval(1.0f)
    , _accumulatedTime(0.0f)
    , _plantPos(Vec2::ZERO)
{
    CCLOG("Plant created.");
}

// Destructor
Plant::~Plant()
{
    CCLOG("Plant destroyed.");
}

// Initialization function
bool Plant::init()
{
    // Call parent class initialization
    if (!GameObject::init())
    {
        return false;
    }

    // Plant-specific initialization logic
    _isDead = false;
    _accumulatedTime = 0.0f;

    return true;
}

// Update every frame
void Plant::update(float delta)
{
    // Skip if already dead
    if (_isDead)
    {
        return;
    }

    // Accumulate time
    _accumulatedTime += delta;
}

// Check if dead
bool Plant::isDead() const
{
    return _isDead;
}

// Take damage
void Plant::takeDamage(float damage)
{
    if (_isDead)
    {
        return;
    }

    _currentHealth -= damage;
    CCLOG("Plant took %d damage, remaining health: %d", damage, _currentHealth);

    if (_currentHealth <= 0)
    {
        _currentHealth = 0;
        _isDead = true;
        CCLOG("Plant is dead.");
        // Can add death effects here, remove from scene, etc.
    }
}

// Set plant position
void Plant::setPlantPosition(const cocos2d::Vec2& pos)
{
    _plantPos = pos;
    this->setPosition(pos);
}

// Helper method to initialize plant with common settings
bool Plant::initPlantWithSettings(const std::string& imageFile, 
                                  const Rect& initialRect,
                                  int maxHealth, 
                                  float cooldown)
{
    if (!Plant::init())
    {
        return false;
    }

    if (!Sprite::initWithFile(imageFile, initialRect))
    {
        CCLOG("Failed to load plant image: %s", imageFile.c_str());
        return false;
    }

    _maxHealth = maxHealth;
    _currentHealth = maxHealth;
    _cooldownInterval = cooldown;
    _accumulatedTime = 0.0f;

    this->setAnimation();
    this->scheduleUpdate();

    return true;
}

// Create animation from sprite sheet
void Plant::createAndRunAnimation(const std::string& imageFile,
                                  float frameWidth, float frameHeight,
                                  int rows, int cols,
                                  float frameDuration,
                                  int totalFrames)
{
    Vector<SpriteFrame*> frames;
    int maxFrames = (totalFrames > 0) ? totalFrames : (rows * cols);
    int frameCount = 0;

    for (int row = 0; row < rows && frameCount < maxFrames; row++)
    {
        for (int col = 0; col < cols && frameCount < maxFrames; col++)
        {
            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                imageFile,
                Rect(x, y, frameWidth, frameHeight)
            );

            if (frame)
            {
                frames.pushBack(frame);
                frameCount++;
            }
        }
    }

    if (!frames.empty())
    {
        auto animation = Animation::createWithSpriteFrames(frames, frameDuration);
        auto animate = Animate::create(animation);
        this->runAction(RepeatForever::create(animate));
    }
}

// Set animation (to be implemented by subclasses)
void Plant::setAnimation()
{
    // Subclasses will implement their specific animations
    CCLOG("Plant::setAnimation() called (to be implemented by subclass).");
}
