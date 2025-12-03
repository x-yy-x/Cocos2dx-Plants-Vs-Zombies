#pragma once
#include "cocos2d.h"
#include "GameObject.h"

class BackGround : public GameObject
{
public:
    virtual bool init() override;
    
    // Implement the "static create()" method manually
    CREATE_FUNC(BackGround);

protected:
    // BackGround class static constants
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
};
