#include "RepeaterSeedPacket.h"

USING_NS_CC;

RepeaterSeedPacket::RepeaterSeedPacket()
{
    _seedPacketImage = "seedpacket_repeater.png";
    _cooldownTime = 3.0f;  // Same cooldown as PeaShooter
    _sunCost = 200;        // Costs 200 sun
}

bool RepeaterSeedPacket::init()
{
    if (!SeedPacket::init())
    {
        return false;
    }

    return true;
}

Plant* RepeaterSeedPacket::plantAt(const cocos2d::Vec2& globalPos)
{
    return Repeater::plantAtPosition(globalPos);
}

Plant* RepeaterSeedPacket::createPreviewPlant()
{
    auto preview = Repeater::create();
    if (preview)
    {
        preview->setOpacity(128); // 50% transparent
    }
    return preview;
}

