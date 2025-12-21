#pragma once

#include "BombPlant.h"
#include "GameDefs.h"
#include <vector>

// Forward declaration
class Zombie;

/**
 * @brief PotatoMine 地雷植物
 * 流程：种植 -> ARMING（埋地，计时） -> READY（破土） -> TRIGGERED（被踩爆炸）
 */
class PotatoMine : public BombPlant
{
public:
    enum class MineState
    {
        ARMING,     // 地下准备阶段
        READY,      // 地面就绪，可被触发
        TRIGGERED   // 已触发，播放爆炸动画
    };

    // ---------- cocos2d-x 标准宏 ----------
    CREATE_FUNC(PotatoMine);

    // ---------- 生命周期 ----------
    virtual bool init() override;
    virtual void update(float delta) override;

    // ---------- 静态工厂，用于 SeedPacket ----------
    static PotatoMine* plantAtPosition(const cocos2d::Vec2& globalPos);

    // ---------- BombPlant 接口 ----------
    virtual void explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol) override;

protected:
    PotatoMine();

    // BombPlant 纯虚函数实现
    virtual void playExplosionAnimation() override;

    // Plant 动画初始化
    virtual void setAnimation() override;

private:
    void switchToReadyState();

    // ---------- 成员变量 ----------
    MineState _state;
    float _armingTimer;     // 埋地准备剩余时间

    // ---------- 常量 ----------
    static const float DEFAULT_ARMING_TIME;          // 准备时长
    static const int   EXPLOSION_DAMAGE;             // 伤害
    static const int   EXPLOSION_RADIUS;             // 作用格半径（0=自身格）
    static const std::string ARMING_IMAGE;           // 埋地图片
    static const std::string READY_FRAME_DIR;        // ready 帧目录（1 (1).png...）
    static const std::string TRIGGERED_IMAGE;        // 爆炸图片
};
