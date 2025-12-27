#ifndef __ICE_TILE_H__
#define __ICE_TILE_H__

#include "cocos2d.h"
#include "GameDefs.h"

/**
 * @brief Represents an ice trail segment left by Zomboni units.
 * Prevents plants from being placed and acts as a environmental hazard.
 */
class IceTile : public cocos2d::Sprite
{
public:
    /**
     * @brief Creates a segment of the ice trail
     * @param worldPos Target world position
     * @param iceIndex Index used to determine the texture slice from the ice sheet
     * @return Pointer to the created IceTile instance
     */
    static IceTile* create(const cocos2d::Vec2& worldPos, int iceIndex);

    /**
     * @brief Per-frame logic update for expiration timing
     * @param dt Delta time since last frame
     */
    virtual void update(float dt) override;

    /**
     * @brief Returns whether the ice tile has reached its maximum lifespan
     */
    bool isExpired() const { return expired; }

    /**
     * @brief Calculates which grid row this tile occupies
     * @return Zero-based row index
     */
    int getRow();

    /**
     * @brief Manually flag the tile for removal
     */
    void markAsExpired();

private:
    float life_time = 0.0f; // Accumulated time since creation
    float max_life = 60.0f; // Maximum duration (seconds) before melting
    bool expired = false;   // Removal flag
};

#endif // __ICE_TILE_H__