#include "WallnutSeedPacket.h"

USING_NS_CC;

WallnutSeedPacket::WallnutSeedPacket()
{
    _seedPacketImage = "seedpacket_wallnut.png";
    _cooldownTime = 30.0f;
    _sunCost = 50;
}

bool WallnutSeedPacket::init()
{
    if (!SeedPacket::init())
    {
        return false;
    }

    return true;
}

Plant* WallnutSeedPacket::plantAt(const cocos2d::Vec2& globalPos)
{
    return Wallnut::plantAtPosition(globalPos);
}

Plant* WallnutSeedPacket::createPreviewPlant()
{
    auto preview = Wallnut::create();
    if (preview)
    {
        preview->setOpacity(128); // 50% transparent
    }
    return preview;
}
