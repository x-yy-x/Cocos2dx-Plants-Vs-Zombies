#pragma once

#include "SeedPacket.h"
#include "Repeater.h"

/**
 * @brief Seed packet for Repeater
 */
class RepeaterSeedPacket : public SeedPacket
{
public:
    /**
     * @brief Initialization
     */
    virtual bool init() override;

    CREATE_FUNC(RepeaterSeedPacket);

    /**
     * @brief Plant a Repeater at given position
     */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) override;

    /**
     * @brief Create preview plant
     */
    virtual Plant* createPreviewPlant() override;

protected:
    RepeaterSeedPacket();
};

