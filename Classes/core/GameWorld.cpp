#include "GameWorld.h"
#include "BackGround.h"
#include "Plant.h"
#include "SunProducingPlant.h"
#include "AttackingPlant.h"
#include "BombPlant.h"
#include "Repeater.h"
#include "Sunflower.h"
#include "Zombie.h"
#include "Shovel.h"
#include "Bullet.h"
#include "SeedPacket.h"
#include "Sun.h"
#include "PoleVaulter.h"
#include "FlagZombie.h"
#include "Imp.h"
#include "Gargantuar.h"
#include "GameMenu.h"
#include "SelectCardsScene.h"
#include "Zomboni.h"
#include "IceTile.h"
#include "Rake.h"
#include "Mower.h"
#include "coin.h"
#include "BucketHeadZombie.h"
#include "NormalZombie.h"
#include "UpgradedPlant.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "audio/include/AudioEngine.h"
#include "base/ccUtils.h"
#include "PlayerProfile.h"
#include "map"

// CCRANDOM
#include "base/ccRandom.h"

USING_NS_CC;

GameWorld* GameWorld::create(bool isNightMode, const std::vector<PlantName>& plantNames)
{
    GameWorld* instance = new (std::nothrow) GameWorld();
    if (instance)
    {
        instance->is_night_mode = isNightMode;
        instance->initial_plant_names = plantNames;
        if (instance->init())
        {
            instance->autorelease();
            return instance;
        }
        delete instance;
    }
    return nullptr;
}

Scene* GameWorld::createScene(bool is_night_mode, const std::vector<PlantName>& plantNames)
{
    return GameWorld::create(is_night_mode, plantNames);
}

GameWorld::~GameWorld()
{
    if (background_music_id != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(background_music_id);
    }

    // Clean up pause menu resources
    if (pause_menu_layer)
    {
        pause_menu_layer = nullptr;
        pause_menu = nullptr;
        volume_label = nullptr;
    }

    // Reset game state
    is_paused = false;
    speed_level = 0;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

void GameWorld::initSeedPackets() {
    float baseX = 187.0f;
    float baseY = 667.0f;
    float spacing = 65.0f;

    // Fix error: Use C++11 compatible way to iterate map
    if (initial_plant_names.empty()) {
        for (std::map<PlantName, PlantConfig>::const_iterator it = SeedPacket::CONFIG_TABLE.begin();
            it != SeedPacket::CONFIG_TABLE.end(); ++it)
        {
            initial_plant_names.push_back(it->first); // it->first is PlantName
        }
    }

    // Add in a unified loop
    for (size_t i = 0; i < initial_plant_names.size(); ++i) {
        SeedPacket* packet = SeedPacket::createFromConfig(initial_plant_names[i]);
        if (packet) {
            packet->setPosition(Vec2(baseX + i * spacing, baseY));
            this->addChild(packet, SEEDPACKET_LAYER);
            seed_packets.push_back(packet);
        }
    }
}
// On "init" you need to initialize your instance
bool GameWorld::init()
{
    CCLOG("Init started!");

    // 1. Super init first
    if (!Scene::init())
    {
        return false;
    }

    cocos2d::AudioEngine::stopAll();
    background_music_id = cocos2d::AudioEngine::INVALID_AUDIO_ID;

    // Initialize plant grid (all cells empty at start)
    for (int row = 0; row < MAX_ROW; ++row)
    {
        for (int col = 0; col < MAX_COL; ++col)
        {
            plant_grid[row][col] = nullptr;
        }
    }
    plant_selected = false;
    selected_seedpacket_index = -1;
    preview_plant = nullptr;
    shovel_selected = false;
    shovel = nullptr;
    shovel_back = nullptr;
    sun_count = 200; // Initial sun count
    sun_count_label = nullptr;

    // Initialize timed batch spawning (Version D)
    current_wave = 0; // legacy
    next_batch_time_sec = 8.0f; // First batch around 8 seconds
    final_wave_triggered = false;
    game_started = true;

    // Initialize sun spawning system
    sun_spawn_timer = 0.0f;
    zombie_groan_timer = 3.0f;

    // Initialize Rake/Mower slots
    for (int r = 0; r < MAX_ROW; ++r) { rake_per_row[r] = nullptr; mower_per_row[r] = nullptr; }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create background
    auto backGround = BackGround::create(is_night_mode);
    if (backGround == nullptr)
    {
        problemLoading("'background.png'");
        return false;
    }
    this->addChild(backGround, BACKGROUND_LAYER);

    // Spawn Rake if enabled (random row, right end)
    if (PlayerProfile::getInstance()->isRakeEnabled())
    {
        int rakeRow = rand() % MAX_ROW;
        auto rake = Rake::create();
        if (rake)
        {
            float y = GRID_ORIGIN.y + rakeRow * CELLSIZE.height + CELLSIZE.height * 0.6f;
            float x = visibleSize.width - 120.0f;
            rake->setPosition(Vec2(x, y));
            this->addChild(rake, ENEMY_LAYER);
            rake_per_row[rakeRow] = rake;
        }
    }

    // Spawn Mowers per row at far left if enabled
    if (PlayerProfile::getInstance()->isMowerEnabled())
    {
        for (int r = 0; r < MAX_ROW; ++r)
        {
            if (mower_per_row[r]) continue;
            auto mower = Mower::create();
            if (mower)
            {
                float y = GRID_ORIGIN.y + r * CELLSIZE.height + CELLSIZE.height * 0.6f;
                float x = GRID_ORIGIN.x - 30.0f; // just left to the first cell
                mower->setPosition(Vec2(x, y));
                this->addChild(mower, ENEMY_LAYER);
                mower_per_row[r] = mower;
            }
        }
    }

    auto progressBG = Sprite::create("FlagMeterEmpty.png");
    if (progressBG) {
        progressBG->setPosition(Vec2(visibleSize.width - 150, 40));
        this->addChild(progressBG, UI_LAYER);

        progress_bar = ui::LoadingBar::create("FlagMeterFull.png");

        progress_bar->setDirection(ui::LoadingBar::Direction::RIGHT);

        progress_bar->setPercent(0);

        progress_bar->setPosition(progressBG->getPosition());
        this->addChild(progress_bar, UI_LAYER + 1);

        // Add flag icon at the left end of the progress bar (FlagMeterParts1.png)
        auto flagIconLeft = Sprite::create("FlagMeterParts2.png");
        if (flagIconLeft) {
            flagIconLeft->setPosition(Vec2(progressBG->getPositionX() - progressBG->getContentSize().width / 2 + 4, progressBG->getPositionY() + 5));
            this->addChild(flagIconLeft, UI_LAYER + 2);
        }

        // Add flag icon at the right end of the progress bar (FlagMeterParts1.png) - moves with progress
        flag_icon_right = Sprite::create("FlagMeterParts1.png");
        if (flag_icon_right) {
            // Initial position at left end (0% progress)
            flag_icon_right->setPosition(Vec2(progressBG->getPositionX() - progressBG->getContentSize().width / 2 + 4, progressBG->getPositionY() + 5));
            this->addChild(flag_icon_right, UI_LAYER + 2);
        }
    }


    elapsed_time = 0.0f;
    initSeedPackets();

    // Create sun counter label
    sun_count_label = Label::createWithSystemFont(std::to_string(sun_count), "Arial", 20);
    if (sun_count_label)
    {
        sun_count_label->setPosition(Vec2(100, visibleSize.height - 90));
        sun_count_label->setColor(Color3B::BLACK);
        this->addChild(sun_count_label, UI_LAYER);
    }

    //Coin system
    money_count_label = Label::createWithSystemFont(std::to_string(PlayerProfile::getInstance()->getCoins()), "Arial", 20);
    if (money_count_label)
    {
        money_count_label->setPosition(Vec2(100, 20));
        money_count_label->setColor(Color3B::BLACK);
        this->addChild(money_count_label, UI_LAYER);
    }

    coin_bank = Sprite::create("CoinBank.png");
    if (coin_bank == nullptr)
    {
        problemLoading("'Coinbank.png'");
    }
    else
    {
        coin_bank->setPosition(Vec2(100, 20));
        this->addChild(coin_bank, SEEDPACKET_LAYER);
    }

    // Create shovel background
    shovel_back = Sprite::create("ShovelBack.png");
    if (shovel_back == nullptr)
    {
        problemLoading("'ShovelBack.png'");
    }
    else
    {
        shovel_back->setPosition(Vec2(visibleSize.width - 465, visibleSize.height - 80));
        this->addChild(shovel_back, SEEDPACKET_LAYER);
    }

    // Create shovel
    shovel = Shovel::create();
    if (shovel == nullptr)
    {
        problemLoading("'Shovel.png'");
    }
    else
    {
        Vec2 shovelPos = shovel_back ? shovel_back->getPosition() : Vec2(visibleSize.width - 500, visibleSize.height - 80);
        shovel->setOriginalPosition(shovelPos);
        this->addChild(shovel, SEEDPACKET_LAYER + 1);
    }

    // Create pause button
    pause_button = MenuItemImage::create(
        "btn_Menu.png",
        "btn_Menu2.png",
        [this](Ref* sender) {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            showPauseMenu(sender);
        }
    );

    if (pause_button)
    {
        pause_button->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 40));
        pause_button->setScale(0.8f);

        auto pauseMenu = Menu::create(pause_button, nullptr);
        pauseMenu->setPosition(Vec2::ZERO);
        this->addChild(pauseMenu, UI_LAYER);
    }

    // Create speed mode toggle button (below pause button)
    auto speedNormalItem = MenuItemFont::create("Normal Speed", CC_CALLBACK_1(GameWorld::toggleSpeedMode, this));
    auto speed2xItem = MenuItemFont::create("2x Speed", CC_CALLBACK_1(GameWorld::toggleSpeedMode, this));
    auto speed3xItem = MenuItemFont::create("3x Speed", CC_CALLBACK_1(GameWorld::toggleSpeedMode, this));
    speed_toggle_button = MenuItemToggle::createWithCallback(
        [this](Ref* sender) {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            toggleSpeedMode(sender);
        },
        speedNormalItem,
        speed2xItem,
        speed3xItem,
        nullptr
    );
    auto speedButtonBack = Sprite::create("button.png");
    speedButtonBack->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 85));
    speedButtonBack->setScale(1.3f, 0.85f);
    this->addChild(speedButtonBack, UI_LAYER - 1);
    if (speed_toggle_button)
    {
        speed_toggle_button->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 80));
        speed_toggle_button->setScale(0.8f);

        auto speedMenu = Menu::create(speed_toggle_button, nullptr);
        speedMenu->setPosition(Vec2::ZERO);
        this->addChild(speedMenu, UI_LAYER);
    }


    // debug mode
    bool debug = true;
    if (debug) {
        {
            auto debugZombie = Gargantuar::createZombie();
            if (debugZombie)
            {
                int row = 2;
                const float ZOMBIE_Y_OFFSET = 0.7f;
                float y = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * ZOMBIE_Y_OFFSET;
                float x = visibleSize.width - 200;
                debugZombie->setPosition(Vec2(x, y));
                this->addChild(debugZombie, ENEMY_LAYER);
                zombies_in_row[row].push_back(debugZombie);
                CCLOG("DEBUG: Spawned test zombie at row %d", row);
            }
        }

        {
            auto debugCoin0 = Coin::create(Coin::CoinType::SILVER);
            if (debugCoin0) {
                debugCoin0->setPosition(Vec2(300, 300));
                this->addChild(debugCoin0, SUN_LAYER);
                coins.push_back(debugCoin0);
            }
            auto debugCoin1 = Coin::create(Coin::CoinType::GOLD);
            if (debugCoin1) {
                debugCoin1->setPosition(Vec2(500, 300));
                this->addChild(debugCoin1, SUN_LAYER);
                coins.push_back(debugCoin1);
            }
            auto debugCoin2 = Coin::create(Coin::CoinType::DIAMOND);
            if (debugCoin2) {
                debugCoin2->setPosition(Vec2(800, 300));
                this->addChild(debugCoin2, SUN_LAYER);
                coins.push_back(debugCoin2);
            }
        }
    }
    // Setup user interaction
    setupUserInteraction();


    // Enable update loop
    this->scheduleUpdate();

    const char* track = is_night_mode ? "night_scene.mp3" : "day_scene.mp3";
    background_music_id = cocos2d::AudioEngine::play2d(track, true);

    return true;
}

void GameWorld::setupUserInteraction()
{
    auto unifiedListener = EventListenerTouchOneByOne::create();
    unifiedListener->setSwallowTouches(true);

    unifiedListener->onTouchBegan = [this](Touch* touch, Event* event) {
        // Don't process touches when game is paused
        if (is_paused) return false;

        Vec2 pos = this->convertToNodeSpace(touch->getLocation());

        // Check if touched any sun (highest priority)
        for (auto sun : suns)
        {
            if (sun && sun->isCollectible())
            {
                if (sun->getBoundingBox().containsPoint(pos))
                {
                    sun->collect([this](int sunvalue) {
                        sun_count += sunvalue;
                        updateSunDisplay();
                        });
                    cocos2d::AudioEngine::play2d("sun_pickup_sound.mp3", false);
                    return true;
                }
            }
        }

        for (auto coin : coins) {
            if (coin && coin->isCollectible()) {
                if (coin->getBoundingBox().containsPoint(pos)) {
                    coin->collect([this](int coinvalue) {
                        PlayerProfile::getInstance()->addCoins(coinvalue);
                        updateMoneyBankDisplay();
                        });

                    return true;
                }
            }
        }

        // Check if touched shovel
        if (shovel && shovel->containsPoint(pos)) {
            // Play button click sound
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);

            shovel_selected = true;
            shovel->setDragging(true);
            return true;
        }

        // Check if touched any seed packet
        for (size_t i = 0; i < seed_packets.size(); ++i)
        {
            if (seed_packets[i] && seed_packets[i]->getBoundingBox().containsPoint(pos))
            {
                SeedPacket* packet = seed_packets[i];

                // Check if ready and enough sun
                if (packet->isReady() && sun_count >= packet->getSunCost())
                {
                    // Play button click sound
                    int audioId = cocos2d::AudioEngine::play2d("planting.mp3", false);

                    plant_selected = true;
                    selected_seedpacket_index = i;

                    // Create preview plant
                    preview_plant = packet->createPreviewPlant();
                    if (preview_plant)
                    {
                        preview_plant->setPosition(touch->getLocation());
                        this->addChild(preview_plant, UI_LAYER);
                    }

                    CCLOG("Seed packet %d selected", static_cast<int>(i));
                    return true;
                }
                else
                {
                    // Play buzzer sound for invalid action
                    cocos2d::AudioEngine::play2d("buzzer.mp3", false);

                    CCLOG("Seed packet not ready or not enough sun!");
                    return true;
                }
            }
        }

        return true;
        };

    unifiedListener->onTouchMoved = [this](Touch* touch, Event* event) {
        Vec2 pos = touch->getLocation();

        // Handle shovel dragging
        if (shovel_selected && shovel)
        {
            shovel->updatePosition(pos);
        }

        // Handle preview plant dragging
        if (plant_selected && preview_plant)
        {
            preview_plant->setPosition(pos);
        }
        };

    unifiedListener->onTouchEnded = [this](Touch* touch, Event* event) {
        Vec2 pos = touch->getLocation();

        // Handle shovel removal
        if (shovel_selected)
        {
            Vec2 shovelTipPos = shovel ? shovel->getPosition() : pos;
            bool removed = this->tryRemovePlantAtPosition(shovelTipPos);

            if (removed)
            {
                int audioId = cocos2d::AudioEngine::play2d("planted.mp3", false, 0.3f);
                CCLOG("Plant removed!");
            }
            else
            {
                // Play buzzer sound for invalid removal
                cocos2d::AudioEngine::play2d("buzzer.mp3", false);
                CCLOG("No plant!");
            }

            shovel_selected = false;
            if (shovel) shovel->resetPosition();
            return;
        }

        // Handle planting
        if (plant_selected)
        {
            SeedPacket* selectedPacket = nullptr;
            if (selected_seedpacket_index >= 0 && selected_seedpacket_index < static_cast<int>(seed_packets.size()))
            {
                selectedPacket = seed_packets[selected_seedpacket_index];
            }

            if (selectedPacket)
            {
                bool planted = this->tryPlantAtPosition(pos, selectedPacket);

                if (planted)
                {
                    CCLOG("Plant placed!");
                    // Deduct sun
                    sun_count -= selectedPacket->getSunCost();
                    updateSunDisplay();
                    int audioId = cocos2d::AudioEngine::play2d("planted.mp3", false);
                    // Start cooldown
                    selectedPacket->startCooldown();
                }
                else
                {
                    // Play buzzer sound for invalid planting
                    cocos2d::AudioEngine::play2d("buzzer.mp3", false);
                    CCLOG("Cannot plant!");
                }
            }

            // Remove preview plant
            if (preview_plant)
            {
                this->removeChild(preview_plant);
                preview_plant = nullptr;
            }

            plant_selected = false;
            selected_seedpacket_index = -1;
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(unifiedListener, this);
}

bool GameWorld::tryPlantAtPosition(const Vec2& globalPos, SeedPacket* seedPacket)
{
    int row, col;

    if (!getGridCoordinates(globalPos, row, col)) return false;

    if (hasIceAt(row, col))
    {
        CCLOG("cannot plant on ice!");
        return false;
    }

    PlantName plantName = seedPacket->getPlantName();

    // Check if we need to upgrade an existing plant
    if (plantName == PlantName::TWINSUNFLOWER ||
        plantName == PlantName::GATLINGPEA ||
        plantName == PlantName::SPIKEROCK) {
        if (!plant_grid[row][col]) return false;
        // Use the UpgradedPlant interface to check if we can upgrade
        if (!UpgradedPlant::canUpgradeByName(plantName, plant_grid[row][col])) {
            return false;
        }

        // First remove the base plant
        Plant* basePlant = plant_grid[row][col];
        if (basePlant) {
            this->removeChild(basePlant);
            plant_grid[row][col] = nullptr;
        }

        // Now plant the upgraded plant
        Plant* plant = seedPacket->plantAt(globalPos);
        if (plant)
        {
            this->addChild(plant, PLANT_LAYER);
            plant_grid[row][col] = plant;
            return true;
        }

        return false;
    }
    else if (plant_grid[row][col] != nullptr) {
        // If there's already a plant in this position and it's not an upgrade case
        return false;
    }

    // Use seed packet to create regular plant
    Plant* plant = seedPacket->plantAt(globalPos);
    if (plant)
    {
        this->addChild(plant, PLANT_LAYER);
        plant_grid[row][col] = plant;
        return true;
    }

    return false;
}

bool GameWorld::getGridCoordinates(const Vec2& globalPos, int& outRow, int& outCol) const
{
    int col = static_cast<int>((globalPos.x - GRID_ORIGIN.x) / CELLSIZE.width);
    int row = static_cast<int>((globalPos.y - GRID_ORIGIN.y) / CELLSIZE.height);

    if (col < 0 || col >= MAX_COL || row < 0 || row >= MAX_ROW) return false;

    outRow = row;
    outCol = col;
    return true;
}

bool GameWorld::tryRemovePlantAtPosition(const Vec2& globalPos)
{
    int row, col;

    if (!getGridCoordinates(globalPos, row, col)) return false;

    Plant* plantToRemove = plant_grid[row][col];
    if (plantToRemove == nullptr) return false;

    plantToRemove->takeDamage(10000);

    return true;
}

void GameWorld::update(float delta)
{
    if (!game_started || is_paused || is_gameover)
        return;

    // Update unified time base
    elapsed_time += delta;

    // Update progress bar based on elapsed time
    float progressPercent = (elapsed_time / TOTAL_GAME_TIME) * 100.0f;
    if (progressPercent > 100.0f) progressPercent = 100.0f;

    if (progress_bar) {
        progress_bar->setPercent(progressPercent);

        // Update flag icon position to follow progress bar growth
        if (flag_icon_right) {
            float progressBarWidth = progress_bar->getContentSize().width;
            float currentProgress = progressPercent / 100.0f; // Convert to 0.0 - 1.0
            float progressBarLeftX = progress_bar->getPositionX() + progressBarWidth / 2;
            float flagX = progressBarLeftX - progressBarWidth * currentProgress;
            flag_icon_right->setPosition(Vec2(flagX, progress_bar->getPositionY() + 5));
        }
    }


    float t = elapsed_time / TOTAL_GAME_TIME;
    if (t > 1.0f) t = 1.0f;

    if (!final_wave_triggered && t >= 0.999f)
    {
        final_wave_triggered = true;
        spawnFinalWave();
        next_batch_time_sec = 1e9f; // Stop regular batches
    }
    else if (!final_wave_triggered && elapsed_time >= next_batch_time_sec)
    {
        spawnTimedBatch(t);
    }


    if (!is_night_mode)
    {
        // Sun spawning system (every 5 seconds)
        sun_spawn_timer += delta;
        if (sun_spawn_timer >= 5.0f)
        {
            spawnSunFromSky();
            sun_spawn_timer = 0.0f;
        }
    }

    // Update Plants (Firing logic)
    updatePlants(delta);

    // Update Bullets (Movement and Collision)
    updateBullets(delta);

    // Update Zombies (Movement and Eating)
    updateZombies(delta);

    // Update Suns (Movement and lifetime)
    updateSuns(delta);
    maybePlayZombieGroan(delta);

    // Coin system update
    updateCoins(delta);


    //Update IceTile
    updateIceTiles(delta);

    // Cleanup
    removeDeadPlants();
    removeDeadZombies();
    removeInactiveBullets();
    removeExpiredSuns();
    removeExpiredIceTiles();
    removeExpiredCoins();

    // Clean up off-screen Mowers
    {
        auto vs = Director::getInstance()->getVisibleSize();
        for (int r = 0; r < MAX_ROW; ++r) {
            auto mower = mower_per_row[r];
            if (mower && mower->getPositionX() > vs.width + 100.0f) {
                mower->removeFromParent();
                mower_per_row[r] = nullptr;
            }
        }
    }

    // Victory condition: Final wave has been triggered, all sub-batches have been scheduled, and no "alive" zombies on the field
    // Container doesn't need to be empty, allows dead/dying zombies with animations
    if (!win_shown && final_wave_triggered && final_wave_spawning_done)
    {
        bool anyAlive = false;
        for (int r = 0; r < MAX_ROW && !anyAlive; ++r)
        {
            for (auto* z : zombies_in_row[r])
            {
                if (z && !z->isDead()) { anyAlive = true; break; }
            }
        }
        if (!anyAlive)
        {
            showWinTrophy();
        }
    }
}

void GameWorld::updatePlants(float delta)
{
    for (int row = 0; row < MAX_ROW; ++row)
    {
        for (int col = 0; col < MAX_COL; ++col)
        {
            Plant* plant = plant_grid[row][col];
            if (plant && !plant->isDead())
            {
                PlantCategory category = plant->getCategory();

                switch (category)
                {
                    case PlantCategory::SUN_PRODUCING:
                    {
                        // Sun-producing plants (e.g., Sunflower)
                        SunProducingPlant* sunPlant = dynamic_cast<SunProducingPlant*>(plant);
                        for (auto& sun : sunPlant->produceSun()) {
                            if (sun)
                            {
                                this->addChild(sun, SUN_LAYER);
                                suns.push_back(sun);
                                CCLOG("Sun-producing plant produced sun at position (%.2f, %.2f)",
                                    sun->getPositionX(), sun->getPositionY());
                            }
                        }
                        break;
                    }

                    case PlantCategory::ATTACKING:
                    {
                        // Attacking plants (e.g., PeaShooter, Repeater, ThreePeater, Wallnut)
                        // Pass all zombies to plant, let plant decide which rows to check
                        AttackingPlant* attackPlant = dynamic_cast<AttackingPlant*>(plant);

                        std::vector<Bullet*> newBullets = attackPlant->checkAndAttack(zombies_in_row, row);

                        // Add all created bullets to scene and container
                        for (Bullet* bullet : newBullets)
                        {
                            if (bullet)
                            {
                                this->addChild(bullet, BULLET_LAYER);
                                bullets.push_back(bullet);
                            }
                        }
                        break;
                    }

                    case PlantCategory::BOMB:
                    {
                        // Bomb plants (e.g., CherryBomb)
                        BombPlant* bombPlant = dynamic_cast<BombPlant*>(plant);
                        bombPlant->explode(zombies_in_row, row, col);
                        break;
                    }

                    default:
                        CCLOG("Unknown plant category!");
                        break;
                }
            }
        }
    }
}

void GameWorld::updateBullets(float delta)
{
    for (auto bullet : bullets)
    {
        if (bullet && bullet->isActive())
        {
            // Calculate row from bullet Y
            int row = static_cast<int>((bullet->getPositionY() - GRID_ORIGIN.y) / CELLSIZE.height);

            // Check if bullet is within valid row bounds
            if (row >= 0 && row < MAX_ROW)
            {
                // CRITICAL FIX: Use iterator to avoid invalidation during iteration
                // Iterate zombies in this row using iterator (safe for concurrent modification)
                auto& zombiesInThisRow = zombies_in_row[row];
                for (auto it = zombiesInThisRow.begin(); it != zombiesInThisRow.end(); ++it)
                {
                    Zombie* zombie = *it;
                    // Check pointer validity and skip dead/dying zombies
                    if (zombie && !zombie->isDead())
                    {
                        // Optimization: Simple x-axis check first before full bounding box
                        float bulletX = bullet->getPositionX();
                        float zombieX = zombie->getPositionX();

                        // Check if bullet is close enough to zombie on X axis (assuming bullet moves right)
                        if (bulletX < zombieX + 60 && bulletX > zombieX - 60)
                        {
                            if (bullet->getBoundingBox().intersectsRect(zombie->getBoundingBox()))
                            {
                                // Hit!
                                zombie->takeDamage(static_cast<float>(bullet->getDamage()));
                                bullet->deactivate();

                                // Use virtual function to determine sound effect instead of dynamic_cast
                                if (!zombie->playsMetalHitSound())
                                {
                                    cocos2d::AudioEngine::play2d("bullet_hit.mp3");
                                }
                                else 
                                {
                                    int r = cocos2d::random(1, 3);
                                    switch (r) {
                                        case 1:
                                            cocos2d::AudioEngine::play2d("hittingiron1.mp3");
                                            break;
                                        case 2:
                                            cocos2d::AudioEngine::play2d("hittingiron2.mp3");
                                            break;
                                        case 3:
                                            cocos2d::AudioEngine::play2d("hittingiron3.mp3");
                                            break;
                                        default:
                                            break;
                                    }
                                }
                                break; // Bullet hits one zombie and disappears
                            }
                        }
                    }
                }
            }
        }
    }
}

void GameWorld::updateZombies(float delta)
{
    for (int row = 0; row < MAX_ROW; ++row)
    {
        std::vector<Plant*> plantsInRow;
        for (int col = 0; col < MAX_COL; ++col)
        {
            if (plant_grid[row][col] != nullptr)
            {
                plantsInRow.push_back(plant_grid[row][col]);
            }
        }

        // CRITICAL FIX: Use iterator to avoid invalidation during iteration
        auto& zombiesInThisRow = zombies_in_row[row];
        for (auto it = zombiesInThisRow.begin(); it != zombiesInThisRow.end(); ++it)
        {
            Zombie* zombie = *it;
            // Check pointer validity and skip dead/dying zombies
            if (zombie && !zombie->isDead())
            {
                    // Rake collision: check on this row
                if (rake_per_row[row])
                {
                    auto rake = rake_per_row[row];
                    Rect zbb = zombie->getBoundingBox();
                    Rect rbb = rake->getBoundingBox();
                    if (zbb.intersectsRect(rbb))
                    {
                        zombie->takeDamage(5000); // massive damage
                        rake->trigger(zombie);
                        rake_per_row[row] = nullptr; // one-time
                    }
                }

                // Mower collision (row-based)
                if (mower_per_row[row])
                {
                    auto mower = mower_per_row[row];
                    Rect zbb = zombie->getBoundingBox();
                    Rect mbb = mower->getBoundingBox();
                    if (zbb.intersectsRect(mbb))
                    {
                        if (!mower->isMoving()) {
                            mower->start();
                        } else {
                            // kill zombies that the mower drives through
                            cocos2d::AudioEngine::play2d("limbs-pop.mp3", false, 1.0f);
                            zombie->takeDamage(99999);
                        }
                    }
                }

                // Check if zombie reached the left edge of screen
                float zombieX = zombie->getPositionX();
                if (zombieX <= 0 && !is_gameover)
                {
                    // Game over!
                    showGameOver();
                    return;
                }
                
                zombie->encounterPlant(plantsInRow);
            }
        }
    }
}

void GameWorld::removeDeadPlants()
{
    for (int row = 0; row < MAX_ROW; ++row)
    {
        for (int col = 0; col < MAX_COL; ++col)
        {
            Plant* plant = plant_grid[row][col];
            if (plant != nullptr && plant->isDead())
            {
                this->removeChild(plant);
                plant_grid[row][col] = nullptr;
            }
        }
    }
}

void GameWorld::removeDeadZombies()
{
    for (int row = 0; row < MAX_ROW; ++row)
    {
        auto& zombiesInThisRow = zombies_in_row[row];
        auto it = zombiesInThisRow.begin();
        while (it != zombiesInThisRow.end())
        {
            Zombie* zombie = *it;
            
            // CRITICAL FIX: Only remove truly dead zombies (death animation finished)
            // isDead() returns true only when _isDead == true && _isDying == false
            if (zombie && zombie->isDead())
            {
                spawnCoinAfterZombieDeath(zombie);
                // Safe removal sequence to prevent dangling pointers and double deletion:
                // 1. Get pointer before erasing (iterator will be invalidated after erase)
                Zombie* deadZombie = zombie;
                
                // 2. Remove from container FIRST (prevents phantom collision in other systems)
                // This ensures no other code can access this zombie pointer
                it = zombiesInThisRow.erase(it);
                
                // 3. Remove from scene (zombie has already finished death animation)
                // Double-check pointer is still valid and has correct parent
                if (deadZombie && deadZombie->getParent() == this)
                {
                    // removeChild will handle cleanup and release
                    this->removeChild(deadZombie, true); // true = cleanup
                }
            }
            else
            {
                ++it;
            }
        }
    }
}

void GameWorld::removeInactiveBullets()
{
    auto it = bullets.begin();
    while (it != bullets.end())
    {
        if ((*it) && !(*it)->isActive())
        {
            // Safe removal sequence:
            // 1. Get pointer
            Bullet* b = *it;
            // 2. Remove from container (iterator valid, pointer valid)
            it = bullets.erase(it);
            // 3. Remove from scene (might trigger destructor)
            if (b) b->removeFromParent();
        }
        else
        {
            ++it;
        }
    }
}

void GameWorld::updateMoneyBankDisplay()
{
    if (money_count_label)
    {
        money_count_label->setString(std::to_string(PlayerProfile::getInstance()->getCoins()));
    }
}

void GameWorld::updateCoins(float delta)
{
    for (auto coin : coins) {
        if (coin) {
            coin->update(delta);
        }
    }
}

void GameWorld::removeExpiredCoins()
{
    coins.erase(
        std::remove_if(
            coins.begin(),
            coins.end(),
            [&](Coin* coin)
            {
                if (!coin) return true;

                if (coin->shouldRemove())
                {
                    coin->removeFromParent();
                    return true;
                }
                return false;
            }
        ),
        coins.end()
    );
}


void GameWorld::updateSunDisplay()
{
    if (sun_count_label)
    {
        sun_count_label->setString(std::to_string(sun_count));
    }
}

void GameWorld::updateSuns(float delta)
{
    for (auto sun : suns)
    {
        if (sun)
        {
            sun->update(delta);
        }
    }
}

int GameWorld::randRange(int a, int b)
{
    if (b < a) std::swap(a,b);
    return a + (rand() % (b - a + 1));
}

int GameWorld::applyNightFactor(int baseCount, bool allowZero)
{
    if (!is_night_mode) return baseCount;
    if (baseCount <= 0) return 0;
    float scaled = baseCount * 0.75f; // Reduce overall at night
    int c = static_cast<int>(std::round(scaled));
    if (!allowZero) c = std::max(1, c);
    return std::max(0, c);
}

void GameWorld::spawnSubBatch(int normalCnt, int poleCnt, int bucketHeadCnt, int zamboniCnt, int gargantuarCnt, float delaySec)
{
    this->runAction(Sequence::create(
        DelayTime::create(delaySec),
        CallFunc::create([=]() {
            auto visibleSize = Director::getInstance()->getVisibleSize();
            auto spawnAtRow = [&](cocos2d::Node* z, int row){
                const float ZOMBIE_Y_OFFSET = 0.7f;
                float y = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * ZOMBIE_Y_OFFSET;
                float x = visibleSize.width + 10;
                z->setPosition(Vec2(x, y));
                this->addChild(z, ENEMY_LAYER);
                zombies_in_row[row].push_back(static_cast<Zombie*>(z));
            };

            for (int i = 0; i < normalCnt; ++i) {
                if (auto z = NormalZombie::createZombie()) {
                    int row = rand() % MAX_ROW;
                    spawnAtRow(z, row);
                }
            }
            for (int i = 0; i < poleCnt; ++i) {
                if (auto z = PoleVaulter::createZombie()) {
                    int row = rand() % MAX_ROW;
                    spawnAtRow(z, row);
                }
            }
            for (int i = 0; i < bucketHeadCnt; ++i) {
                if (auto z = BucketHeadZombie::createZombie()) {
                    int row = rand() % MAX_ROW;
                    spawnAtRow(z, row);
                }
            }
            for (int i = 0; i < zamboniCnt; ++i) {
                if (auto z = Zomboni::createZombie()) {
                    int row = rand() % MAX_ROW;
                    spawnAtRow(z, row);
                }
            }
            for (int i = 0; i < gargantuarCnt; ++i) {
                if (auto z = Gargantuar::createZombie()) {
                    int row = rand() % MAX_ROW;
                    spawnAtRow(z, row);
                }
            }
        }),
        nullptr));
}

void GameWorld::spawnTimedBatch(float normalizedTime)
{
    // Phase division
    bool p0 = normalizedTime <= 0.25f;
    bool p1 = (normalizedTime > 0.25f && normalizedTime <= 0.60f);
    bool p2 = (normalizedTime > 0.60f && normalizedTime < 0.999f);

    // Phase parameters
    int normalMin=1, normalMax=2;
    float poleProb = 0.0f;
    float bucketHeadProb = 0.0f; // New bucket head probability
    float zamboniProb = 0.0f;
    float gargantuarProb = 0.0f;
    float intervalSec = 10.0f;
    int subBatches = randRange(3,4);
    float subDelay = 0.9f;

    if (p0) {
        normalMin = 1; normalMax = 2;
        poleProb = 0.0f;
        zamboniProb = 0.0f;
        gargantuarProb = 0.0f;
        intervalSec = 10.0f;
    } else if (p1) {
        normalMin = 3; normalMax = 5;
        poleProb = 0.12f;         // Reduce pole vaulter probability
        bucketHeadProb = 0.12f;   // New bucket head probability
        zamboniProb = 0.10f;      // Reduce zamboni probability
        gargantuarProb = 0.03f;   // Small probability for gargantuar
        intervalSec = 10.0f;
    } else if (p2) {
        normalMin = 3; normalMax = 5;
        poleProb = 0.20f;         // Reduce pole vaulter probability
        bucketHeadProb = 0.25f;   // Increase bucket head probability
        zamboniProb = 0.25f;      // Reduce zamboni probability
        gargantuarProb = 0.08f;   // Slightly increase gargantuar probability
        intervalSec = 12.0f;
    }

    // Adjust probability at night
    float probScale = is_night_mode ? 0.8f : 1.0f;
    poleProb *= probScale;
    bucketHeadProb *= probScale;
    zamboniProb *= probScale;
    gargantuarProb *= probScale;

    // Generate count for this batch
    int normalCnt = randRange(normalMin, normalMax);
    normalCnt = applyNightFactor(normalCnt, false);

    int poleCnt = 0;
    if (CCRANDOM_0_1() < poleProb) poleCnt = 1;

    int zamboniCnt = 0;
    if (CCRANDOM_0_1() < zamboniProb) zamboniCnt = 1;

    int bucketHeadCnt = 0;
    if (CCRANDOM_0_1() < bucketHeadProb) bucketHeadCnt = 1;

    int gargantuarCnt = 0;
    if (CCRANDOM_0_1() < gargantuarProb) gargantuarCnt = 1; // Maximum 1 gargantuar in regular phases

    // Distribute to sub-batches
    int nRemain = normalCnt, pRemain = poleCnt, bRemain = bucketHeadCnt, zRemain = zamboniCnt, gRemain = gargantuarCnt;
    float startDelay = 0.0f;

    auto takePortion = [](int& remain, int slotsLeft){
        if (remain <= 0) return 0;
        int base = remain / slotsLeft;
        int extra = remain % slotsLeft;
        int take = base + (extra > 0 ? 1 : 0);
        remain -= take;
        return take;
    };

    for (int i = 0; i < subBatches; ++i) {
        int slotsLeft = subBatches - i;
        int nThis = takePortion(nRemain, slotsLeft);
        int pThis = takePortion(pRemain, slotsLeft);
        int bThis = takePortion(bRemain, slotsLeft);
        int zThis = takePortion(zRemain, slotsLeft);
        int gThis = takePortion(gRemain, slotsLeft);
        spawnSubBatch(nThis, pThis, bThis, zThis, gThis, startDelay + i * subDelay);
    }

    // Next batch time
    next_batch_time_sec = elapsed_time + intervalSec;
}

void GameWorld::spawnFinalWave()
{
    // Banner (subtitle displays for 4 seconds)
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto banner = Sprite::create("LargeWave.png");
    AudioEngine::play2d("plants-vs-zombies-wave.mp3");
    if (banner) {
        banner->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
        banner->setOpacity(0);
        this->addChild(banner, UI_LAYER + 5);
        banner->runAction(Sequence::create(
            FadeIn::create(0.2f), DelayTime::create(4.0f), FadeOut::create(0.3f),
            CallFunc::create([banner](){ banner->removeFromParent(); }), nullptr));
    }

    float baseDelay = 4.0f; // Start spawning after subtitle plays for 4 seconds

    // Adjust count at night
    int gCount = applyNightFactor(2, false); // Two gargantuars (keep at least 1 at night)

    // Configuration for several sub-batches
    int normal2 = applyNightFactor(randRange(3,5), false);
    int pole2 = (CCRANDOM_0_1() < (is_night_mode ? 0.18f : 0.28f)) ? 1 : 0;

    int zambo3 = applyNightFactor(1, true); // May be 0 (reduced at night)
    int normal3 = applyNightFactor(randRange(2,3), false);

    int normal4 = applyNightFactor(randRange(3,4), false);
    int pole4 = (CCRANDOM_0_1() < (is_night_mode ? 0.16f : 0.24f)) ? 1 : 0;

    int zambo5 = applyNightFactor(1, true);
    int normal5 = applyNightFactor(randRange(2,3), false);

    // Flag zombie
    auto z = FlagZombie::createZombie();
    const float ZOMBIE_Y_OFFSET = 0.7f;
    float y = GRID_ORIGIN.y + 3 * CELLSIZE.height + CELLSIZE.height * ZOMBIE_Y_OFFSET;
    float x = visibleSize.width + 10;
    z->setPosition(Vec2(x, y));
    this->addChild(z, ENEMY_LAYER);
    zombies_in_row[3].push_back(z);
    // Add bucket head zombies in final wave
    int bucket2 = (CCRANDOM_0_1() < (is_night_mode ? 0.25f : 0.35f)) ? 1 : 0;
    int bucket4 = (CCRANDOM_0_1() < (is_night_mode ? 0.20f : 0.30f)) ? 1 : 0;

    // Sub-batches: 0s gargantuar, 1.2s normal+pole+bucket, 2.4s zamboni+normal, 3.6s normal+pole+bucket, 4.8s zamboni+normal (all delayed by 4 seconds)
    spawnSubBatch(0, 0, 0, 0, gCount, baseDelay + 0.0f);
    AudioEngine::play2d("zombies.mp3");
    spawnSubBatch(normal2, pole2, bucket2, 0, 0, baseDelay + 1.2f);
    spawnSubBatch(normal3, 0, 0, zambo3, 0, baseDelay + 2.4f);
    spawnSubBatch(normal4, pole4, bucket4, 0, 0, baseDelay + 3.6f);
    spawnSubBatch(normal5, 0, 0, zambo5, 0, baseDelay + 4.8f);

    // After the last batch is scheduled, mark final wave as all released (delay a little more to ensure scheduling is complete)
    this->runAction(Sequence::create(
        DelayTime::create(baseDelay + 5.0f),
        CallFunc::create([this](){ final_wave_spawning_done = true; }),
        nullptr));
}

void GameWorld::showGameOver()
{
    if (is_gameover) return; // Already showing game over

    is_gameover = true;

    // Stop background music
    if (background_music_id != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(background_music_id);
        background_music_id = cocos2d::AudioEngine::INVALID_AUDIO_ID;
    }

    // Create game over layer
    auto gameOverLayer = Layer::create();
    gameOverLayer->setPosition(Vec2::ZERO);
    this->addChild(gameOverLayer, UI_LAYER + 20);

    // Create game over image
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto gameOverSprite = Sprite::create("gameOver.png");
    
    if (gameOverSprite)
    {
        // Set initial position at center
        gameOverSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        
        // Set initial scale to 0 (invisible)
        gameOverSprite->setScale(0.0f);
        
        // Calculate scale to fill screen while maintaining aspect ratio
        float scaleX = visibleSize.width / gameOverSprite->getContentSize().width;
        float scaleY = visibleSize.height / gameOverSprite->getContentSize().height;
        float targetScale = MAX(scaleX, scaleY);
        
        // Add to layer
        cocos2d::AudioEngine::stopAll();
        gameOverLayer->addChild(gameOverSprite, WIN_LOSE_LAYER);
        AudioEngine::play2d("lose-music-pvz1.mp3");

        AudioEngine::play2d("pvz-crazy-dave-nooooooooooo.mp3");


        // Create scale animation (slowly enlarge from center)
        auto scaleAction = ScaleTo::create(1.0f, targetScale);
        
        // After animation completes, wait 3 seconds then return to menu
        auto delayAction = DelayTime::create(8.0f);
        auto callbackAction = CallFunc::create([this]() {
            // Stop all audio
            cocos2d::AudioEngine::stopAll();

            // Reset time scale to normal
            Director::getInstance()->getScheduler()->setTimeScale(1.0f);
            speed_level = 0;
            
            // Return to main menu with smooth transition
            auto scene = GameMenu::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
        });
        
        // Run sequence: scale animation -> delay -> return to menu
        auto sequence = Sequence::create(scaleAction, delayAction, callbackAction, nullptr);
        gameOverSprite->runAction(sequence);
        
        CCLOG("Game Over! Showing game over screen...");
    }
    else
    {
        // Fallback if image fails to load
        problemLoading("'gameOver.png'");
        
        // Still return to menu after delay
        auto delayAction = DelayTime::create(3.0f);
        auto callbackAction = CallFunc::create([this]() {
            cocos2d::AudioEngine::stopAll();
            Director::getInstance()->getScheduler()->setTimeScale(1.0f);
            speed_level = 0;
            auto scene = GameMenu::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
        });
        auto sequence = Sequence::create(delayAction, callbackAction, nullptr);
        gameOverLayer->runAction(sequence);
    }
}

void GameWorld::showWinTrophy()
{
    if (win_shown) return; // Prevent duplicate calls
    win_shown = true;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    cocos2d::AudioEngine::stopAll();
    AudioEngine::play2d("pvz-victory.mp3");
    trophy_sprite = Sprite::create("trophy.png");
    if (!trophy_sprite)
        return;

    trophy_sprite->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
    trophy_sprite->setScale(1.0f);
    this->addChild(trophy_sprite, UI_LAYER + 30);

    // Scale to appropriate size
    float targetScale = 0.7f;
    trophy_sprite->runAction(ScaleTo::create(0.6f, targetScale));

    // Independent click listener to ensure clickability
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        if (!trophy_sprite) return false;
        Vec2 p = trophy_sprite->getParent()->convertToNodeSpace(touch->getLocation());
        return trophy_sprite->getBoundingBox().containsPoint(p);
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event){
        // Return to main menu after clicking trophy
        returnToMenu(nullptr);
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, trophy_sprite);
}

void GameWorld::toggleSpeedMode(Ref* sender)
{
    speed_level = (speed_level + 1) % 3;

    if (speed_toggle_button)
    {
        speed_toggle_button->setSelectedIndex(speed_level);
    }

    if (!is_paused)
    {
        float timeScale = 1.0f;
        if (speed_level == 1) timeScale = 2.0f;
        else if (speed_level == 2) timeScale = 3.0f;
        Director::getInstance()->getScheduler()->setTimeScale(timeScale);
    }
}

void GameWorld::showPauseMenu(Ref* sender)
{
    if (is_paused) return;

    is_paused = true;
    Director::getInstance()->pause();
    cocos2d::AudioEngine::play2d("pause_menu.mp3", false);
    pause_menu_layer = Layer::create();
    pause_menu_layer->setPosition(Vec2::ZERO);
    this->addChild(pause_menu_layer, UI_LAYER + 10);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto background = Sprite::create("Menu.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        float scaleX = visibleSize.width * 0.8f / background->getContentSize().width;
        float scaleY = visibleSize.height * 0.8f / background->getContentSize().height;
        background->setScale(MIN(scaleX, scaleY));
        pause_menu_layer->addChild(background, 0);
    }
    else
    {
        auto fallbackBackground = LayerColor::create(Color4B(0, 0, 0, 128));
        pause_menu_layer->addChild(fallbackBackground, 0);
    }

    auto resumeItem = MenuItemFont::create("Resume", [this](Ref* sender) {
        cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
        resumeGame(sender);
    });
    auto restartItem = MenuItemFont::create("Restart", [this](Ref* sender) {
        cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
        restartGame(sender);
    });
    auto menuItem = MenuItemFont::create("Main Menu", [this](Ref* sender) {
        cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
        returnToMenu(sender);
    });

    auto volumeUpItem = MenuItemFont::create("Volume +", [this](Ref* sender) {
        if (music_volume >= 1.0f) {
            cocos2d::AudioEngine::play2d("buzzer.mp3", false);
        } else {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            increaseMusicVolume(sender);
        }
    });
    auto volumeDownItem = MenuItemFont::create("Volume -", [this](Ref* sender) {
        if (music_volume <= 0.0f) {
            cocos2d::AudioEngine::play2d("buzzer.mp3", false);
        } else {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            decreaseMusicVolume(sender);
        }
    });

    volume_label = Label::createWithSystemFont(StringUtils::format("Volume: %.0f%%", music_volume * 100), "Arial", 24);
    volume_label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 100));
    volume_label->setColor(Color3B::WHITE);
    pause_menu_layer->addChild(volume_label);

    resumeItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 120));
    restartItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 60));
    menuItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    volumeUpItem->setPosition(Vec2(visibleSize.width / 2 - 60, visibleSize.height / 2 - 60));
    volumeDownItem->setPosition(Vec2(visibleSize.width / 2 + 60, visibleSize.height / 2 - 60));

    resumeItem->setScale(0.8f);
    restartItem->setScale(0.8f);
    menuItem->setScale(0.8f);
    volumeUpItem->setScale(0.7f);
    volumeDownItem->setScale(0.7f);

    pause_menu = Menu::create(resumeItem, restartItem, menuItem, volumeUpItem, volumeDownItem, nullptr);
    pause_menu->setPosition(Vec2::ZERO);
    pause_menu_layer->addChild(pause_menu);
}

void GameWorld::resumeGame(Ref* sender)
{
    if (!is_paused) return;

    is_paused = false;
    Director::getInstance()->resume();

    if (speed_level > 0)
    {
        float timeScale = speed_level == 1 ? 2.0f : 3.0f;
        Director::getInstance()->getScheduler()->setTimeScale(timeScale);
    }

    if (pause_menu_layer)
    {
        this->removeChild(pause_menu_layer);
        pause_menu_layer = nullptr;
        pause_menu = nullptr;
    }
}

void GameWorld::restartGame(Ref* sender)
{
    if (background_music_id != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(background_music_id);
        background_music_id = cocos2d::AudioEngine::INVALID_AUDIO_ID;
    }

    if (is_paused)
    {
        Director::getInstance()->resume();
        is_paused = false;
    }

    Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    speed_level = 0;

    // Restart from card selection scene
    auto newScene = SelectCardsScene::createScene(is_night_mode);
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, newScene));
}

void GameWorld::returnToMenu(Ref* sender)
{
    if (background_music_id != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(background_music_id);
        background_music_id = cocos2d::AudioEngine::INVALID_AUDIO_ID;
    }

    if (is_paused)
    {
        Director::getInstance()->resume();
        is_paused = false;
    }

    Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    speed_level = 0;

    cocos2d::AudioEngine::stopAll();

    auto scene = GameMenu::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
}

void GameWorld::increaseMusicVolume(Ref* sender)
{
    music_volume = MIN(music_volume + 0.1f, 1.0f);
    cocos2d::AudioEngine::setVolume(background_music_id, music_volume);
    if (volume_label)
    {
        volume_label->setString(StringUtils::format("Volume: %.0f%%", music_volume * 100));
    }
}

void GameWorld::decreaseMusicVolume(Ref* sender)
{
    music_volume = MAX(music_volume - 0.1f, 0.0f);
    cocos2d::AudioEngine::setVolume(background_music_id, music_volume);
    if (volume_label)
    {
        volume_label->setString(StringUtils::format("Volume: %.0f%%", music_volume * 100));
    }
}

void GameWorld::spawnSunFromSky()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    int targetCol = rand() % MAX_COL;
    float startY = 700.0f;
    Sun* sun = Sun::createFromSky(targetCol, startY);
    if (sun)
    {
        this->addChild(sun, SUN_LAYER);
        suns.push_back(sun);
    }
}

void GameWorld::maybePlayZombieGroan(float delta)
{
    bool hasZombie = false;
    for (int row = 0; row < MAX_ROW && !hasZombie; ++row)
    {
        auto& zombiesInThisRow = zombies_in_row[row];
        for (auto it = zombiesInThisRow.begin(); it != zombiesInThisRow.end() && !hasZombie; ++it)
        {
            Zombie* zombie = *it;
            if (zombie && !zombie->isDead())
            {
                hasZombie = true;
                break;
            }
        }
    }

    if (!hasZombie)
    {
        zombie_groan_timer = 1.0f;
        return;
    }

    zombie_groan_timer -= delta;
    if (zombie_groan_timer <= 0.0f)
    {
        if (CCRANDOM_0_1() < 0.05f)
        {
            cocos2d::AudioEngine::play2d("zombie_groan.mp3", false);
        }
        zombie_groan_timer = 10.0f;
    }
}

void GameWorld::removeExpiredSuns()
{
    suns.erase(
        std::remove_if(
            suns.begin(),
            suns.end(),
            [&](Sun* sun)
            {
                if (!sun) return true;

                if (sun->shouldRemove())
                {
                    sun->removeFromParent();
                    return true;
                }
                return false;
            }
        ),
        suns.end()
    );
}

void GameWorld::addZombie(Zombie* z)
{
   float y = z->getPositionY();
   int row = static_cast<int>((y - CELLSIZE.height * 0.7f - GRID_ORIGIN.y) / CELLSIZE.height);
   zombies_in_row[row].push_back(z);
}

void GameWorld::addIceTile(IceTile* ice)
{
    this->addChild(ice, ICE_LAYER);
    ice_tiles.push_back(ice);
}

void GameWorld::updateIceTiles(float delta)
{
    for (auto ice : ice_tiles)
    {
        ice->update(delta);
    }
}

void GameWorld::removeExpiredIceTiles()
{
    ice_tiles.erase(
        std::remove_if(ice_tiles.begin(), ice_tiles.end(),
            [](IceTile* ice)
            {
                if (!ice) return true;

                if (ice->isExpired())
                {
                    ice->removeFromParent();
                    return true;
                }
                return false;
            }),
        ice_tiles.end()
    );
}

bool GameWorld::hasIceAt(int row,int col) const
{
    for (auto ice : ice_tiles)
    {
        if (!ice) continue;
        if (ice->isExpired()) continue;
        
        int iceRow, iceCol;
        if (!getGridCoordinates(ice->getPosition(), iceRow, iceCol))
            continue;

        if (iceRow== row && iceCol==col)
        {
            return true;
        }
    }
    return false;
}

void GameWorld::removeIceInRow(int row)
{
    for (auto& ice : ice_tiles) {
        if (ice && ice->getRow() == row) {
            ice->markAsExpired();
        }
    }
}

void GameWorld::spawnCoinAfterZombieDeath(Zombie* zombie)
{
    if (!zombie)
        return;
    
    // Use virtual function to get coin drop bonus instead of dynamic_cast
    float possibilityBonus = zombie->getCoinDropBonus();
    float r = CCRANDOM_0_1();

    float silver = 0.4f, gold = 0.2f, diamond = 0.05f;
    if (r <= possibilityBonus * diamond) {
        auto coin = Coin::create(Coin::CoinType::DIAMOND);
        if (coin) {
            coin->setPosition(zombie->getPosition());
            this->addChild(coin, SUN_LAYER);
            coins.push_back(coin);
        }
    }
    else if (r <= possibilityBonus * gold) {
        auto coin = Coin::create(Coin::CoinType::GOLD);
        if (coin) {
            coin->setPosition(zombie->getPosition());
            this->addChild(coin, SUN_LAYER);
            coins.push_back(coin);
        }
    }
    else if (r <= possibilityBonus * silver) {
        auto coin = Coin::create(Coin::CoinType::SILVER);
        if (coin) {
            coin->setPosition(zombie->getPosition());
            this->addChild(coin, SUN_LAYER);
            coins.push_back(coin);
        }
    }

}
