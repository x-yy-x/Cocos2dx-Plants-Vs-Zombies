#include "IceTile.h"

USING_NS_CC;

// Constants for texture slicing and positioning
static const float ICE_STEP = 10.0f;   // Horizontal width of one ice segment slice
static const float ICE_HEIGHT = 90.0f; // Height of the ice texture
static const float ICE_LENGTH = 150.0f;// Total length of the original ice texture

IceTile* IceTile::create(const Vec2& worldPos, int iceIndex)
{
    // Calculate the rectangle slice to create a continuous trail effect
    float rectX = ICE_LENGTH - (iceIndex + 1) * ICE_STEP;
    Rect iceRect(rectX, 0, ICE_STEP, ICE_HEIGHT);

    auto ice = new (std::nothrow) IceTile();
    if (!ice || !ice->initWithFile("ice.png", iceRect))
    {
        CC_SAFE_DELETE(ice);
        return nullptr;
    }

    // Adjust visual scaling and apply offset for proper grid alignment
    ice->setScale(1.1f);
    ice->setPosition(worldPos + Vec2(0, -30));

    // Ensure the instance is managed by Cocos2d-x autorelease pool
    ice->autorelease();

    return ice;
}

void IceTile::update(float dt)
{
    life_time += dt;
    if (life_time >= max_life)
    {
        expired = true;
    }
}

int IceTile::getRow()
{
    // Coordinate conversion to determine row based on global game definitions
    int row = static_cast<int>((this->getPositionY() + 30 - GRID_ORIGIN.y - CELLSIZE.height * 0.7f) / CELLSIZE.height);

    // Log for debugging grid placement
    CCLOG("Ice tile identified in row: %d", row);
    return row;
}

void IceTile::markAsExpired()
{
    this->expired = true;
}