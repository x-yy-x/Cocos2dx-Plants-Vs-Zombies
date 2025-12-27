#ifndef __CHERRY_BOMB_H__
#define __CHERRY_BOMB_H__

#include "cocos2d.h"
#include "BombPlant.h"
#include "GameDefs.h"

/**
 * @brief CherryBomb is a powerful explosive plant that deals massive area damage.
 * It features a short "arming" animation before detonating in a 3x3 grid area.
 */
class CherryBomb : public BombPlant
{
public:
    /**
     * @brief Initializes the CherryBomb sprite and base stats.
     * @return true if initialization was successful.
     */
    virtual bool init() override;

    // Standard factory macro for Cocos2d-x
    CREATE_FUNC(CherryBomb);

    /**
     * @brief Static helper to create and snap a CherryBomb to the game grid.
     * @param globalPos The world position where the player intends to plant.
     * @return Pointer to the new CherryBomb instance.
     */
    static CherryBomb* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Update loop to handle the transition from arming to detonation.
     * @param delta Elapsed time since last frame.
     */
    virtual void update(float delta) override;

    /**
     * @brief Triggers the damage logic and explosion visuals.
     * @param allZombiesInRow Array of zombie vectors used to calculate hits.
     * @param plantRow The grid row of this plant.
     * @param plantCol The grid column of this plant.
     */
    virtual void explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol) override;

private:
    // ----------------------------------------------------
    // Static configuration constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const std::string EXPLOSION_IMAGE;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_RANGE;
    static const int EXPLOSION_DAMAGE;
    static const int EXPLOSION_RADIUS; // Represents 1 cell in every direction (3x3 total)

    /**
     * @brief Configures the "swelling" animation that plays before the blast.
     */
    virtual void setAnimation() override;

    /**
     * @brief Spawns the explosion overlay and handles audio feedback.
     */
    virtual void playExplosionAnimation() override;

    CherryBomb();

    float idle_animation_duration; // Duration of the arming phase in seconds
};

#endif // __CHERRY_BOMB_H__