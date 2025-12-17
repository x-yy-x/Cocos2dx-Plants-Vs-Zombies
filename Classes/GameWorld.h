#ifndef __GAMEWORLD_H__
#define __GAMEWORLD_H__

#include "GameDefs.h"
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include <vector>

// Forward declarations
class Plant;
class Zombie;
class Shovel;
class Bullet;
class SeedPacket;
class Sun;
class SunProducingPlant;
class AttackingPlant;
class BombPlant;

class GameWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(bool isNightMode = false);
    static GameWorld* create(bool isNightMode = false);

    virtual bool init() override;
    virtual ~GameWorld();
    
    // A selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    /**
     * @brief Toggle speed mode on/off
     * @param sender The menu item that triggered this callback
     */
    void toggleSpeedMode(cocos2d::Ref* sender);

    /**
     * @brief Show pause menu
     * @param sender The menu item that triggered this callback
     */
    void showPauseMenu(cocos2d::Ref* sender);

    /**
     * @brief Resume game from pause
     * @param sender The menu item that triggered this callback
     */
    void resumeGame(cocos2d::Ref* sender);

    /**
     * @brief Restart current game
     * @param sender The menu item that triggered this callback
     */
    void restartGame(cocos2d::Ref* sender);

    /**
     * @brief Return to main menu
     * @param sender The menu item that triggered this callback
     */
    void returnToMenu(cocos2d::Ref* sender);

    /**
     * @brief Show game over screen when zombie reaches left edge
     */
    void showGameOver();

    /**
     * @brief Increase background music volume
     * @param sender The menu item that triggered this callback
     */
    void increaseMusicVolume(cocos2d::Ref* sender);

    /**
     * @brief Decrease background music volume
     * @param sender The menu item that triggered this callback
     */
    void decreaseMusicVolume(cocos2d::Ref* sender);

    /**
     * @brief Increase background music volume
     * @param sender The menu item that triggered this callback
     */
    void increaseVolume(cocos2d::Ref* sender);

    /**
     * @brief Decrease background music volume
     * @param sender The menu item that triggered this callback
     */
    void decreaseVolume(cocos2d::Ref* sender);

    /**
     * @brief Update function called every frame
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    void addZombie(Zombie* z);

	bool isNightMode() const { return _isNightMode; }

private:
    /**
     * @brief Setup user interaction (touch events for planting and shoveling)
     */
    void setupUserInteraction();

    /**
     * @brief Try to plant using selected seed packet
     * @param globalPos The global position where the user touched
     * @param seedPacket The seed packet to use for planting
     * @return true if planting was successful, false otherwise
     */
    bool tryPlantAtPosition(const cocos2d::Vec2& globalPos, SeedPacket* seedPacket);

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

    /**
     * @brief Update sun counter display
     */
    void updateSunDisplay();

    /**
     * @brief Update all suns (movement and lifetime)
     * @param delta Time delta
     */
    void updateSuns(float delta);

    /**
     * @brief Remove expired or collected suns
     */
    void removeExpiredSuns();

    /**
     * @brief Spawn a sun from sky at random grid column
     */
    void spawnSunFromSky();

    void maybePlayZombieGroan(float delta);


    // Grid storage system - stores Plant pointers directly
    Plant* _plantGrid[MAX_ROW][MAX_COL];
    
    // Plant selection state
    bool _plantSelected;
    int _selectedSeedPacketIndex;  // Index of selected seed packet (-1 if none)
    Plant* _previewPlant;          // Transparent preview plant following cursor

    // Shovel state
    bool _shovelSelected;

    // UI elements
    Shovel* _shovel;
    cocos2d::Sprite* _shovelBack;
    std::vector<SeedPacket*> _seedPackets;

    // --- 进度条相关 ---
    cocos2d::ui::LoadingBar* _progressBar;
    cocos2d::Sprite* _progressBarFull;
    float _elapsedTime = 0.0f;

    const float TOTAL_GAME_TIME = 300.0f;
    
    // Sun system
    int _sunCount;
    cocos2d::Label* _sunCountLabel;

    // Containers for game objects
    std::vector<Zombie*> _zombiesInRow[MAX_ROW];
    std::vector<Bullet*> _bullets;
    std::vector<Sun*> _suns;

    // Wave spawning system
    int _currentWave;                // Current wave number (starts at 0)
    int _tickCount;                  // Total tick count (1 tick = 1/30 second)
    int _nextWaveTickCount;          // Tick count when next wave should spawn
    bool _gameStarted;               // Whether game has started

    float _zombieGroanTimer;

    // Sun spawning system
    float _sunSpawnTimer;            // Timer for sun spawning (every 5 seconds)
    int _backgroundMusicId{-1};
    bool _isNightMode{ false };

    // Speed mode system
    int _speedLevel{ 0 };           // Speed level: 0=normal, 1=2x, 2=3x
    float _speedScale{ 2.0f };       // Speed multiplier when in speed mode
    cocos2d::MenuItemToggle* _speedToggleButton; // Speed mode toggle button

    // Pause system
    bool _isPaused{ false };
    cocos2d::MenuItemImage* _pauseButton;
    cocos2d::Layer* _pauseMenuLayer;
    cocos2d::Menu* _pauseMenu;
    cocos2d::Label* _volumeLabel;
    float _musicVolume{ 1.0f };

    // Game over system
    bool _isGameOver{ false };
};

// Wave spawning parameters (reduced difficulty)
const int FIRST_WAVE_TICK = 1800;    // First wave spawns at 1800 ticks (60 seconds)
const int MIN_WAVE_INTERVAL = 300;   // Minimum interval between waves (10 seconds)

#endif // __GAMEWORLD_H__
