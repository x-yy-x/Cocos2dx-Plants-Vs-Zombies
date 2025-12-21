#pragma once

#include "cocos2d.h"
#include "GameDefs.h"

class ShopScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;

    CREATE_FUNC(ShopScene);

private:
    void setupUI();
    void setupDaveAnimation();
    void setupShopItems();
    void createShopItem(const std::string& image, const std::string& name, int price, const cocos2d::Vec2& position, PlantName plantName = PlantName::UNKNOWN);
    void updateCoinLabel();

    // callbacks
    void onBackToMenu(cocos2d::Ref* sender);

    cocos2d::Label* _coinLabel{nullptr};
    cocos2d::Sprite* _daveSprite{nullptr};
};