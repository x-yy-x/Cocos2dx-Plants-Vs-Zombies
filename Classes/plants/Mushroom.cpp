#include "Mushroom.h"

USING_NS_CC;

Mushroom::Mushroom()
    : is_night_mode(false)
    , is_initialized(false)
    , activity_state(ActivityState::ACTIVE)
    , current_animation(nullptr)
{
}

Mushroom::~Mushroom()
{
    // Animations are handled by Cocos2d-x reference counting or auto-cleanup
}

bool Mushroom::isDaytime() const
{
    return !is_night_mode;
}

bool Mushroom::checkDayNightChange()
{
    // Retrieve the GameWorld context from the running scene
    Scene* runningScene = Director::getInstance()->getRunningScene();
    auto gameWorld = dynamic_cast<GameWorld*>(runningScene);

    if (gameWorld)
    {
        bool newMode = gameWorld->isNightMode();
        bool changed = (newMode != is_night_mode);
        is_night_mode = newMode;

        // Force a "change" event during the first initialization frame
        if (!is_initialized)
        {
            is_initialized = true;
            CCLOG("Mushroom logic synchronized. Mode: %s", is_night_mode ? "NIGHT" : "DAY");
            return true;
        }

        if (changed)
        {
            CCLOG("Mushroom environment shifted to: %s", is_night_mode ? "NIGHT" : "DAY");
        }

        return changed;
    }

    return false;
}

Animation* Mushroom::loadAnimation(const std::string& folderPath, int frameCount, float scale,
    float objectW, float objectH,
    float offsetX, float offsetY, float cropWidth, float cropHeight)
{
    Vector<SpriteFrame*> frames;

    // Determine the clipping rectangle; fallback to object size if no custom crop is provided
    float actualCropW = (cropWidth < 0) ? objectW : cropWidth;
    float actualCropH = (cropHeight < 0) ? objectH : cropHeight;

    for (int i = 1; i <= frameCount; ++i)
    {
        // Expects file naming convention: "folder/1 (1).png", "folder/1 (2).png", etc.
        std::string filename = StringUtils::format("%s/1 (%d).png", folderPath.c_str(), i);

        auto frame = SpriteFrame::create(filename, Rect(offsetX, offsetY, actualCropW, actualCropH));

        if (frame)
        {
            frames.pushBack(frame);
        }
        else
        {
            CCLOG("Warning: Mushroom failed to load animation frame: %s", filename.c_str());
        }
    }

    if (!frames.empty())
    {
        // Default frame rate of 10 FPS (0.1s per frame)
        return Animation::createWithSpriteFrames(frames, 0.1f);
    }

    return nullptr;
}