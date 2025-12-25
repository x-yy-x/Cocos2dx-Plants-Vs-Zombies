#pragma once

#include "cocos2d.h"
#include "SunProducingPlant.h" // 包含 virtual public Plant
#include "Mushroom.h"          // 包含 virtual public Plant
#include "GameDefs.h"

class Sunshroom : public SunProducingPlant, public Mushroom
{
public:
    // 阳光菇特有的生长阶段
    enum class GrowthState
    {
        SMALL_INIT,     // 刚种下，小个子
        GROWING,        // 正在长大
        GROWN,          // 长大了
        SLEEPING        // 睡觉中（此状态与 Mushroom::_activityState 联动）
    };

    // 构造/析构
    Sunshroom();
    virtual ~Sunshroom() {}

    // 标准 Cocos create 函数
    CREATE_FUNC(Sunshroom);

    virtual bool init() override;
    virtual void update(float delta) override;

    // 实现 SunProducingPlant 接口
    virtual std::vector<Sun*> produceSun() override;

    // 静态种植函数
    static Sunshroom* plantAtPosition(const cocos2d::Vec2& globalPos);

protected:
    // 实现 Mushroom 的虚函数
    virtual void wakeUp() override;
    virtual void sleep() override;

    // 内部逻辑
    void setGrowthState(GrowthState state);
    void setAnimation() override; // 覆盖 Plant 的 setAnimation
    void startGrowingSequence();
    void onGrowthSequenceFinished();

    // 静态常量
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float SUN_PRODUCTION_INTERVAL;
    static const float GROWTH_TIME;
    static const float SMALL_SCALE;
    static const float GROWN_SCALE;
    static const int SMALL_SUN_VALUE;
    static const int GROWN_SUN_VALUE;

private:
    GrowthState _growthState;
    float _growthTimer;
    float _currentScale;
};