#pragma once
#include "cocos2d.h"
class Zombie;

class Rake : public cocos2d::Sprite
{
public:
    static Rake* create();
    bool init() override;

    // 激活（与僵尸碰撞时调用）
    void trigger(Zombie* z);

    bool isUsed() const { return _used; }

    virtual cocos2d::Rect getBoundingBox() const override;
private:
    bool _used{false};
};
