#pragma once
#include "cocos2d.h"
#include <string>
#include <functional>

/**
 * @section Grid Configuration
 * Defines the dimensions and positioning of the gameplay grid.
 */
 // Individual grid cell dimensions (Width, Height)
const cocos2d::Size CELLSIZE = cocos2d::Size(130, 120);

// World coordinates for the bottom-left corner of the grid
const cocos2d::Vec2 GRID_ORIGIN = cocos2d::Vec2(10, 10);

// Grid constraints
const int MAX_ROW = 5;
const int MAX_COL = 9;

/**
 * @section Rendering Z-Order
 * Constants used to manage the drawing order (depth) of different game elements.
 */
const int BACKGROUND_LAYER = 0;
const int PLANT_LAYER = 1;
const int BULLET_LAYER = 2;
const int ENEMY_LAYER = 3;
const int SEEDPACKET_LAYER = 4;
const int SUN_LAYER = 5;
const int ICE_LAYER = 1;
const int UI_LAYER = 10;        // Layer for labels, HUD, and interface elements
const int WIN_LOSE_LAYER = 11;  // Top-most layer for endgame screens

/**
 * @enum PlantCategory
 * @brief Categorizes plants based on their primary gameplay function.
 */
enum class PlantCategory
{
    SUN_PRODUCING,   // Produces sun currency (e.g., Sunflower)
    ATTACKING,       // Engages enemies via projectiles or contact (e.g., PeaShooter)
    BOMB             // Explosive plants with area-of-effect damage (e.g., CherryBomb)
};

/**
 * @enum PlantName
 * @brief Unique identifiers for every plant species in the game.
 */
enum class PlantName
{
    SUNFLOWER,
    PEASHOOTER,
    WALLNUT,
    POTATOMINE,
    REPEATER,
    THREEPEATER,
    CHERRYBOMB,
    SUNSHROOM,
    PUFFSHROOM,
    SPIKEWEED,
    JALAPENO,
    TWINSUNFLOWER,
    GATLINGPEA,
    SPIKEROCK,
    UNKNOWN
};

class SeedPacket;

/**
 * @typedef SeedPacketFactory
 * @brief Function wrapper used as a factory pattern to instantiate SeedPacket objects.
 * Parameters: Image path, Cooldown time, Sun cost, Plant identifier.
 */
typedef std::function<SeedPacket* (const std::string&, float, int, PlantName)> SeedPacketFactory;

/**
 * @struct PlantConfig
 * @brief Static configuration data for plant types, typically stored in a global lookup table.
 */
struct PlantConfig {
    std::string packetImage;    // Path to the UI texture for the seed packet
    float cooldown;             // Time in seconds before the plant can be used again
    int sunCost;                // Amount of sun required to deploy the plant
    SeedPacketFactory factory;  // Factory method to create the specific seed packet instance
};