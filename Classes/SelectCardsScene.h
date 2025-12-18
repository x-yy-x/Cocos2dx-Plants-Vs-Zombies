#pragma once
#include "cocos2d.h"

class SelectCardsScene : public cocos2d::Scene
{
private:
    bool _isNightMode;
    cocos2d::Node* _worldLayer;
    cocos2d::Node* _uiLayer;
    cocos2d::Sprite* _background;
    cocos2d::Menu* _backMenu;
    cocos2d::Sprite* _selectBG;
    cocos2d::Node* _zombieShowLayer;
    float _moveDuration;
    float _moveOffsetX;
    float _selectBGPosXRatio;
    float _selectBGPosYRatio;
    float _selectBGScale;
    float _zShowStartX;
    float _zShowStartY;
    float _zShowGapY;

public:
    static SelectCardsScene* createScene(bool isNightMode);
    static SelectCardsScene* create(bool isNightMode);
    virtual bool init() override;
    void setMoveDuration(float d);
    void setMoveOffsetX(float x);
    void setSelectBGPosRatio(float xr, float yr);
    void setSelectBGScale(float s);

protected:
    SelectCardsScene();
    virtual ~SelectCardsScene();
    void buildWorld();
    void buildUI();
    void runIntroMove();
    void showSelectBG();
    void spawnZombieShowcase();
};
