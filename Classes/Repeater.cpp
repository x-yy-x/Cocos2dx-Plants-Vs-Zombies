#include "Repeater.h"
#include "Pea.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Repeater::IMAGE_FILENAME = "repeater_spritesheet.png";
const cocos2d::Rect Repeater::INITIAL_PIC_RECT = Rect(0, 0, 85.333, 85.333);
const cocos2d::Size Repeater::OBJECT_SIZE = Size(85.333, 85.333);

// Protected constructor
Repeater::Repeater()
{
    CCLOG("Repeater created.");
}

// ------------------------------------------------------------------------
// 1. Repeater initialization
// ------------------------------------------------------------------------
bool Repeater::init()
{
    // Call Plant::init() instead of PeaShooter::init() to avoid loading PeaShooter's image
    if (!Plant::init())
    {
        return false;
    }

    // Load Repeater's specific image
    if (!Sprite::initWithFile(IMAGE_FILENAME, INITIAL_PIC_RECT))
    {
        return false;
    }

    // Set Repeater stats (same as PeaShooter except different image)
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
Repeater* Repeater::plantAtPosition(const Vec2& globalPos)
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

    auto plant = Repeater::create();

    if (plant)
    {
        plant->setPlantPosition(plantPos);
    }

    return plant;
}

// ------------------------------------------------------------------------
// 3. Repeater animation (4 rows x 6 columns, 512x512 / 24 frames)
// ------------------------------------------------------------------------
void Repeater::setAnimation()
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
// 4. Check and attack logic - shoots TWO peas simultaneously
// ------------------------------------------------------------------------
std::vector<Bullet*> Repeater::checkAndAttack(const std::vector<Zombie*>& zombiesInRow)
{
    std::vector<Bullet*> bullets;

    // Check if any zombie is in range and cooldown is ready
    if (!isZombieInRange(zombiesInRow) || _accumulatedTime < _cooldownInterval)
    {
        return bullets;
    }

    _accumulatedTime = 0.0f; // Reset cooldown
    
    // Create first pea
    Vec2 spawnPos1 = this->getPosition() + Vec2(30, 20);
    Pea* firstPea = Pea::create(spawnPos1);
    
    // Create second pea with horizontal offset (same height)
    Vec2 spawnPos2 = this->getPosition() + Vec2(70, 20);  // +10 horizontal offset
    Pea* secondPea = Pea::create(spawnPos2);
    
    if (firstPea)
    {
        bullets.push_back(firstPea);
    }
    
    if (secondPea)
    {
        bullets.push_back(secondPea);
    }
    
    if (!bullets.empty())
    {
        CCLOG("Repeater fired %d peas", (int)bullets.size());
    }
    
    return bullets;
}

