#ifndef __SUNSHROOM_H__
#define __SUNSHROOM_H__

#include "cocos2d.h"
#include "SunProducingPlant.h" 
#include "Mushroom.h"           
#include "GameDefs.h"

/**
 * @brief Sun-shroom: A nighttime sun producer that grows over time.
 * It starts small (producing 15 sun) and grows into a large mushroom (producing 25 sun)
 * after a set duration. Like all mushrooms, it sleeps during the day.
 */
class Sunshroom : public SunProducingPlant, public Mushroom
{
public:
    Sunshroom();
    virtual ~Sunshroom() {}

    CREATE_FUNC(Sunshroom);

    virtual bool init() override;
    virtual void update(float delta) override;

    /**
     * @brief Produces sun based on current growth state.
     * @return Vector containing the spawned Sun object.
     */
    virtual std::vector<Sun*> produceSun() override;

    /** @brief Explicit override to resolve diamond inheritance dominance (C4250). */
    virtual PlantCategory getCategory() const override { return PlantCategory::SUN_PRODUCING; }

    /** @brief Static factory for planting on the grid. */
    static Sunshroom* plantAtPosition(const cocos2d::Vec2& globalPos);

private:
    /** @brief Internal growth stages of the Sun-shroom. */
    enum class GrowthState
    {
        SMALL_INIT,     // Initial small stage
        GROWING,        // Mid-transition sequence
        GROWN,          // Final large stage
        SLEEPING        // Inactive (Daytime)
    };

    // Mushroom interface implementation
    virtual void wakeUp() override;
    virtual void sleep() override;

    // Logic helpers
    void setGrowthState(GrowthState state);
    void setAnimation() override;
    void startGrowingSequence();
    void onGrowthSequenceFinished();

    // Static configuration
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float SUN_PRODUCTION_INTERVAL;
    static const float GROWTH_TIME;     // Time required to mature
    static const float SMALL_SCALE;     // Scale factor for small stage
    static const float GROWN_SCALE;     // Scale factor for large stage
    static const int SMALL_SUN_VALUE;   // 15
    static const int GROWN_SUN_VALUE;   // 25

    GrowthState growth_state;
    float growth_timer;
    float current_scale;
};

#endif // __SUNSHROOM_H__