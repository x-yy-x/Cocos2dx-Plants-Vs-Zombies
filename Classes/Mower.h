#pragma once
#include "cocos2d.h"
#include "GameObject.h"

class Mower : public GameObject
{
public:
    static Mower* create();
    virtual bool init() override;

    void start();           // Start moving forward
    bool isMoving() const { return _moving; }

private:
    bool _moving{false};
};
