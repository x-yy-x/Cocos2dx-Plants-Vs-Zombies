#pragma once

#include "Plant.h"

/**
 * @brief UpgradedPlant class, inherits from Plant.
 * Base class for all upgraded plant variants.
 * Provides an interface for upgrading base plants.
 */
class UpgradedPlant : virtual public Plant
{
public:
    /**
     * @brief Static method to check if a base plant can be upgraded by plant name.
     * @param plantName The upgraded plant name
     * @param basePlant The base plant to check
     * @return true if the base plant can be upgraded, false otherwise
     */
    static bool canUpgradeByName(PlantName plantName, Plant* basePlant);
};