#pragma once

#include "BombPlant.h"
#include "GameDefs.h"
#include <vector>

// Forward declaration
class Zombie;

/**
 * @brief PotatoMine landmine plant
 * Flow: Planting -> ARMING (underground, timing) -> READY (emerged) -> TRIGGERED (explodes when stepped on)
 */
class PotatoMine : public BombPlant
{
public:
    enum class MineState
    {
        ARMING,     // Underground preparation stage
        READY,      // Ready above ground, can be triggered
        TRIGGERED   // Triggered, playing explosion animation
    };

    // ---------- cocos2d-x standard macros ----------
    CREATE_FUNC(PotatoMine);

    // ---------- Lifecycle ----------
    virtual bool init() override;
    virtual void update(float delta) override;

    // ---------- Static factory for SeedPacket ----------
    static PotatoMine* plantAtPosition(const cocos2d::Vec2& globalPos);

    // ---------- BombPlant interface ----------
    virtual void explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol) override;

private:
    PotatoMine();

    // BombPlant pure virtual function implementation
    virtual void playExplosionAnimation() override;

    // Plant animation initialization
    virtual void setAnimation() override;

    void switchToReadyState();

    // ---------- Member variables ----------
    MineState _state;
    float _armingTimer;     // Underground preparation remaining time

    // ---------- 常量 ----------
    static const float DEFAULT_ARMING_TIME;          // Preparation duration
    static const int   EXPLOSION_DAMAGE;             // Damage
    static const int   EXPLOSION_RADIUS;             // Effect grid radius (0 = own grid)
    static const std::string ARMING_IMAGE;           // Underground image
    static const std::string READY_FRAME_DIR;        // Ready frame directory (1 (1).png...)
    static const std::string TRIGGERED_IMAGE;        // Explosion image
};
