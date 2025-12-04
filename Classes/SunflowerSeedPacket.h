#pragma once

#include "SeedPacket.h"
#include "Sunflower.h"

/**
 * @brief Seed packet for Sunflower
 */
class SunflowerSeedPacket : public SeedPacket
{
public:
    /**
     * @brief Initialization
     */
    virtual bool init() override;

    CREATE_FUNC(SunflowerSeedPacket);

    /**
     * @brief Plant a Sunflower at given position
     */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) override;

    /**
     * @brief Create preview plant
     */
    virtual Plant* createPreviewPlant() override;

protected:
    SunflowerSeedPacket();
};
