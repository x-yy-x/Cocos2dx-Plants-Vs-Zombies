#pragma once
#include "cocos2d.h"

class Mower : public cocos2d::Sprite
{
public:
    static Mower* create();
    virtual bool init() override;

    void start();           // 开始向前移动
    bool isMoving() const { return _moving; }

private:
    bool _moving{false};
};
