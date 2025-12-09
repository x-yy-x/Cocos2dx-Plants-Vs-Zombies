#include "ThreePeater.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string ThreePeater::IMAGE_FILENAME = "threePeat.png";
const cocos2d::Rect ThreePeater::INITIAL_PIC_RECT = Rect(0.0f, 0.0f, 85.333f, 85.333f);

// Protected constructor
ThreePeater::ThreePeater()
{
    CCLOG("ThreePeater created.");
}

// ------------------------------------------------------------------------
// 1. ThreePeater initialization
// ------------------------------------------------------------------------
bool ThreePeater::init()
{
    return initPlantWithSettings(IMAGE_FILENAME, INITIAL_PIC_RECT, 300, 1.5f);
}

// ------------------------------------------------------------------------
// 2. ThreePeater animation (6 cols x 3 rows, last 2 frames of last column are empty)
// ------------------------------------------------------------------------
void ThreePeater::setAnimation()
{
    // 6 columns x 3 rows, but last column only has 1 frame (total 16 frames)
    const int totalFrames = 16;
    createAndRunAnimation(IMAGE_FILENAME, 70.0f, 90.0f, 3, 6, 0.07f, totalFrames);
}

// ------------------------------------------------------------------------
// 3. Check and attack logic - shoots peas in THREE lanes
// ------------------------------------------------------------------------
std::vector<Bullet*> ThreePeater::checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow)
{
    std::vector<Bullet*> bullets;

    // Check if cooldown is ready
    if (_accumulatedTime < _cooldownInterval)
    {
        return bullets;
    }

    // Check if any zombie is in range in three rows (above, current, below)
    std::vector<int> rowsToCheck = {plantRow - 1, plantRow, plantRow + 1};
    if (!isZombieInRangeMultiRow(allZombiesInRow, rowsToCheck))
    {
        return bullets;
    }

    // Reset cooldown
    _accumulatedTime = 0.0f;

    // Create peas for three lanes: row above, current row, row below
    Vec2 basePos = this->getPosition();
    
    for (int offset = -1; offset <= 1; ++offset)
    {
        int targetRow = plantRow + offset;
        
        // Check if target row is valid
        if (targetRow < 0 || targetRow >= MAX_ROW)
        {
            continue;  // Skip invalid rows
        }

        // Calculate pea spawn position for this row
        float targetY = GRID_ORIGIN.y + targetRow * CELLSIZE.height + CELLSIZE.height * 0.5f + 20.0f;
        Vec2 spawnPos(basePos.x + 30.0f, targetY);

        // Create pea for this lane
        Pea* pea = Pea::create(spawnPos);
        if (pea)
        {
            bullets.push_back(pea);
            CCLOG("ThreePeater created pea for row %d at (%.2f, %.2f)", targetRow, spawnPos.x, spawnPos.y);
        }
    }

    if (!bullets.empty())
    {
        CCLOG("ThreePeater fired %d peas in multiple lanes", (int)bullets.size());
    }

    return bullets;
}

