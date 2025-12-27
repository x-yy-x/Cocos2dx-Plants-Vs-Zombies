#include "GameObject.h"

USING_NS_CC;

// ---------------------------------------------------------
// Constructor & Destructor
// ---------------------------------------------------------

GameObject::GameObject()
{
    CCLOG("GameObject instance created.");
}

GameObject::~GameObject()
{
    CCLOG("GameObject instance destroyed.");
}

bool GameObject::init()
{
    if (!Sprite::init())
    {
        return false;
    }
    return true;
}

// ---------------------------------------------------------
// Animation Helper Methods
// ---------------------------------------------------------

cocos2d::Animation* GameObject::initAnimate(const std::string& fileName, float frameWidth, float frameHeight,
    int row, int col, int frameCount, float delay)
{
    Vector<SpriteFrame*> frames;
    int currentFrameCount = 0;

    // Traverse the sprite sheet grid to extract the specified number of frames
    for (int currentRow = 0; currentRow < row; currentRow++)
    {
        for (int currentCol = 0; currentCol < col; currentCol++)
        {
            float x = currentCol * frameWidth;
            float y = currentRow * frameHeight;

            auto frame = SpriteFrame::create(fileName, Rect(x, y, frameWidth, frameHeight));
            frames.pushBack(frame);

            if (++currentFrameCount >= frameCount)
            {
                // Break both loops once the target frame count is reached
                currentRow = row;
                break;
            }
        }
    }
    return Animation::createWithSpriteFrames(frames, delay);
}

cocos2d::Animation* GameObject::initAnimate(const std::string& fileName, float frameWidth, float frameHeight,
    int row, int col, int startIndex, int endIndex, float delay)
{
    Vector<SpriteFrame*> frames;

    // Extract a linear range of frames based on grid indices
    for (int i = startIndex; i <= endIndex; ++i) {
        int currentRow = i / col;
        int currentCol = i % col;

        float x = currentCol * frameWidth;
        float y = currentRow * frameHeight;

        auto frame = SpriteFrame::create(fileName, Rect(x, y, frameWidth, frameHeight));
        frames.pushBack(frame);
    }
    return Animation::createWithSpriteFrames(frames, delay);
}

cocos2d::Animation* GameObject::initAnimateForCycle(const std::string& fileName, float frameWidth, float frameHeight,
    int row, int col, int startIndex, int totalFrameCount, float delay)
{
    Vector<SpriteFrame*> frames;

    // Create a circular sequence of frames starting from the given index
    for (int i = 0; i < totalFrameCount; ++i) {
        int index = (startIndex + i) % totalFrameCount;
        int currentRow = index / col;
        int currentCol = index % col;

        float x = currentCol * frameWidth;
        float y = currentRow * frameHeight;

        auto frame = SpriteFrame::create(fileName, Rect(x, y, frameWidth, frameHeight));
        frames.pushBack(frame);
    }
    return Animation::createWithSpriteFrames(frames, delay);
}