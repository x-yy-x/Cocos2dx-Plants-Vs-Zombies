#pragma once

#include "cocos2d.h"
#include "GameDefs.h"

/**
 * @brief Base class for all interactive objects displayed on screen.
 * Inherits from cocos2d::Sprite to provide common game logic and rendering.
 * Can be extended with update logic (override update function if needed).
 */
class GameObject : public cocos2d::Sprite
{
public:
    // Virtual destructor to ensure proper cleanup in derived classes
    virtual ~GameObject();


    virtual bool init() { return true; }

    cocos2d::Animation* initAnimateForCycle(const std::string& fileName, float frameWidth, float frameHeight,
        int row, int col, int startIndex, int totoalFrameCount, float delay);

    cocos2d::Animation* initAnimate(const std::string& fileName, float frameWidth, float frameHeight,
        int row, int col, int startIndex, int endIndex, float delay);

    cocos2d::Animation* initAnimate(const std::string& fileName, float frameWidth, float frameHeight,
        int row, int col, int frameCount, float delay);


protected:
    // Protected constructor to prevent direct instantiation (use CREATE_FUNC macro)
    GameObject();

};
