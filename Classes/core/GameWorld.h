#ifndef __GAMEWORLD_H__
#define __GAMEWORLD_H__

#include "GameDefs.h"
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include <vector>

// Forward declarations to improve compilation time
class Plant;
class Zombie;
class Shovel;
class Bullet;
class SeedPacket;
class Sun;
class SunProducingPlant;
class AttackingPlant;
class BombPlant;
class IceTile;
class Coin;
class Rake;
class Mower;

class GameWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(bool isNightMode = false, const std::vector<PlantName>& plantNames = std::vector<PlantName>());
    static GameWorld* create(bool isNightMode = false, const std::vector<PlantName>& plantNames = std::vector<PlantName>());

    virtual bool init() override;
    virtual ~GameWorld();

    /** @brief Add a zombie instance to the game world */
    void addZombie(Zombie* z);

    /** @brief Check if current scene is in night mode */
    bool isNightMode() const { return is_night_mode; }

    /** @brief Add an ice tile effect to the world */
    void addIceTile(IceTile* ice);

    /** @brief Remove all ice tiles in a specific row */
    void removeIceInRow(int row);

    /** @brief Get the current total sun resources */
    int getSunCount() const { return sun_count; }

private:
    // UI Callbacks
    void menuCloseCallback(cocos2d::Ref* pSender);

    /**
     * @brief Toggles game speed between Normal, 2x, and 3x
     * @param sender The menu item that triggered the callback
     */
    void toggleSpeedMode(cocos2d::Ref* sender);

    /** @brief Initialize the seed packet UI at the top of the screen */
    void initSeedPackets();

    /** @brief Displays the in-game pause menu overlay */
    void showPauseMenu(cocos2d::Ref* sender);

    /** @brief Resumes the game from a paused state */
    void resumeGame(cocos2d::Ref* sender);

    /** @brief Restarts the current level */
    void restartGame(cocos2d::Ref* sender);

    /** @brief Returns to the main menu scene */
    void returnToMenu(cocos2d::Ref* sender);

    /** @brief Triggered when a zombie reaches the player's house */
    void showGameOver();

    // Volume Control Logic
    void increaseMusicVolume(cocos2d::Ref* sender);
    void decreaseMusicVolume(cocos2d::Ref* sender);
    void increaseVolume(cocos2d::Ref* sender);
    void decreaseVolume(cocos2d::Ref* sender);

    /** @brief Main game loop logic */
    virtual void update(float delta) override;

    // Phased Batch Generation (Wave System)
    void spawnTimedBatch(float normalizedTime);
    void spawnFinalWave();
    void spawnSubBatch(int normalCnt, int poleCnt, int bucketHeadCnt, int zamboniCnt, int gargantuarCnt, float delaySec);
    int randRange(int a, int b);
    int applyNightFactor(int baseCount, bool allowZero = false);

    /** @brief Victory sequence when all waves are cleared */
    void showWinTrophy();

    /** @brief Initializes touch listeners for gameplay mechanics */
    void setupUserInteraction();

    /**
     * @brief Logic for placing a plant on the grid
     * @return true if successful
     */
    bool tryPlantAtPosition(const cocos2d::Vec2& globalPos, SeedPacket* seedPacket);

    /**
     * @brief Logic for removing a plant using the shovel tool
     * @return true if successful
     */
    bool tryRemovePlantAtPosition(const cocos2d::Vec2& globalPos);

    /** @brief Maps screen position to grid row/column */
    bool getGridCoordinates(const cocos2d::Vec2& globalPos, int& outRow, int& outCol) const;

    // Component update functions
    void updateZombies(float delta);
    void updatePlants(float delta);
    void updateBullets(float delta);
    void updateMoneyBankDisplay();
    void updateCoins(float delta);
    void updateSunDisplay();
    void updateSuns(float delta);
    void updateIceTiles(float delta);

    // Garbage collection for dead objects
    void removeDeadZombies();
    void removeDeadPlants();
    void removeInactiveBullets();
    void removeExpiredCoins();
    void removeExpiredSuns();
    void removeExpiredIceTiles();

    // Spawn systems
    void spawnSunFromSky();
    void maybePlayZombieGroan(float delta);
    void spawnCoinAfterZombieDeath(Zombie* zombie);

    /** @brief Checks if a specific grid cell is covered by ice */
    bool hasIceAt(int row, int col) const;

    // Grid Storage: directly stores Plant pointers for O(1) access
    Plant* plant_grid[MAX_ROW][MAX_COL];

    // Interaction State
    bool plant_selected;
    int selected_seedpacket_index;
    Plant* preview_plant;

    bool shovel_selected;

    // UI Components
    cocos2d::Sprite* coin_bank;
    Shovel* shovel;
    cocos2d::Sprite* shovel_back;
    std::vector<SeedPacket*> seed_packets;
    std::vector<PlantName> initial_plant_names;

    // Progress Tracking
    cocos2d::Sprite* flag_icon_right{ nullptr };
    cocos2d::ui::LoadingBar* progress_bar;
    cocos2d::Sprite* progress_bar_full;
    float elapsed_time = 0.0f;
    const float TOTAL_GAME_TIME = 300.0f;

    // Resource System
    int sun_count;
    cocos2d::Label* sun_count_label;
    cocos2d::Label* money_count_label;

    // Object Containers
    std::vector<Zombie*> zombies_in_row[MAX_ROW];
    std::vector<Bullet*> bullets;
    std::vector<Sun*> suns;
    std::vector<IceTile*> ice_tiles;
    std::vector<Coin*> coins;

    // Map Utilities
    Rake* rake_per_row[MAX_ROW];
    Mower* mower_per_row[MAX_ROW];

    // Wave/Batch Spawning State
    int current_wave;
    float next_batch_time_sec;
    bool final_wave_triggered{ false };

    // Win/Lose Flow State
    bool final_wave_spawning_done{ false };
    bool win_shown{ false };
    cocos2d::Sprite* trophy_sprite{ nullptr };

    float zombie_groan_timer;
    float sun_spawn_timer;
    int background_music_id{ -1 };
    bool is_night_mode{ false };

    // Speed Control
    int speed_level{ 0 };
    float speed_scale{ 2.0f };
    cocos2d::MenuItemToggle* speed_toggle_button;

    // Pause & Control Systems
    bool is_paused{ false };
    cocos2d::MenuItemImage* pause_button;
    cocos2d::Layer* pause_menu_layer;
    cocos2d::Menu* pause_menu;
    cocos2d::Label* volume_label;
    float music_volume{ 1.0f };
    bool is_gameover{ false };
    bool game_started{ false };
};

// Global Wave Constants
const float FIRST_WAVE_TIME = 60.0f;
const float MIN_WAVE_INTERVAL = 10.0f;

#endif // __GAMEWORLD_H__