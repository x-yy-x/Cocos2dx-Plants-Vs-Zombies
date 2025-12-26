#pragma once

#include "cocos2d.h"
#include "AttackingPlant.h" // ���� virtual public Plant
#include "Mushroom.h"       // ���� virtual public Plant
#include "Zombie.h"

class Puffshroom : public AttackingPlant, public Mushroom
{
public:
    CREATE_FUNC(Puffshroom);

    Puffshroom();
    virtual ~Puffshroom() {}

    virtual bool init() override;
    virtual void update(float delta) override;

    // ʵ�� AttackingPlant �ӿ�
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;
    
    // Explicit override to avoid C4250 inheritance dominance warning
    virtual PlantCategory getCategory() const override { return PlantCategory::ATTACKING; }

    static Puffshroom* plantAtPosition(const cocos2d::Vec2& globalPos);

protected:
    // ʵ�� Mushroom �ӿ�
    virtual void wakeUp() override;
    virtual void sleep() override;
    virtual void setAnimation() override;

    // ��̬����
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_COOLDOWN;
    static const int DETECTION_RANGE;
};