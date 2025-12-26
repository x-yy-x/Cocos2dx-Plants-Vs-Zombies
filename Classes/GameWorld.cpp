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
        instance->_isNightMode = isNightMode;
        instance->_initialPlantNames = plantNames;
        if (instance->init())
        {
            instance->autorelease();
            return instance;
        }
        delete instance;
    }
    return nullptr;
}

Scene* GameWorld::createScene(bool isNightMode, const std::vector<PlantName>& plantNames)
{
    return GameWorld::create(isNightMode, plantNames);
}

GameWorld::~GameWorld()
{
    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
    }

    // Clean up pause menu resources
    if (_pauseMenuLayer)
    {
        _pauseMenuLayer = nullptr;
        _pauseMenu = nullptr;
        _volumeLabel = nullptr;
    }

    // Reset game state
    _isPaused = false;
    _speedLevel = 0;
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

    // 修复报错：使用 C++11 兼容方式遍历 map
    if (_initialPlantNames.empty()) {
        for (std::map<PlantName, PlantConfig>::const_iterator it = SeedPacket::CONFIG_TABLE.begin();
            it != SeedPacket::CONFIG_TABLE.end(); ++it)
        {
            _initialPlantNames.push_back(it->first); // it->first 是 PlantName
        }
    }

    // 统一循环添加
    for (size_t i = 0; i < _initialPlantNames.size(); ++i) {
        SeedPacket* packet = SeedPacket::createFromConfig(_initialPlantNames[i]);
        if (packet) {
            packet->setPosition(Vec2(baseX + i * spacing, baseY));
            this->addChild(packet, SEEDPACKET_LAYER);
            _seedPackets.push_back(packet);
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
    _backgroundMusicId = cocos2d::AudioEngine::INVALID_AUDIO_ID;

    // Initialize plant grid (all cells empty at start)
    for (int row = 0; row < MAX_ROW; ++row)
    {
        for (int col = 0; col < MAX_COL; ++col)
        {
            _plantGrid[row][col] = nullptr;
        }
    }
    _plantSelected = false;
    _selectedSeedPacketIndex = -1;
    _previewPlant = nullptr;
    _shovelSelected = false;
    _shovel = nullptr;
    _shovelBack = nullptr;
    _sunCount = 200; // Initial sun count
    _sunCountLabel = nullptr;

    // Initialize timed batch spawning (方案D)
    _currentWave = 0; // legacy
    _nextBatchTimeSec = 8.0f; // 首批在8秒左右
    _finalWaveTriggered = false;
    _gameStarted = true;

    // Initialize sun spawning system
    _sunSpawnTimer = 0.0f;
    _zombieGroanTimer = 3.0f;

    // 初始化 Rake/Mower 槽
    for (int r = 0; r < MAX_ROW; ++r) { _rakePerRow[r] = nullptr; _mowerPerRow[r] = nullptr; }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create background
    auto backGround = BackGround::create(_isNightMode);
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
            _rakePerRow[rakeRow] = rake;
        }
    }

    // Spawn Mowers per row at far left if enabled
    if (PlayerProfile::getInstance()->isMowerEnabled())
    {
        for (int r = 0; r < MAX_ROW; ++r)
        {
            if (_mowerPerRow[r]) continue;
            auto mower = Mower::create();
            if (mower)
            {
                float y = GRID_ORIGIN.y + r * CELLSIZE.height + CELLSIZE.height * 0.6f;
                float x = GRID_ORIGIN.x - 30.0f; // just left to the first cell
                mower->setPosition(Vec2(x, y));
                this->addChild(mower, ENEMY_LAYER);
                _mowerPerRow[r] = mower;
            }
        }
    }

    auto progressBG = Sprite::create("FlagMeterEmpty.png");
    if (progressBG) {
        progressBG->setPosition(Vec2(visibleSize.width - 150, 40));
        this->addChild(progressBG, UI_LAYER);

        _progressBar = ui::LoadingBar::create("FlagMeterFull.png");

        _progressBar->setDirection(ui::LoadingBar::Direction::RIGHT);

        _progressBar->setPercent(0);

        _progressBar->setPosition(progressBG->getPosition());
        this->addChild(_progressBar, UI_LAYER + 1);

        // Add flag icon at the left end of the progress bar (FlagMeterParts1.png)
        auto flagIconLeft = Sprite::create("FlagMeterParts2.png");
        if (flagIconLeft) {
            flagIconLeft->setPosition(Vec2(progressBG->getPositionX() - progressBG->getContentSize().width / 2 + 4, progressBG->getPositionY() + 5));
            this->addChild(flagIconLeft, UI_LAYER + 2);
        }
      
        // Add flag icon at the right end of the progress bar (FlagMeterParts1.png) - moves with progress
        _flagIconRight = Sprite::create("FlagMeterParts1.png");
        if (_flagIconRight) {
            // Initial position at left end (0% progress)
            _flagIconRight->setPosition(Vec2(progressBG->getPositionX() - progressBG->getContentSize().width / 2 + 4, progressBG->getPositionY() + 5));
            this->addChild(_flagIconRight, UI_LAYER + 2);
        }
    }
    

    _elapsedTime = 0.0f;
	initSeedPackets();

    // Create sun counter label
    _sunCountLabel = Label::createWithSystemFont(std::to_string(_sunCount), "Arial", 20);
    if (_sunCountLabel)
    {
        _sunCountLabel->setPosition(Vec2(100, visibleSize.height - 90));
        _sunCountLabel->setColor(Color3B::BLACK);
        this->addChild(_sunCountLabel, UI_LAYER);
    }

   //金币系统
    _moneyCountLabel = Label::createWithSystemFont(std::to_string(PlayerProfile::getInstance()->getCoins()), "Arial", 20);
    if (_moneyCountLabel)
    {
        _moneyCountLabel->setPosition(Vec2(100, 20));
        _moneyCountLabel->setColor(Color3B::BLACK);
        this->addChild(_moneyCountLabel, UI_LAYER);
    }

    _coinBank = Sprite::create("CoinBank.png");
    if (_coinBank == nullptr)
    {
        problemLoading("'Coinbank.png'");
    }
    else
    {
        _coinBank->setPosition(Vec2(100, 20));
        this->addChild(_coinBank, SEEDPACKET_LAYER);
    }

    // Create shovel background
    _shovelBack = Sprite::create("ShovelBack.png");
    if (_shovelBack == nullptr)
    {
        problemLoading("'ShovelBack.png'");
    }
    else
    {
        _shovelBack->setPosition(Vec2(visibleSize.width - 465, visibleSize.height - 80));
        this->addChild(_shovelBack, SEEDPACKET_LAYER);
    }

    // Create shovel
    _shovel = Shovel::create();
    if (_shovel == nullptr)
    {
        problemLoading("'Shovel.png'");
    }
    else
    {
        Vec2 shovelPos = _shovelBack ? _shovelBack->getPosition() : Vec2(visibleSize.width - 500, visibleSize.height - 80);
        _shovel->setOriginalPosition(shovelPos);
        this->addChild(_shovel, SEEDPACKET_LAYER + 1);
    }

    // Create pause button
    _pauseButton = MenuItemImage::create(
        "btn_Menu.png",  // Normal state
        "btn_Menu2.png", // Selected state
        [this](Ref* sender) {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            showPauseMenu(sender);
        }
    );

    if (_pauseButton)
    {
        _pauseButton->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 40));
        _pauseButton->setScale(0.8f);

        auto pauseMenu = Menu::create(_pauseButton, nullptr);
        pauseMenu->setPosition(Vec2::ZERO);
        this->addChild(pauseMenu, UI_LAYER);
    }

    // Create speed mode toggle button (below pause button)
    auto speedNormalItem = MenuItemFont::create("Normal Speed", CC_CALLBACK_1(GameWorld::toggleSpeedMode, this));
    auto speed2xItem = MenuItemFont::create("2x Speed", CC_CALLBACK_1(GameWorld::toggleSpeedMode, this));
    auto speed3xItem = MenuItemFont::create("3x Speed", CC_CALLBACK_1(GameWorld::toggleSpeedMode, this));
    _speedToggleButton = MenuItemToggle::createWithCallback(
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
    speedButtonBack->setScale(1.3f,0.85f);
	this->addChild(speedButtonBack, UI_LAYER - 1);
    if (_speedToggleButton)
    {
        _speedToggleButton->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 80));
        _speedToggleButton->setScale(0.8f);

        auto speedMenu = Menu::create(_speedToggleButton, nullptr);
        speedMenu->setPosition(Vec2::ZERO);
        this->addChild(speedMenu, UI_LAYER);
    }

    
    // debug mode
    bool debug = true;
    if (debug) {
        // DEBUG:rake
        /*auto rake = Rake::create();
        if (rake)
        {
            float y = GRID_ORIGIN.y + 2 * CELLSIZE.height + CELLSIZE.height * 0.6f;
            float x = visibleSize.width - 500.0f;
            rake->setPosition(Vec2(x, y));
            this->addChild(rake, ENEMY_LAYER);
            _rakePerRow[2] = rake;
        }*/

        // DEBUG: Spawn one zombie at start for testing
        // TODO: Remove this before final release
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
                _zombiesInRow[row].push_back(debugZombie);
                CCLOG("DEBUG: Spawned test zombie at row %d", row);
            }
        }

        {
            auto debugCoin0 = Coin::create(Coin::CoinType::SILVER);
            if (debugCoin0) {
                debugCoin0->setPosition(Vec2(300, 300));
                this->addChild(debugCoin0, SUN_LAYER);
                _coins.push_back(debugCoin0);
            }
            auto debugCoin1 = Coin::create(Coin::CoinType::GOLD);
            if (debugCoin1) {
                debugCoin1->setPosition(Vec2(500, 300));
                this->addChild(debugCoin1, SUN_LAYER);
                _coins.push_back(debugCoin1);
            }
            auto debugCoin2 = Coin::create(Coin::CoinType::DIAMOND);
            if (debugCoin2) {
                debugCoin2->setPosition(Vec2(800, 300));
                this->addChild(debugCoin2, SUN_LAYER);
                _coins.push_back(debugCoin2);
            }
        }
    }
    // Setup user interaction
    setupUserInteraction();


    // Enable update loop
    this->scheduleUpdate();

    const char* track = _isNightMode ? "night_scene.mp3" : "day_scene.mp3";
    _backgroundMusicId = cocos2d::AudioEngine::play2d(track, true);
    
    return true;
}

void GameWorld::setupUserInteraction()
{
    auto unifiedListener = EventListenerTouchOneByOne::create();
    unifiedListener->setSwallowTouches(true); 

    unifiedListener->onTouchBegan = [this](Touch* touch, Event* event) {
        // Don't process touches when game is paused
        if (_isPaused) return false;

        Vec2 pos = this->convertToNodeSpace(touch->getLocation());

        // Check if touched any sun (highest priority)
        for (auto sun : _suns)
        {
            if (sun && sun->isCollectible())
            {
                if (sun->getBoundingBox().containsPoint(pos))
                {
                    sun->collect([this](int sunvalue) {
                        _sunCount += sunvalue;
                        updateSunDisplay();
                        });
                    cocos2d::AudioEngine::play2d("sun_pickup_sound.mp3", false);
                    return true;
                }
            }
        }

        for (auto coin : _coins) {
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
        if (_shovel && _shovel->containsPoint(pos)) {
            // Play button click sound
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);

            _shovelSelected = true;
            _shovel->setDragging(true);
            return true;
        }

        // Check if touched any seed packet
        for (size_t i = 0; i < _seedPackets.size(); ++i)
        {
            if (_seedPackets[i] && _seedPackets[i]->getBoundingBox().containsPoint(pos))
            {
                SeedPacket* packet = _seedPackets[i];

                // Check if ready and enough sun
                if (packet->isReady() && _sunCount >= packet->getSunCost())
                {
                    // Play button click sound
                    int audioId = cocos2d::AudioEngine::play2d("planting.mp3", false);

                    _plantSelected = true;
                    _selectedSeedPacketIndex = i;

                    // Create preview plant
                    _previewPlant = packet->createPreviewPlant();
                    if (_previewPlant)
                    {
                        _previewPlant->setPosition(touch->getLocation());
                        this->addChild(_previewPlant, UI_LAYER);
                    }

                    CCLOG("Seed packet %d selected", (int)i);
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
        if (_shovelSelected && _shovel)
        {
            _shovel->updatePosition(pos);
        }

        // Handle preview plant dragging
        if (_plantSelected && _previewPlant)
        {
            _previewPlant->setPosition(pos);
        }
    };

    unifiedListener->onTouchEnded = [this](Touch* touch, Event* event) {
        Vec2 pos = touch->getLocation();

        // Handle shovel removal
        if (_shovelSelected)
        {
            Vec2 shovelTipPos = _shovel ? _shovel->getPosition() : pos;
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

            _shovelSelected = false;
            if (_shovel) _shovel->resetPosition();
            return;
        }

        // Handle planting
        if (_plantSelected)
        {
            SeedPacket* selectedPacket = nullptr;
            if (_selectedSeedPacketIndex >= 0 && _selectedSeedPacketIndex < (int)_seedPackets.size())
            {
                selectedPacket = _seedPackets[_selectedSeedPacketIndex];
            }

            if (selectedPacket)
            {
                bool planted = this->tryPlantAtPosition(pos, selectedPacket);

                if (planted)
                {
                    CCLOG("Plant placed!");
                    // Deduct sun
                    _sunCount -= selectedPacket->getSunCost();
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
            if (_previewPlant)
            {
                this->removeChild(_previewPlant);
                _previewPlant = nullptr;
            }

            _plantSelected = false;
            _selectedSeedPacketIndex = -1;
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(unifiedListener, this);
}

bool GameWorld::tryPlantAtPosition(const Vec2& globalPos, SeedPacket* seedPacket)
{
    int row, col;
    
    if (!getGridCoordinates(globalPos, row, col)) return false;

    if (hasIceAt(row,col))
    {
        CCLOG("cannot plant on ice!");
        return false;
    }

    PlantName plantName = seedPacket->getPlantName();
    
    // Check if we need to upgrade an existing plant
    if (plantName == PlantName::TWINSUNFLOWER || 
        plantName == PlantName::GATLINGPEA || 
        plantName == PlantName::SPIKEROCK) {
        if (!_plantGrid[row][col]) return false;
        // Use the UpgradedPlant interface to check if we can upgrade
        if (!UpgradedPlant::canUpgradeByName(plantName, _plantGrid[row][col])) {
            return false;
        }
        
        // 先移除基础植物
        Plant* basePlant = _plantGrid[row][col];
        if (basePlant) {
            this->removeChild(basePlant);
            _plantGrid[row][col] = nullptr;
        }        
        
        // 现在种植升级植物
        Plant* plant = seedPacket->plantAt(globalPos);
        if (plant)
        {
            this->addChild(plant, PLANT_LAYER);
            _plantGrid[row][col] = plant;
            return true;
        }
        
        return false;
    }
    else if (_plantGrid[row][col] != nullptr) {
        // If there's already a plant in this position and it's not an upgrade case
        return false;
    }

    // Use seed packet to create regular plant
    Plant* plant = seedPacket->plantAt(globalPos);
    if (plant)
    {
        this->addChild(plant, PLANT_LAYER);
        _plantGrid[row][col] = plant;
        return true;
    }

    return false;
}

bool GameWorld::getGridCoordinates(const Vec2& globalPos, int& outRow, int& outCol)
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

    Plant* plantToRemove = _plantGrid[row][col];
    if (plantToRemove == nullptr) return false;

    plantToRemove->takeDamage(10000);

    return true;
}

void GameWorld::update(float delta)
{
    if (!_gameStarted || _isPaused || _isGameOver)
        return;

    // Update unified time base
    _elapsedTime += delta;

    // Update progress bar based on elapsed time
    float progressPercent = (_elapsedTime / TOTAL_GAME_TIME) * 100.0f;
    if (progressPercent > 100.0f) progressPercent = 100.0f;

    if (_progressBar) {
        _progressBar->setPercent(progressPercent);
        
        // Update flag icon position to follow progress bar growth
        if (_flagIconRight) {
            float progressBarWidth = _progressBar->getContentSize().width;
            float currentProgress = progressPercent / 100.0f; // Convert to 0.0 - 1.0
            float progressBarLeftX = _progressBar->getPositionX() + progressBarWidth / 2;
            float flagX = progressBarLeftX - progressBarWidth * currentProgress;
            _flagIconRight->setPosition(Vec2(flagX, _progressBar->getPositionY() + 5));
        }
    }


    // 方案D：按时间脚本触发批次
    float t = _elapsedTime / TOTAL_GAME_TIME;
    if (t > 1.0f) t = 1.0f;

    if (!_finalWaveTriggered && t >= 0.999f)
    {
        _finalWaveTriggered = true;
        spawnFinalWave();
        _nextBatchTimeSec = 1e9f; // 停止常规批次
    }
    else if (!_finalWaveTriggered && _elapsedTime >= _nextBatchTimeSec)
    {
        spawnTimedBatch(t);
    }
    

    if (!_isNightMode)
    {
        // Sun spawning system (every 5 seconds)
        _sunSpawnTimer += delta;
        if (_sunSpawnTimer >= 5.0f)
        {
            spawnSunFromSky();
            _sunSpawnTimer = 0.0f;
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

    // 金币系统更新
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

    // 清理离屏 Mower
    {
        auto vs = Director::getInstance()->getVisibleSize();
        for (int r = 0; r < MAX_ROW; ++r) {
            auto mower = _mowerPerRow[r];
            if (mower && mower->getPositionX() > vs.width + 100.0f) {
                mower->removeFromParent();
                _mowerPerRow[r] = nullptr;
            }
        }
    }

    // 胜利判定：最终波已触发并且所有子批已排程完成，且场上无“存活中的”僵尸
    // 不要求容器为空，允许仍存在已死亡/正在死亡动画中的僵尸对象
    if (!_winShown && _finalWaveTriggered && _finalWaveSpawningDone)
    {
        bool anyAlive = false;
        for (int r = 0; r < MAX_ROW && !anyAlive; ++r)
        {
            for (auto* z : _zombiesInRow[r])
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
            Plant* plant = _plantGrid[row][col];
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
                            _suns.push_back(sun);
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
                    
                    std::vector<Bullet*> newBullets = attackPlant->checkAndAttack(_zombiesInRow, row);
                    
                    // Add all created bullets to scene and container
                    for (Bullet* bullet : newBullets)
                    {
                        if (bullet)
                        {
                            this->addChild(bullet, BULLET_LAYER);
                            _bullets.push_back(bullet);
                        }
                    }
                    break;
                }

                case PlantCategory::BOMB:
                {
                    // Bomb plants (e.g., CherryBomb)
                    BombPlant* bombPlant = dynamic_cast<BombPlant*>(plant);
                    bombPlant->explode(_zombiesInRow, row, col);
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
    for (auto bullet : _bullets)
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
                auto& zombiesInThisRow = _zombiesInRow[row];
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
                                zombie->takeDamage(bullet->getDamage());
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
            if (_plantGrid[row][col] != nullptr)
            {
                plantsInRow.push_back(_plantGrid[row][col]);
            }
        }

        // CRITICAL FIX: Use iterator to avoid invalidation during iteration
        auto& zombiesInThisRow = _zombiesInRow[row];
        for (auto it = zombiesInThisRow.begin(); it != zombiesInThisRow.end(); ++it)
        {
            Zombie* zombie = *it;
            // Check pointer validity and skip dead/dying zombies
            if (zombie && !zombie->isDead())
            {
                    // Rake collision: check on this row
                if (_rakePerRow[row])
                {
                    auto rake = _rakePerRow[row];
                    Rect zbb = zombie->getBoundingBox();
                    Rect rbb = rake->getBoundingBox();
                    if (zbb.intersectsRect(rbb))
                    {
                        zombie->takeDamage(5000); // massive damage
                        rake->trigger(zombie);
                        _rakePerRow[row] = nullptr; // one-time
                    }
                }

                // Mower collision (row-based)
                if (_mowerPerRow[row])
                {
                    auto mower = _mowerPerRow[row];
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
                if (zombieX <= 0 && !_isGameOver)
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
            Plant* plant = _plantGrid[row][col];
            if (plant != nullptr && plant->isDead())
            {
                this->removeChild(plant);
                _plantGrid[row][col] = nullptr;
            }
        }
    }
}

void GameWorld::removeDeadZombies()
{
    for (int row = 0; row < MAX_ROW; ++row)
    {
        auto& zombiesInThisRow = _zombiesInRow[row];
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
    auto it = _bullets.begin();
    while (it != _bullets.end())
    {
        if ((*it) && !(*it)->isActive())
        {
            // Safe removal sequence:
            // 1. Get pointer
            Bullet* b = *it;
            // 2. Remove from container (iterator valid, pointer valid)
            it = _bullets.erase(it);
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
    if (_moneyCountLabel)
    {
        _moneyCountLabel->setString(std::to_string(PlayerProfile::getInstance()->getCoins()));
    }
}

void GameWorld::updateCoins(float delta)
{
    for (auto coin : _coins) {
        if (coin) {
            coin->update(delta);
        }
    }
}

void GameWorld::removeExpiredCoins()
{
    _coins.erase(
        std::remove_if(
            _coins.begin(),
            _coins.end(),
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
        _coins.end()
    );
}

/*
// 根据阳光数量刷新种子卡片颜色（冷却结束但阳光不足时保持 50% 灰度）
void GameWorld::refreshSeedPacketColors()
{
    for (auto packet : _seedPackets)
    {
        if (!packet) continue;

        // 只有在冷却结束后才根据阳光数量亮/灰
        if (packet->isReady())
        {
            if (_sunCount >= packet->getSunCost())
            {
                packet->setColor(cocos2d::Color3B::WHITE); // 亮色
            }
            else
            {
                packet->setColor(cocos2d::Color3B(128,128,128)); // 50% 灰
            }
        }
        // 冷却中的卡片颜色由 SeedPacket 自己控制
    }
}
*/
void GameWorld::updateSunDisplay()
{
    if (_sunCountLabel)
    {
        _sunCountLabel->setString(std::to_string(_sunCount));
    }
}

void GameWorld::updateSuns(float delta)
{
    for (auto sun : _suns)
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
    if (!_isNightMode) return baseCount;
    if (baseCount <= 0) return 0;
    float scaled = baseCount * 0.75f; // 夜间整体减少
    int c = (int)std::round(scaled);
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
                _zombiesInRow[row].push_back(static_cast<Zombie*>(z));
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
    // 阶段划分
    bool p0 = normalizedTime <= 0.25f;
    bool p1 = (normalizedTime > 0.25f && normalizedTime <= 0.60f);
    bool p2 = (normalizedTime > 0.60f && normalizedTime < 0.999f);

    // 阶段参数
    int normalMin=1, normalMax=2;
    float poleProb = 0.0f;
    float bucketHeadProb = 0.0f; // 新增铁桶概率
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
        poleProb = 0.12f;         // 降低撑杆概率
        bucketHeadProb = 0.12f;   // 新增铁桶概率
        zamboniProb = 0.10f;      // 降低冰车概率
        gargantuarProb = 0.03f;   // 小概率出现巨人
        intervalSec = 10.0f;
    } else if (p2) {
        normalMin = 3; normalMax = 5;
        poleProb = 0.20f;         // 降低撑杆概率
        bucketHeadProb = 0.25f;   // 提高铁桶概率
        zamboniProb = 0.25f;      // 降低冰车概率
        gargantuarProb = 0.08f;   // 略提高巨人概率
        intervalSec = 12.0f;
    }

    // 夜间调整概率
    float probScale = _isNightMode ? 0.8f : 1.0f;
    poleProb *= probScale;
    bucketHeadProb *= probScale;
    zamboniProb *= probScale;
    gargantuarProb *= probScale;

    // 生成这一批的数量
    int normalCnt = randRange(normalMin, normalMax);
    normalCnt = applyNightFactor(normalCnt, false);

    int poleCnt = 0;
    if (CCRANDOM_0_1() < poleProb) poleCnt = 1;

    int zamboniCnt = 0;
    if (CCRANDOM_0_1() < zamboniProb) zamboniCnt = 1;

    int bucketHeadCnt = 0;
    if (CCRANDOM_0_1() < bucketHeadProb) bucketHeadCnt = 1;

    int gargantuarCnt = 0;
    if (CCRANDOM_0_1() < gargantuarProb) gargantuarCnt = 1; // 常规阶段最多1个巨人

    // 分发到子批
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

    // 下一批时间
    _nextBatchTimeSec = _elapsedTime + intervalSec;
}

void GameWorld::spawnFinalWave()
{
    // 提示图（字幕显示4秒）
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

    float baseDelay = 4.0f; // 字幕播放4秒后再开始生成

    // 夜间数量调整
    int gCount = applyNightFactor(2, false); // 两个巨人（夜间保持至少1）

    // 几个子批的配置
    int normal2 = applyNightFactor(randRange(3,5), false);
    int pole2 = (CCRANDOM_0_1() < (_isNightMode ? 0.18f : 0.28f)) ? 1 : 0;

    int zambo3 = applyNightFactor(1, true); // 可能为0（夜间减少）
    int normal3 = applyNightFactor(randRange(2,3), false);

    int normal4 = applyNightFactor(randRange(3,4), false);
    int pole4 = (CCRANDOM_0_1() < (_isNightMode ? 0.16f : 0.24f)) ? 1 : 0;

    int zambo5 = applyNightFactor(1, true);
    int normal5 = applyNightFactor(randRange(2,3), false);

    // 旗帜僵尸
    auto z = FlagZombie::createZombie();
    const float ZOMBIE_Y_OFFSET = 0.7f;
    float y = GRID_ORIGIN.y + 3 * CELLSIZE.height + CELLSIZE.height * ZOMBIE_Y_OFFSET;
    float x = visibleSize.width + 10;
    z->setPosition(Vec2(x, y));
    this->addChild(z, ENEMY_LAYER);
    _zombiesInRow[3].push_back(z);
    // 最终波加入铁桶僵尸
    int bucket2 = (CCRANDOM_0_1() < (_isNightMode ? 0.25f : 0.35f)) ? 1 : 0;
    int bucket4 = (CCRANDOM_0_1() < (_isNightMode ? 0.20f : 0.30f)) ? 1 : 0;

    // 子批：0s巨人、1.2s普通+撑杆+铁桶、2.4s冰车+普通、3.6s普通+撑杆+铁桶、4.8s冰车+普通（都整体延后4秒）
    spawnSubBatch(0, 0, 0, 0, gCount, baseDelay + 0.0f);
    AudioEngine::play2d("zombies.mp3");
    spawnSubBatch(normal2, pole2, bucket2, 0, 0, baseDelay + 1.2f);
    spawnSubBatch(normal3, 0, 0, zambo3, 0, baseDelay + 2.4f);
    spawnSubBatch(normal4, pole4, bucket4, 0, 0, baseDelay + 3.6f);
    spawnSubBatch(normal5, 0, 0, zambo5, 0, baseDelay + 4.8f);

    // 在最后一批计划完毕后，标记最终波已全部释放完成（再稍微延迟一点点，确保排程添加完成）
    this->runAction(Sequence::create(
        DelayTime::create(baseDelay + 5.0f),
        CallFunc::create([this](){ _finalWaveSpawningDone = true; }),
        nullptr));
}

void GameWorld::showGameOver()
{
    if (_isGameOver) return; // Already showing game over

    _isGameOver = true;

    // Stop background music
    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
        _backgroundMusicId = cocos2d::AudioEngine::INVALID_AUDIO_ID;
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
            _speedLevel = 0;
            
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
            _speedLevel = 0;
            auto scene = GameMenu::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
        });
        auto sequence = Sequence::create(delayAction, callbackAction, nullptr);
        gameOverLayer->runAction(sequence);
    }
}

void GameWorld::showWinTrophy()
{
    if (_winShown) return; // 防止重复调用
    _winShown = true;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    cocos2d::AudioEngine::stopAll();
    AudioEngine::play2d("pvz-victory.mp3");
    _trophySprite = Sprite::create("trophy.png");
    if (!_trophySprite)
        return;

    _trophySprite->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
    _trophySprite->setScale(1.0f);
    this->addChild(_trophySprite, UI_LAYER + 30);

    // 放大到适中尺寸
    float targetScale = 0.7f;
    _trophySprite->runAction(ScaleTo::create(0.6f, targetScale));

    // 独立的点击监听，确保可点击
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        if (!_trophySprite) return false;
        Vec2 p = _trophySprite->getParent()->convertToNodeSpace(touch->getLocation());
        return _trophySprite->getBoundingBox().containsPoint(p);
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event){
        // 点击奖杯后回到主菜单
        returnToMenu(nullptr);
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _trophySprite);
}

void GameWorld::toggleSpeedMode(Ref* sender)
{
    _speedLevel = (_speedLevel + 1) % 3;

    if (_speedToggleButton)
    {
        _speedToggleButton->setSelectedIndex(_speedLevel);
    }

    if (!_isPaused)
    {
        float timeScale = 1.0f;
        if (_speedLevel == 1) timeScale = 2.0f;
        else if (_speedLevel == 2) timeScale = 3.0f;
        Director::getInstance()->getScheduler()->setTimeScale(timeScale);
    }
}

void GameWorld::showPauseMenu(Ref* sender)
{
    if (_isPaused) return;

    _isPaused = true;
    Director::getInstance()->pause();
    cocos2d::AudioEngine::play2d("pause_menu.mp3", false);
    _pauseMenuLayer = Layer::create();
    _pauseMenuLayer->setPosition(Vec2::ZERO);
    this->addChild(_pauseMenuLayer, UI_LAYER + 10);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto background = Sprite::create("Menu.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        float scaleX = visibleSize.width * 0.8f / background->getContentSize().width;
        float scaleY = visibleSize.height * 0.8f / background->getContentSize().height;
        background->setScale(MIN(scaleX, scaleY));
        _pauseMenuLayer->addChild(background, 0);
    }
    else
    {
        auto fallbackBackground = LayerColor::create(Color4B(0, 0, 0, 128));
        _pauseMenuLayer->addChild(fallbackBackground, 0);
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
        if (_musicVolume >= 1.0f) {
            cocos2d::AudioEngine::play2d("buzzer.mp3", false);
        } else {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            increaseMusicVolume(sender);
        }
    });
    auto volumeDownItem = MenuItemFont::create("Volume -", [this](Ref* sender) {
        if (_musicVolume <= 0.0f) {
            cocos2d::AudioEngine::play2d("buzzer.mp3", false);
        } else {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            decreaseMusicVolume(sender);
        }
    });

    _volumeLabel = Label::createWithSystemFont(StringUtils::format("Volume: %.0f%%", _musicVolume * 100), "Arial", 24);
    _volumeLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 100));
    _volumeLabel->setColor(Color3B::WHITE);
    _pauseMenuLayer->addChild(_volumeLabel);

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

    _pauseMenu = Menu::create(resumeItem, restartItem, menuItem, volumeUpItem, volumeDownItem, nullptr);
    _pauseMenu->setPosition(Vec2::ZERO);
    _pauseMenuLayer->addChild(_pauseMenu);
}

void GameWorld::resumeGame(Ref* sender)
{
    if (!_isPaused) return;

    _isPaused = false;
    Director::getInstance()->resume();

    if (_speedLevel > 0)
    {
        float timeScale = _speedLevel == 1 ? 2.0f : 3.0f;
        Director::getInstance()->getScheduler()->setTimeScale(timeScale);
    }

    if (_pauseMenuLayer)
    {
        this->removeChild(_pauseMenuLayer);
        _pauseMenuLayer = nullptr;
        _pauseMenu = nullptr;
    }
}

void GameWorld::restartGame(Ref* sender)
{
    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
        _backgroundMusicId = cocos2d::AudioEngine::INVALID_AUDIO_ID;
    }

    if (_isPaused)
    {
        Director::getInstance()->resume();
        _isPaused = false;
    }

    Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    _speedLevel = 0;

    // Restart from card selection scene
    auto newScene = SelectCardsScene::createScene(_isNightMode);
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, newScene));
}

void GameWorld::returnToMenu(Ref* sender)
{
    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
        _backgroundMusicId = cocos2d::AudioEngine::INVALID_AUDIO_ID;
    }

    if (_isPaused)
    {
        Director::getInstance()->resume();
        _isPaused = false;
    }

    Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    _speedLevel = 0;

    cocos2d::AudioEngine::stopAll();

    auto scene = GameMenu::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
}

void GameWorld::increaseMusicVolume(Ref* sender)
{
    _musicVolume = MIN(_musicVolume + 0.1f, 1.0f);
    cocos2d::AudioEngine::setVolume(_backgroundMusicId, _musicVolume);
    if (_volumeLabel)
    {
        _volumeLabel->setString(StringUtils::format("Volume: %.0f%%", _musicVolume * 100));
    }
}

void GameWorld::decreaseMusicVolume(Ref* sender)
{
    _musicVolume = MAX(_musicVolume - 0.1f, 0.0f);
    cocos2d::AudioEngine::setVolume(_backgroundMusicId, _musicVolume);
    if (_volumeLabel)
    {
        _volumeLabel->setString(StringUtils::format("Volume: %.0f%%", _musicVolume * 100));
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
        _suns.push_back(sun);
    }
}

void GameWorld::maybePlayZombieGroan(float delta)
{
    bool hasZombie = false;
    for (int row = 0; row < MAX_ROW && !hasZombie; ++row)
    {
        auto& zombiesInThisRow = _zombiesInRow[row];
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
        _zombieGroanTimer = 1.0f;
        return;
    }

    _zombieGroanTimer -= delta;
    if (_zombieGroanTimer <= 0.0f)
    {
        if (CCRANDOM_0_1() < 0.05f)
        {
            cocos2d::AudioEngine::play2d("zombie_groan.mp3", false);
        }
        _zombieGroanTimer = 10.0f;
    }
}

void GameWorld::removeExpiredSuns()
{
    _suns.erase(
        std::remove_if(
            _suns.begin(),
            _suns.end(),
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
        _suns.end()
    );
}

void GameWorld::addZombie(Zombie* z)
{
   float y = z->getPositionY();
   int row = static_cast<int>((y - CELLSIZE.height * 0.7f - GRID_ORIGIN.y) / CELLSIZE.height);
   _zombiesInRow[row].push_back(z);
}

void GameWorld::addIceTile(IceTile* ice)
{
    this->addChild(ice, ICE_LAYER);
    _iceTiles.push_back(ice);
}

void GameWorld::updateIceTiles(float delta)
{
    for (auto ice : _iceTiles)
    {
        ice->update(delta);
    }
}

void GameWorld::removeExpiredIceTiles()
{
    _iceTiles.erase(
        std::remove_if(_iceTiles.begin(), _iceTiles.end(),
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
        _iceTiles.end()
    );
}

bool GameWorld::hasIceAt(int row,int col)
{
    for (auto ice : _iceTiles)
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
    for (auto& ice : _iceTiles) {
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
            _coins.push_back(coin);
        }
    }
    else if (r <= possibilityBonus * gold) {
        auto coin = Coin::create(Coin::CoinType::GOLD);
        if (coin) {
            coin->setPosition(zombie->getPosition());
            this->addChild(coin, SUN_LAYER);
            _coins.push_back(coin);
        }
    }
    else if (r <= possibilityBonus * silver) {
        auto coin = Coin::create(Coin::CoinType::SILVER);
        if (coin) {
            coin->setPosition(zombie->getPosition());
            this->addChild(coin, SUN_LAYER);
            _coins.push_back(coin);
        }
    }

}
