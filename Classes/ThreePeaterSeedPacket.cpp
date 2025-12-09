#include "ThreePeaterSeedPacket.h"

USING_NS_CC;

ThreePeaterSeedPacket::ThreePeaterSeedPacket()
{
    // TODO: Replace with actual "seedpacket_threepeater.png" when available
    _seedPacketImage = "seedpacket_threepeater(1).png";  // Temporary placeholder
    _cooldownTime = 3.0f;
    _sunCost = 325;
    CCLOG("Warning: Using placeholder image for ThreePeaterSeedPacket. Please add seedpacket_threepeater.png to Resources folder.");
}

bool ThreePeaterSeedPacket::init()
{
    if (!SeedPacket::init())
    {
        return false;
    }
    
    return true;
}

Plant* ThreePeaterSeedPacket::plantAt(const cocos2d::Vec2& globalPos)
{
    return ThreePeater::plantAtPosition(globalPos);
}

Plant* ThreePeaterSeedPacket::createPreviewPlant()
{
    auto preview = ThreePeater::create();
    if (preview)
    {
        preview->setOpacity(128); // 50% transparent
    }
    return preview;
}

