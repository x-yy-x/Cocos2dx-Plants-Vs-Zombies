#ifndef __GAMEWORLD_H__
#define __GAMEWORLD_H__

#include "GameDefs.h"
#include "cocos2d.h"
#include <vector>

// Forward declarations
class Plant;
class Zombie;
class Shovel;
class Bullet;

class GameWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    
    // A selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // Implement the "static create()" method manually
    CREATE_FUNC(GameWorld);

    /**
     * @brief Update function called every frame
     * @param delta Time delta
     */
    virtual void update(float delta) override;

private:
    /**
     * @brief Setup user interaction (touch events for planting and shoveling)
     * @param sprite_seedpacket_peashooter The seed packet sprite for peashooter
     */
    void setupUserInteraction(cocos2d::Sprite* sprite_seedpacket_peashooter);

    /**
     * @brief Try to plant a plant at the given position
     * @param globalPos The global position where the user touched
     * @return true if planting was successful, false otherwise
     */
    bool tryPlantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Try to remove a plant at the given position using shovel
     * @param globalPos The global position where the shovel was released
     * @return true if removal was successful, false otherwise
     */
    bool tryRemovePlantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Get grid cell coordinates from global position
     * @param globalPos The global position
     * @param outRow Output parameter for row
     * @param outCol Output parameter for column
     * @return true if the position is within valid grid bounds
     */
    bool getGridCoordinates(const cocos2d::Vec2& globalPos, int& outRow, int& outCol);

    /**
     * @brief Spawn a wave of zombies
     * @param waveNumber Current wave number
     */
    void spawnZombieWave(int waveNumber);

    /**
     * @brief Update all zombies
     * @param delta Time delta
     */
    void updateZombies(float delta);

    /**
     * @brief Update all plants (check for firing)
     * @param delta Time delta
     */
    void updatePlants(float delta);

    /**
     * @brief Update all bullets (movement and collision)
     * @param delta Time delta
     */
    void updateBullets(float delta);

    /**
     * @brief Remove dead zombies from the scene and container
     */
    void removeDeadZombies();

    /**
     * @brief Remove dead plants from the scene and grid
     */
    void removeDeadPlants();

    /**
     * @brief Remove inactive bullets from the scene and container
     */
    void removeInactiveBullets();

    // Grid storage system - stores Plant pointers directly
    Plant* _plantGrid[MAX_ROW][MAX_COL];
    
    // Plant selection state
    bool _plantSelected;

    // Shovel state
    bool _shovelSelected;

    // UI elements
    Shovel* _shovel;
    cocos2d::Sprite* _shovelBack;

    // Containers for game objects
    std::vector<Plant*> _plants; // Keep for general iteration if needed, but main logic uses grid/row
    std::vector<Zombie*> _zombiesInRow[MAX_ROW];
    std::vector<Bullet*> _bullets;

    // Wave spawning system
    int _currentWave;                // Current wave number (starts at 0)
    int _tickCount;                  // Total tick count (1 tick = 1/30 second)
    int _nextWaveTickCount;          // Tick count when next wave should spawn
    bool _gameStarted;               // Whether game has started
};

// Wave spawning parameters (reduced difficulty)
const int FIRST_WAVE_TICK = 1800;    // First wave spawns at 1800 ticks (60 seconds)
const int MIN_WAVE_INTERVAL = 300;   // Minimum interval between waves (10 seconds)

#endif // __GAMEWORLD_H__
