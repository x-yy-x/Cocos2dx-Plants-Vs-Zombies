#include "GameObject.h"

USING_NS_CC;


// Protected constructor implementation
GameObject::GameObject()
{
    CCLOG("GameObject created.");
    // Initialize any GameObject-specific state here
}

// Destructor implementation
GameObject::~GameObject()
{
    CCLOG("GameObject destroyed.");
}



cocos2d::Animation* GameObject::initAnimate(const std::string& fileName, float frameWidth, float frameHeight,
    int row, int col, int frameCount, float delay)
{
    Vector<SpriteFrame*> frames;
    int currentFrameCount = 0;
    for (int currentRow = 0; currentRow < row; currentRow++)
    {
        for (int currentCol = 0; currentCol < col; currentCol++)
        {
            float x = currentCol * frameWidth;
            float y = currentRow * frameHeight;
            auto frame = SpriteFrame::create(fileName, Rect(x, y, frameWidth, frameHeight));
            frames.pushBack(frame);
            if (++currentFrameCount >= frameCount)
                break;
        }
    }
    return  Animation::createWithSpriteFrames(frames, delay);
}

cocos2d::Animation* GameObject::initAnimate(const std::string& fileName, float frameWidth, float frameHeight,
    int row, int col, int startIndex, int endIndex, float delay)
{
    Vector<SpriteFrame*> frames;
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
    int row, int col, int startIndex, int totoalFrameCount, float delay)
{
    Vector<SpriteFrame*> frames;
    for (int i = 0; i < totoalFrameCount; ++i) {
        int index = (startIndex + i) % totoalFrameCount;
        int currentRow = index / col;
        int currentCol = index % col;
        float x = currentCol * frameWidth;
        float y = currentRow * frameHeight;
        auto frame = SpriteFrame::create(fileName, Rect(x, y, frameWidth, frameHeight));
        frames.pushBack(frame);
    }
    return Animation::createWithSpriteFrames(frames, delay);
}