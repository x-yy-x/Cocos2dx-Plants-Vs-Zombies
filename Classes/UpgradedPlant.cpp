#include "UpgradedPlant.h"
#include "GameDefs.h"

USING_NS_CC;

// Static method: Check if base plant can be upgraded by plant name
bool UpgradedPlant::canUpgradeByName(PlantName plantName, Plant* basePlant)
{
    if (!basePlant) return false;
    
    // Use virtual function to check upgrade compatibility
    return basePlant->canBeUpgradedTo(plantName);
}