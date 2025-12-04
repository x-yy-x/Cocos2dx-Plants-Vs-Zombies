#include "Wallnut.h"

USING_NS_CC;

// ------------------------------------------------------------------------
// 0. Static constant definitions
// ------------------------------------------------------------------------
const std::string Wallnut::IMAGE_FILENAME = "wallnut_spritesheet.png";
const cocos2d::Rect Wallnut::INITIAL_PIC_RECT = Rect(0, 512 - 128, 85.333, 128);
const cocos2d::Size Wallnut::OBJECT_SIZE = Size(85.333, 128);

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
    int col = (globalPos.x - GRID_ORIGIN.x) / CELLSIZE.width;
    int row = (globalPos.y - GRID_ORIGIN.y) / CELLSIZE.height;

    if (col < 0 || col >= MAX_COL || row < 0 || row >= MAX_ROW) {
        return nullptr;
    }

    float centerX = GRID_ORIGIN.x + col * CELLSIZE.width + CELLSIZE.width * 0.5f;
    float centerY = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * 0.5f;

    int dx = 30, dy = 8;
    Vec2 plantPos(centerX + dx, centerY + dy);

    auto plant = Wallnut::create();

    if (plant)
    {
        plant->setPlantPosition(plantPos);
    }

    return plant;
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

    // ֻ��״̬�仯ʱ���л��������������� runAction��
    if (newState != _currentState)
    {
        _currentState = newState;

        this->stopAllActions();  //  ����ͣ���ɶ���

        if (_currentState == WallnutState::NORMAL)
            this->runAction(normalAnimation);    //  ��ȷʹ��
        else
            this->runAction(crackedAnimation);
    }
}



