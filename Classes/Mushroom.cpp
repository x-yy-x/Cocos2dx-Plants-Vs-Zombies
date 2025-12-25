#include "Mushroom.h"

USING_NS_CC;

Mushroom::Mushroom()
    : _isNightMode(false)
    , _isInitialized(false)
    , _activityState(ActivityState::ACTIVE)
    , _currentAnimation(nullptr)
{
}

Mushroom::~Mushroom()
{
}

bool Mushroom::isDaytime() const
{
    return !_isNightMode;
}

bool Mushroom::checkDayNightChange()
{
    Scene* runningScene = Director::getInstance()->getRunningScene();
    auto gameWorld = dynamic_cast<GameWorld*>(runningScene);

    if (gameWorld)
    {
        bool newMode = gameWorld->isNightMode();
        bool changed = (newMode != _isNightMode);
        _isNightMode = newMode;

        // 如果是第一帧初始化
        if (!_isInitialized)
        {
            _isInitialized = true;
            CCLOG("Mushroom Initialized. Mode: %s", _isNightMode ? "NIGHT" : "DAY");
            // 返回 true 以便子类在第一帧就能正确设置状态
            return true;
        }

        if (changed)
        {
            CCLOG("Mushroom environment changed to: %s", _isNightMode ? "NIGHT" : "DAY");
        }

        return changed;
    }

    // 如果找不到 GameWorld，不做改变
    return false;
}

Animation* Mushroom::loadAnimation(const std::string& folderPath, int frameCount, float scale,
    float objectW, float objectH,
    float offsetX, float offsetY, float cropWidth, float cropHeight)
{
    Vector<SpriteFrame*> frames;

    // 如果未指定裁剪宽高，使用对象默认宽高
    float actualCropW = (cropWidth < 0) ? objectW : cropWidth;
    float actualCropH = (cropHeight < 0) ? objectH : cropHeight;

    for (int i = 1; i <= frameCount; ++i)
    {
        std::string filename = StringUtils::format("%s/1 (%d).png", folderPath.c_str(), i);

        // 创建 SpriteFrame
        auto frame = SpriteFrame::create(filename, Rect(offsetX, offsetY, actualCropW, actualCropH));

        if (frame)
        {
            frames.pushBack(frame);
        }
        else
        {
            CCLOG("Mushroom failed to load frame: %s", filename.c_str());
        }
    }

    if (!frames.empty())
    {
        return Animation::createWithSpriteFrames(frames, 0.1f); // 0.1秒一帧
    }

    return nullptr;
}