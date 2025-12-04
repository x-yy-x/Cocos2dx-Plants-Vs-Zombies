#include "SunflowerSeedPacket.h"

USING_NS_CC;

SunflowerSeedPacket::SunflowerSeedPacket()
{
    _seedPacketImage = "seedpacket_sunflower.png";
    _cooldownTime = 3.0f;
    _sunCost = 50;
}

bool SunflowerSeedPacket::init()
{
    if (!SeedPacket::init())
    {
        return false;
    }

    return true;
}

Plant* SunflowerSeedPacket::plantAt(const cocos2d::Vec2& globalPos)
{
    return Sunflower::plantAtPosition(globalPos);
}

Plant* SunflowerSeedPacket::createPreviewPlant()
{
    auto preview = Sunflower::create();
    if (preview)
    {
        preview->setOpacity(128); // 50% transparent
    }
    return preview;
}
