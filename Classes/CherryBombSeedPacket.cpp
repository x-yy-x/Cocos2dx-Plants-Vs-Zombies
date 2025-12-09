#include "CherryBombSeedPacket.h"

USING_NS_CC;

CherryBombSeedPacket::CherryBombSeedPacket()
{
    _seedPacketImage = "seedpacket_cherry_bomb.png";
    _cooldownTime = 30.0f;  // Same as Wallnut (5 seconds)
    _sunCost = 150;         // Cherry bomb costs 150 sun
}

bool CherryBombSeedPacket::init()
{
    if (!SeedPacket::init())
    {
        return false;
    }

    return true;
}

Plant* CherryBombSeedPacket::plantAt(const cocos2d::Vec2& globalPos)
{
    return CherryBomb::plantAtPosition(globalPos);
}

Plant* CherryBombSeedPacket::createPreviewPlant()
{
    // Create a static sprite for preview (no animation)
    auto preview = Sprite::create("cherry_bomb_spritesheet.png", Rect(0, 0, 128, 128));
    if (preview)
    {
        preview->setOpacity(128); // 50% transparent
    }
    
    // We return a Sprite* but the caller expects Plant*
    // So we need to cast or create a dummy CherryBomb
    // Let's create a CherryBomb but stop its animation
    auto cherryPreview = CherryBomb::create();
    if (cherryPreview)
    {
        cherryPreview->setOpacity(128);
        cherryPreview->stopAllActions();  // Stop the idle animation
    }
    return cherryPreview;
}

