#include "IceTile.h"

USING_NS_CC;

static const float ICE_STEP = 10.0f;
static const float ICE_HEIGHT = 90.0f;
static const float ICE_LENGTH = 150.0f;

IceTile* IceTile::create(const Vec2& worldPos, int iceIndex)
{
    float rectX = ICE_LENGTH - (iceIndex + 1) * ICE_STEP;
    Rect iceRect(rectX, 0, ICE_STEP, ICE_HEIGHT);

    auto ice = new (std::nothrow) IceTile();
    if (!ice || !ice->initWithFile("ice.png", iceRect))
    {
        CC_SAFE_DELETE(ice);
        return nullptr;
    }


    ice->setScale(1.1f);
    ice->setPosition(worldPos + Vec2(0, -30));

    return ice;
}

void IceTile::update(float dt)
{
    _lifeTime += dt;
    if (_lifeTime >= _maxLife)
    {
        _expired = true;
    }
}

int IceTile::getRow()
{
    int row = static_cast<int>((this->getPositionY() + 30 - GRID_ORIGIN.y - CELLSIZE.height * 0.7f) / CELLSIZE.height);
    CCLOG("current ice row=%d", row);
    return row;
}

void IceTile::markAsExpired()
{
    this->_expired = true;
}