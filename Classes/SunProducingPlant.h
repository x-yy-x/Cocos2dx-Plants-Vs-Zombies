#pragma once

#include "Plant.h"

// Forward declaration
class Sun;

/**
 * @brief Abstract base class for sun-producing plants
 */
class SunProducingPlant : virtual public Plant
{
public:
    /**
     * @brief Get plant category
     */
    virtual PlantCategory getCategory() const override { return PlantCategory::SUN_PRODUCING; }

    /**
     * @brief Try to produce sun if cooldown is finished
     * @return Sun* Returns Sun instance if ready, nullptr otherwise
     */
    virtual std::vector<Sun*> produceSun() = 0;

protected:
    SunProducingPlant() : Plant() {}
    virtual ~SunProducingPlant() {}
};

