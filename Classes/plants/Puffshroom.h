#ifndef __PUFFSHROOM_H__
#define __PUFFSHROOM_H__

#include "cocos2d.h"
#include "AttackingPlant.h" 
#include "Mushroom.h"       
#include "Zombie.h"

/**
 * @brief Puff-shroom: A 0-cost mushroom that fires short-range spores.
 * Inherits from AttackingPlant for combat logic and Mushroom for sleep mechanics.
 */
class Puffshroom : public AttackingPlant, public Mushroom
{
public:
    CREATE_FUNC(Puffshroom);

    Puffshroom();
    virtual ~Puffshroom() {}

    virtual bool init() override;
    virtual void update(float delta) override;

    /**
     * @brief Combat logic: Checks for zombies within a 3-tile range.
     * @return Vector containing a spawned Puff bullet if conditions are met.
     */
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) override;

    /** @brief Explicit override to resolve diamond inheritance dominance (C4250). */
    virtual PlantCategory getCategory() const override { return PlantCategory::ATTACKING; }

    /** @brief Static factory to plant Puff-shroom on the grid. */
    static Puffshroom* plantAtPosition(const cocos2d::Vec2& globalPos);

private:
    // Mushroom interface implementation
    virtual void wakeUp() override;
    virtual void sleep() override;
    virtual void setAnimation() override;

    // Static configuration
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_COOLDOWN;
    static const int DETECTION_RANGE; // Range limited to ~3 tiles
};

#endif // __PUFFSHROOM_H__