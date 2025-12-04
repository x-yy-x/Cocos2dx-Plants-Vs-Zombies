#pragma once

#include "SeedPacket.h"
#include "Wallnut.h"

/**
 * @brief Seed packet for Wallnut
 */
class WallnutSeedPacket : public SeedPacket
{
public:
    /**
     * @brief Initialization
     */
    virtual bool init() override;

    CREATE_FUNC(WallnutSeedPacket);

    /**
     * @brief Plant a Wallnut at given position
     */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) override;

    /**
     * @brief Create preview plant
     */
    virtual Plant* createPreviewPlant() override;

protected:
    WallnutSeedPacket();
};
