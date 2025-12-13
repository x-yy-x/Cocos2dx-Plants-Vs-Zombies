#pragma once
#include "cocos2d.h"
#include "GameObject.h"

class BackGround : public GameObject
{
public:
    static BackGround* create();
    static BackGround* create(bool isNightMode);

    virtual bool init() override;

    void setNightMode(bool isNightMode)
    {
        _isNightMode = isNightMode;
    }

protected:
    static const std::string DAY_IMAGE;
    static const std::string NIGHT_IMAGE;

private:
    bool _isNightMode{ false };
};
