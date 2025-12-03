#pragma once
#include "cocos2d.h"
class Zombie : public cocos2d::Sprite
{
private:
    float _normalSpeed = 25.0f;  // 正常移动速度
    float _speed = 25.0f;        // 当前速度（吃的时候为0）
    bool _isEating = false;      // 状态机：是否正在咬植物

    cocos2d::Sprite* _targetPlant = nullptr;  // 当前正在吃的植物

    cocos2d::Action* _walkAction = nullptr;   // 行走动画 action
    cocos2d::Action* _eatAction = nullptr;    // 啃食动画 action

    std::vector<cocos2d::Sprite*> *_plants=nullptr;    // 植物列表

public:
    // 创建一个 zombie 的静态函数
    static Zombie* createZombie();

    void update(float dt);

    // 设置植物列表（外部传入）
    void setPlantList(std::vector<cocos2d::Sprite*>*p);
    virtual bool init() override;
    ~Zombie();
private:
    // 初始化动画
    void initWalkAnimation();
    void initEatAnimation();

    // 碰撞检查
    void checkCollision();

    // 启动吃植物
    void startEating(cocos2d::Sprite* plant);

    // 植物被吃死
    void onPlantDied();
};
