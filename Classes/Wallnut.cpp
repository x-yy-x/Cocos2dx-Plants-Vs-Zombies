#include "Wallnut.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string Wallnut::IMAGE_FILENAME = "wallnut_spritesheet.png";
const cocos2d::Rect Wallnut::INITIAL_PIC_RECT = Rect(0.0f, 512.0f - 128.0f, 85.333f, 128.0f);
const cocos2d::Size Wallnut::OBJECT_SIZE = Size(85.333f, 128.0f);

// Protected constructor
Wallnut::Wallnut()
{
    CCLOG("Wallnut created.");
}

// ------------------------------------------------------------------------
// 1. PeaShooter initialization
// ------------------------------------------------------------------------
bool Wallnut::init()
{
    if (!Plant::init())
    {
        return false;
    }

    if (!Sprite::initWithFile(IMAGE_FILENAME, INITIAL_PIC_RECT))
    {
        return false;
    }

    _maxHealth = 1000;
    _currentHealth = 1000;
    _cooldownInterval = 1.5f;  // Attack once every 1.5 seconds
    _accumulatedTime = 0.0f;
    _currentState = WallnutState::NORMAL;

    this->setAnimation();
    this->setCrackedAnimation();
    this->scheduleUpdate();
    this->runAction(normalAnimation);
    return true;
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
Wallnut* Wallnut::plantAtPosition(const Vec2& globalPos)
{
    return createPlantAtPosition<Wallnut>(globalPos);
}

// ------------------------------------------------------------------------
// 3. PeaShooter animation
// ------------------------------------------------------------------------
void Wallnut::setAnimation()
{
    const float frameWidth = 100;
    const float frameHeight = 100;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            if (row == 5 && col == 2)
                break;
            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                IMAGE_FILENAME,
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.07f);
    auto animate = Animate::create(animation);

    this->normalAnimation = RepeatForever::create(animate);
    normalAnimation->retain();
}

void Wallnut::setCrackedAnimation()
{
    const float frameWidth = 100;
    const float frameHeight = 100;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            if (row == 5 && col == 2)
                break;
            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "wallnut_cracked_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.07f);
    auto animate = Animate::create(animation);

    this->crackedAnimation = RepeatForever::create(animate);
    crackedAnimation->retain();
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void Wallnut::update(float delta)
{
    Plant::update(delta);

    WallnutState newState = (_currentHealth >= 300)
        ? WallnutState::NORMAL
        : WallnutState::CRACKED;

    // Only switch animation when state changes
    if (newState != _currentState)
    {
        _currentState = newState;

        this->stopAllActions();

        if (_currentState == WallnutState::NORMAL)
            this->runAction(normalAnimation);
        else
            this->runAction(crackedAnimation);
    }
}

// ------------------------------------------------------------------------
// 5. Wallnut does not attack
// ------------------------------------------------------------------------
std::vector<Bullet*> Wallnut::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    // Wallnut is a defensive plant, does not attack
    return std::vector<Bullet*>();
}

