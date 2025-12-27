#ifndef __MUSHROOM_H__
#define __MUSHROOM_H__

#include "Plant.h"
#include "GameWorld.h"
#include "cocos2d.h"

/**
 * @brief Abstract base class for all Mushroom-type plants.
 * Uses virtual inheritance from Plant to resolve potential diamond inheritance issues.
 * Mushrooms have a unique "Sleep" mechanic where they become inactive during daytime
 * unless awakened by a Coffee Bean.
 */
class Mushroom : virtual public Plant
{
public:
    /** @brief Represents the physiological state of the mushroom. */
    enum class ActivityState
    {
        ACTIVE,     // Fully functional (Nighttime or awakened)
        SLEEPING    // Inactive, grayed out animation (Daytime)
    };

protected:
    Mushroom();
    virtual ~Mushroom();

    // ----------------------------------------------------
    // State Variables
    // ----------------------------------------------------
    bool is_night_mode;                 // Tracks if the current level environment is night
    bool is_initialized;                // Ensures first-frame state synchronization
    ActivityState activity_state;       // Current operational status
    cocos2d::Action* current_animation; // Pointer to the currently playing animation action

    // ----------------------------------------------------
    // Core Shared Functionality
    // ----------------------------------------------------

    /**
     * @brief Queries the GameWorld to synchronize with the current day/night cycle.
     * @return true if the environment changed (e.g., transition or first-frame setup).
     */
    bool checkDayNightChange();

    /** @brief Returns true if the current environment is daytime. */
    bool isDaytime() const;

    /**
     * @brief Utility function to load multi-frame animations from a directory.
     * @param folderPath Path to the folder containing numbered .png files.
     * @param frameCount Number of frames to load.
     * @param scale Visual scale factor.
     * @param objectW/objectH Base dimensions of the mushroom.
     * @param offsetX/offsetY Rect offsets for sprite sheet slicing.
     * @return A pointer to the created Animation object, or nullptr on failure.
     */
    cocos2d::Animation* loadAnimation(const std::string& folderPath,
        int frameCount,
        float scale,
        float objectW, float objectH,
        float offsetX = 0.0f,
        float offsetY = 0.0f,
        float cropWidth = -1.0f,
        float cropHeight = -1.0f);

    /** @brief Triggered when the mushroom enters an active state. */
    virtual void wakeUp() = 0;

    /** @brief Triggered when the mushroom falls asleep (e.g., planted in day). */
    virtual void sleep() = 0;
};

#endif // __MUSHROOM_H__