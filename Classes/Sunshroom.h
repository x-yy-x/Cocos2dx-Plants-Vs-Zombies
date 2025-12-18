#pragma once

#include "cocos2d.h"
#include "SunProducingPlant.h"
#include "GameDefs.h"

// Forward declaration
class Sun;

class Sunshroom : public SunProducingPlant
{
public:
    // Sunshroom growth states
    enum class GrowthState
    {
        SMALL_INIT,     // Just planted, small size, playing init animation
        GROWING,        // Growing animation playing
        GROWN,          // Fully grown, normal size
        SLEEPING        // Daytime sleeping state
    };

    /**
     * @brief Sunshroom initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static Sunshroom* create() function
    CREATE_FUNC(Sunshroom);

    /**
     * @brief Static planting function for Sunshroom.
     * @param globalPos Touch position in global coordinates
     * @return Sunshroom* Returns Sunshroom instance on success, nullptr on failure
     */
    static Sunshroom* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Produce sun if cooldown is finished (override from SunProducingPlant)
     * @return Sun* Returns Sun instance if ready, nullptr otherwise
     */
    virtual std::vector<Sun*> produceSun() override;

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
     * @brief Set the current growth state and update animation accordingly
     * @param state New growth state
     */
    void setGrowthState(GrowthState state);

    /**
     * @brief Get current growth state
     * @return Current growth state
     */
    GrowthState getGrowthState() const { return _growthState; }

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float SUN_PRODUCTION_INTERVAL;  // Time between sun productions (24 seconds)
    static const float GROWTH_TIME;             // Time to grow from small to grown (5 seconds)
    static const float SMALL_SCALE;             // Scale factor for small Sunshroom (0.7f)
    static const float GROWN_SCALE;             // Scale factor for grown Sunshroom (1.0f)
    static const int SMALL_SUN_VALUE;           // Sun value for small Sunshroom (15)
    static const int GROWN_SUN_VALUE;           // Sun value for grown Sunshroom (25)

    /**
     * @brief Set up animation frames based on current state
     */
    virtual void setAnimation() override;

    /**
     * @brief Load animation frames from specified folder and filename pattern
     * @param folderPath Folder path (e.g., "sunshroom/init")
     * @param frameCount Number of frames to load
     * @param scale Scale factor for the animation
     * @return Animation* Created animation, nullptr if failed
     */
    cocos2d::Animation* loadAnimation(const std::string& folderPath, int frameCount, float scale = 1.0f);

    /**
     * @brief Wake up the Sunshroom (transition from sleeping to appropriate night state)
     */
    void wakeUp();

    /**
     * @brief Put the Sunshroom to sleep (daytime)
     */
    void sleep();

    /**
     * @brief Start the three-stage growing sequence
     * Stage 1: Scale up animation (0.7 -> 1.0)
     * Stage 2: Play grownup animation frames
     * Stage 3: Switch to GROWN state
     */
    void startGrowingSequence();

    // Protected constructor
    Sunshroom();

    void onGrowthSequenceFinished(); // <-- ¡¾ÐÂÔöÉùÃ÷¡¿
private:
    GrowthState _growthState;        // Current growth state
    float _growthTimer;             // Timer for growth progression
    float _currentScale;            // Current scale factor
    cocos2d::Action* _currentAnimation;  // Current animation action
    bool _isNightMode;              // Current day/night mode state
    bool _isInitialized;            // Flag to track if initial state has been set
};
