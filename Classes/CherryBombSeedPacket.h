#pragma once

#include "SeedPacket.h"
#include "CherryBomb.h"

/**
 * @brief Seed packet for CherryBomb
 */
class CherryBombSeedPacket : public SeedPacket
{
public:
    /**
     * @brief Initialization
     */
    virtual bool init() override;

    CREATE_FUNC(CherryBombSeedPacket);

    /**
     * @brief Plant a CherryBomb at given position
     */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) override;

    /**
     * @brief Create preview plant (static image, no animation)
     */
    virtual Plant* createPreviewPlant() override;

protected:
    CherryBombSeedPacket();
};

