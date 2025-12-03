#include "PeaShooter.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string PeaShooter::IMAGE_FILENAME = "peashooter_spritesheet.png";
const cocos2d::Rect PeaShooter::INITIAL_PIC_RECT = Rect(0, 512 - 128, 85.333, 128);
const cocos2d::Size PeaShooter::OBJECT_SIZE = Size(85.333, 128);
const float PeaShooter::ATTACK_RANGE = 2000.0f;

// Protected constructor
PeaShooter::PeaShooter()
{
    CCLOG("PeaShooter created.");
}

// ------------------------------------------------------------------------
// 1. PeaShooter initialization
// ------------------------------------------------------------------------
bool PeaShooter::init()
{
    if (!Plant::init())
    {
        return false;
    }

    if (!Sprite::initWithFile(IMAGE_FILENAME, INITIAL_PIC_RECT))
    {
        return false;
    }

    _maxHealth = 300;
    _currentHealth = 300;
    _cooldownInterval = 1.5f;  // Attack once every 1.5 seconds
    _accumulatedTime = 0.0f;

    this->setAnimation();
    this->scheduleUpdate();

    return true;
}

// ------------------------------------------------------------------------
// 2. Static planting function
// ------------------------------------------------------------------------
PeaShooter* PeaShooter::plantAtPosition(const Vec2& globalPos)
{
    int col = (globalPos.x - GRID_ORIGIN.x) / CELLSIZE.width;
    int row = (globalPos.y - GRID_ORIGIN.y) / CELLSIZE.height;

    if (col < 0 || col >= MAX_COL || row < 0 || row >= MAX_ROW) {
        return nullptr;
    }

    float centerX = GRID_ORIGIN.x + col * CELLSIZE.width + CELLSIZE.width * 0.5f;
    float centerY = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * 0.5f;

    int dx = 30, dy = 8;
    Vec2 plantPos(centerX + dx, centerY + dy);

    auto plant = PeaShooter::create();

    if (plant)
    {
        plant->setPlantPosition(plantPos);
    }

    return plant;
}

// ------------------------------------------------------------------------
// 3. PeaShooter animation
// ------------------------------------------------------------------------
void PeaShooter::setAnimation()
{
    const float frameWidth = 100;
    const float frameHeight = 100;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 6; col++)
        {
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

    this->runAction(RepeatForever::create(animate));
}

// ------------------------------------------------------------------------
// 4. Update function
// ------------------------------------------------------------------------
void PeaShooter::update(float delta)
{
    Plant::update(delta);
    // Plant::update handles cooldown logic
    // GameWorld will call attack() when conditions are met
}

// ------------------------------------------------------------------------
// 5. Attack logic (override from Plant base class)
// ------------------------------------------------------------------------
Bullet* PeaShooter::attack()
{
    // Check if cooldown is ready
    if (_accumulatedTime >= _cooldownInterval)
    {
        _accumulatedTime = 0.0f; // Reset cooldown
        
        // Create a new Pea bullet at the plant's position
        // Offset slightly to spawn from the "mouth"
        // Use getContentSize() to calculate offset if needed, or fixed value
        Vec2 spawnPos = this->getPosition() + Vec2(30, 20); 
        
        Pea* pea = Pea::create(spawnPos);
        if (pea)
        {
             CCLOG("PeaShooter fired a pea at %f, %f", spawnPos.x, spawnPos.y);
             return pea;
        }
    }
    return nullptr;
}
