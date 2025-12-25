#pragma once

#include "cocos2d.h"
#include "AttackingPlant.h" // 包含 virtual public Plant
#include "Mushroom.h"       // 包含 virtual public Plant
#include "Zombie.h"

class Puffshroom : public AttackingPlant, public Mushroom
{
public:
    CREATE_FUNC(Puffshroom);

    Puffshroom();
    virtual ~Puffshroom() {}

    virtual bool init() override;
    virtual void update(float delta) override;

    // 实现 AttackingPlant 接口
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;

    static Puffshroom* plantAtPosition(const cocos2d::Vec2& globalPos);

protected:
    // 实现 Mushroom 接口
    virtual void wakeUp() override;
    virtual void sleep() override;
    virtual void setAnimation() override;

    // 静态常量
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_COOLDOWN;
    static const int DETECTION_RANGE;
};