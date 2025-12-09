#pragma once

#include "SeedPacket.h"
#include "ThreePeater.h"

/**
 * @brief Seed packet for ThreePeater
 */
class ThreePeaterSeedPacket : public SeedPacket
{
public:
    /**
     * @brief Initialization
     */
    virtual bool init() override;

    CREATE_FUNC(ThreePeaterSeedPacket);

    /**
     * @brief Plant a ThreePeater at given position
     */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) override;

    /**
     * @brief Create preview plant
     */
    virtual Plant* createPreviewPlant() override;

protected:
    ThreePeaterSeedPacket();
};

