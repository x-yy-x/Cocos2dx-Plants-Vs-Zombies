#pragma once

#include "cocos2d.h"
#include "SunProducingPlant.h" // ���� virtual public Plant
#include "Mushroom.h"          // ���� virtual public Plant
#include "GameDefs.h"

class Sunshroom : public SunProducingPlant, public Mushroom
{
public:
    // ���⹽���е������׶�
    enum class GrowthState
    {
        SMALL_INIT,     // �����£�С����
        GROWING,        // ���ڳ���
        GROWN,          // ������
        SLEEPING        // ˯���У���״̬�� Mushroom::_activityState ������
    };

    // ����/����
    Sunshroom();
    virtual ~Sunshroom() {}

    // ��׼ Cocos create ����
    CREATE_FUNC(Sunshroom);

    virtual bool init() override;
    virtual void update(float delta) override;

    // ʵ�� SunProducingPlant �ӿ�
    virtual std::vector<Sun*> produceSun() override;
    
    // Explicit override to avoid C4250 inheritance dominance warning
    virtual PlantCategory getCategory() const override { return PlantCategory::SUN_PRODUCING; }

    // ��̬��ֲ����
    static Sunshroom* plantAtPosition(const cocos2d::Vec2& globalPos);

protected:
    // ʵ�� Mushroom ���麯��
    virtual void wakeUp() override;
    virtual void sleep() override;

    // �ڲ��߼�
    void setGrowthState(GrowthState state);
    void setAnimation() override; // ���� Plant �� setAnimation
    void startGrowingSequence();
    void onGrowthSequenceFinished();

    // ��̬����
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