#include "PeaShooterSeedPacket.h"

USING_NS_CC;

PeaShooterSeedPacket::PeaShooterSeedPacket()
{
    _seedPacketImage = "seedpacket_peashooter.png";
    _cooldownTime = 7.5f;
    _sunCost = 100;
}

bool PeaShooterSeedPacket::init()
{
    if (!SeedPacket::init())
    {
        return false;
    }

    return true;
}

Plant* PeaShooterSeedPacket::plantAt(const cocos2d::Vec2& globalPos)
{
    return PeaShooter::plantAtPosition(globalPos);
}

Plant* PeaShooterSeedPacket::createPreviewPlant()
{
    auto preview = PeaShooter::create();
    if (preview)
    {
        preview->setOpacity(128); // 50% transparent
    }
    return preview;
}
