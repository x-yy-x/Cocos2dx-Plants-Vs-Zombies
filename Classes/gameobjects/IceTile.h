#pragma once

#include "cocos2d.h"
#include "GameDefs.h"

class IceTile :public cocos2d::Sprite
{
public:
    static IceTile* create(const cocos2d::Vec2& worldPos, int iceIndex);

    virtual void update(float dt) override;

    bool isExpired() const { return _expired; }

    int getRow();

    void markAsExpired();

private:
    float _lifeTime = 0.0f;
    float _maxLife = 60.0f;
    bool _expired = false;
};