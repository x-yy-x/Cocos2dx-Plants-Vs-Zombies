#include "Sunflower.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string Sunflower::IMAGE_FILENAME = "Sunflower_spritesheet.png";
const cocos2d::Rect Sunflower::INITIAL_PIC_RECT = Rect(0, 512 - 128, 85.333, 128);
const cocos2d::Size Sunflower::OBJECT_SIZE = Size(85.333, 128);

// Protected constructor
Sunflower::Sunflower()
{
    CCLOG("Sunflower created.");
}

// ------------------------------------------------------------------------
// 1. PeaShooter initialization
// ------------------------------------------------------------------------
bool Sunflower::init()
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
Sunflower* Sunflower::plantAtPosition(const Vec2& globalPos)
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

    auto plant = Sunflower::create();

    if (plant)
    {
        plant->setPlantPosition(plantPos);
    }

    return plant;
}

// ------------------------------------------------------------------------
// 3. PeaShooter animation
// ------------------------------------------------------------------------
void Sunflower::setAnimation()
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
void Sunflower::update(float delta)
{
    Plant::update(delta);
    // Plant::update handles cooldown logic
    // GameWorld will call attack() when conditions are met
}

