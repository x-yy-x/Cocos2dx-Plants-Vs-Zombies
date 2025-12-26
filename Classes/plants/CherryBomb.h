#pragma once

#include "cocos2d.h"
#include "BombPlant.h"
#include "GameDefs.h"

class CherryBomb : public BombPlant
{
public:
    /**
     * @brief CherryBomb initialization function
     */
    virtual bool init() override;

    // Implement the auto-generated static CherryBomb* create() function
    CREATE_FUNC(CherryBomb);

    /**
     * @brief Static planting function for CherryBomb
     * @param globalPos Touch position in global coordinates
     * @return CherryBomb* Returns CherryBomb instance on success, nullptr on failure
     */
    static CherryBomb* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Trigger explosion
     * @param allZombiesInRow All zombies in each row
     * @param plantRow The row this plant is in
     * @param plantCol The column this plant is in
     */
    virtual void explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol) override;

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const std::string EXPLOSION_IMAGE;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_RANGE;
    static const int EXPLOSION_DAMAGE;
    static const int EXPLOSION_RADIUS;  // 3x3 grid

    /**
     * @brief Set up animation frames (cherry bomb idle animation before explosion)
     */
    virtual void setAnimation() override;

    /**
     * @brief Play explosion animation
     */
    virtual void playExplosionAnimation() override;

    // Protected constructor
    CherryBomb();

    // Animation tracking
    float _idleAnimationDuration;  // How long the idle animation plays
};

