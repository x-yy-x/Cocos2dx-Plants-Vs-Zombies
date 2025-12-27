#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "cocos2d.h"
#include "GameDefs.h"

/**
 * @brief Base class for all interactive objects displayed on screen.
 * Inherits from cocos2d::Sprite to provide common game logic and rendering.
 * Provides helper methods for sprite sheet animation slicing.
 */
class GameObject : public cocos2d::Sprite
{
public:
    /**
     * @brief Virtual destructor to ensure proper memory cleanup in derived classes
     */
    virtual ~GameObject();

    /**
     * @brief Standard Cocos2d-x initialization
     * @return true if successful
     */
    virtual bool init() override;

protected:
    /**
     * @brief Creates an animation by cycling through frames starting from a specific index
     * @param fileName Path to the sprite sheet texture
     * @param frameWidth Width of a single frame
     * @param frameHeight Height of a single frame
     * @param row Total rows in the sprite sheet
     * @param col Total columns in the sprite sheet
     * @param startIndex The starting frame index for the cycle
     * @param totalFrameCount Total number of frames to include in the cycle
     * @param delay Time interval between frames
     * @return Pointer to the created cocos2d::Animation
     */
    cocos2d::Animation* initAnimateForCycle(const std::string& fileName, float frameWidth, float frameHeight,
        int row, int col, int startIndex, int totalFrameCount, float delay);

    /**
     * @brief Creates an animation using a specific range of frames
     * @param startIndex Index of the first frame
     * @param endIndex Index of the last frame
     */
    cocos2d::Animation* initAnimate(const std::string& fileName, float frameWidth, float frameHeight,
        int row, int col, int startIndex, int endIndex, float delay);

    /**
     * @brief Creates an animation using a total count of frames starting from the top-left (0,0)
     * @param frameCount Number of frames to extract
     */
    cocos2d::Animation* initAnimate(const std::string& fileName, float frameWidth, float frameHeight,
        int row, int col, int frameCount, float delay);

    /**
     * @brief Protected constructor to enforce use of create() methods
     */
    GameObject();
};

#endif // __GAME_OBJECT_H__