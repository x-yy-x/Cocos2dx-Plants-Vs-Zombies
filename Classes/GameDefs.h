#pragma once
#include "cocos2d.h"

// Grid cell size
const cocos2d::Size CELLSIZE = cocos2d::Size(130, 120);

// Grid origin position (bottom-left corner)
const cocos2d::Vec2 GRID_ORIGIN = cocos2d::Vec2(10, 10);

// Maximum number of rows and columns in the grid
const int MAX_ROW = 5;
const int MAX_COL = 9;

// Layer constants for z-ordering
const int BACKGROUND_LAYER = 0;
const int PLANT_LAYER = 1;
const int BULLET_LAYER = 2;
const int ENEMY_LAYER = 3;
const int SUN_LAYER = 4;
const int SEEDPACKET_LAYER = 5;
const int ICE_LAYER = 1;
const int UI_LAYER = 10;  // For labels and other UI elements
const int WIN_LOSE_LAYER = 11;

/**
 * @brief Plant type enumeration for distinguishing plant categories
 */
enum class PlantCategory
{
    SUN_PRODUCING,   // Sun-producing plants (Sunflower)
    ATTACKING,       // Attacking plants (PeaShooter, Wallnut)
    BOMB             // Bomb plants (CherryBomb)
};

enum class PlantName
{
    CHERRYBOMB,
    GATLINGPEA,
    JALAPENO,
    PEASHOOTER,
    PUFFSHROOM,
    REPEATER,
    SPIKEROCK,
    SPIKEWEED,
    SUNFLOWER,
    SUNSHROOM,
    THREEPEATER,
    TWINSUNFLOWER,
    WALLNUT
};