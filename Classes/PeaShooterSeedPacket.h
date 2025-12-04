#pragma once

#include "SeedPacket.h"
#include "PeaShooter.h"

/**
 * @brief Seed packet for PeaShooter
 */
class PeaShooterSeedPacket : public SeedPacket
{
public:
    /**
     * @brief Initialization
     */
    virtual bool init() override;

    CREATE_FUNC(PeaShooterSeedPacket);

    /**
     * @brief Plant a PeaShooter at given position
     */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) override;

    /**
     * @brief Create preview plant
     */
    virtual Plant* createPreviewPlant() override;

protected:
    PeaShooterSeedPacket();
};
