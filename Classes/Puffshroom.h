#pragma once

#include "cocos2d.h"
#include "AttackingPlant.h"
#include "GameDefs.h"
#include "Zombie.h"

// Forward declaration
class Puff;

class Puffshroom : public AttackingPlant
{
public:
    /**
     * @brief Puffshroom activity states
     */
    enum class ActivityState
    {
        ACTIVE,     // Active state (night) - playing init animation
        SLEEPING    // Sleeping state (day) - playing sleep animation
    };

    /**
     * @brief Puffshroom initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static Puffshroom* create() function
    CREATE_FUNC(Puffshroom);

    /**
     * @brief Static planting function for Puffshroom.
     * @param globalPos Touch position in global coordinates
     * @return Puffshroom* Returns Puffshroom instance on success, nullptr on failure
     */
    static Puffshroom* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Check for zombies in range and attack if possible (override from AttackingPlant)
     * @param allZombiesInRow All zombies in each row (array of 5 vectors)
     * @param plantRow The row this plant is in
     * @return std::vector<Bullet*> Returns vector of bullets created (can be empty, single, or multiple)
     */
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;

    /**
     * @brief Check if it's daytime (sleeping state)
     * @return true if daytime, false if nighttime
     */
    bool isDaytime() const;

    /**
     * @brief Update day/night mode from GameWorld
     * @return true if mode changed, false otherwise
     */
    bool updateDayNightMode();

    /**
     * @brief Set the current activity state and update animation accordingly
     * @param state New activity state
     */
    void setActivityState(ActivityState state);

    /**
     * @brief Get current activity state
     * @return Current activity state
     */
    ActivityState getActivityState() const { return _activityState; }

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_COOLDOWN;     // Time between attacks
    static const int DETECTION_RANGE;       // Detection range in grid cells (3 cells)

    /**
     * @brief Set up animation frames based on current state
     */
    virtual void setAnimation() override;

    /**
     * @brief Load animation frames from specified folder and filename pattern
     * @param folderPath Folder path (e.g., "puffshroom/init")
     * @param frameCount Number of frames to load
     * @param scale Scale factor for the animation
     * @param offsetX X offset for cropping rectangle (default: 0)
     * @param offsetY Y offset for cropping rectangle (default: 0)
     * @param cropWidth Width of cropping rectangle (default: OBJECT_SIZE.width)
     * @param cropHeight Height of cropping rectangle (default: OBJECT_SIZE.height)
     * @return Animation* Created animation, nullptr if failed
     */
    cocos2d::Animation* loadAnimation(const std::string& folderPath, int frameCount, float scale = 1.0f,
                                     float offsetX = 0.0f, float offsetY = 0.0f,
                                     float cropWidth = -1.0f, float cropHeight = -1.0f);

    /**
     * @brief Wake up the Puffshroom (transition to active state)
     */
    void wakeUp();

    /**
     * @brief Put the Puffshroom to sleep (daytime)
     */
    void sleep();

    // Protected constructor
    Puffshroom();

private:
    ActivityState _activityState;           // Current activity state
    bool _isNightMode;                      // Current day/night mode state
    bool _isInitialized;                    // Flag to track if initial state has been set
    cocos2d::Action* _currentAnimation;     // Current animation action
};
