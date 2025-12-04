#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"

// Forward declaration
class Plant;

/**
 * @brief SeedPacket base class for plant seed cards
 * Manages cooldown, sun cost, and plant creation
 */
class SeedPacket : public GameObject
{
public:
    /**
     * @brief Initialization function
     */
    virtual bool init() override;

    /**
     * @brief Update function for cooldown animation
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Check if the seed packet is ready (not on cooldown)
     * @return true if ready, false if cooling down
     */
    bool isReady() const;

    /**
     * @brief Start cooldown
     */
    void startCooldown();

    /**
     * @brief Get sun cost
     * @return Sun cost value
     */
    int getSunCost() const;

    /**
     * @brief Create and return a plant at given position (to be overridden by subclasses)
     * @param globalPos Position to plant
     * @return Pointer to created plant, or nullptr if failed
     */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) = 0;

    /**
     * @brief Create a preview plant (transparent, non-interactive)
     * @return Pointer to preview plant
     */
    virtual Plant* createPreviewPlant() = 0;

protected:
    // Protected constructor
    SeedPacket();

    // Virtual destructor
    virtual ~SeedPacket();

    /**
     * @brief Update cooldown visual effect (grayscale to color)
     */
    void updateCooldownEffect();

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    float _cooldownTime;         // Total cooldown time in seconds
    float _accumulatedTime;      // Time accumulated since cooldown started
    int _sunCost;                // Sun cost to plant
    bool _isOnCooldown;          // Is currently cooling down

    // Image filename (to be set by subclasses)
    std::string _seedPacketImage;
};
