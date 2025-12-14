#include "GameWorld.h"
#include "BackGround.h"
#include "Plant.h"
#include "SunProducingPlant.h"
#include "AttackingPlant.h"
#include "BombPlant.h"
#include "PeaShooter.h"
#include "Repeater.h"
#include "ThreePeater.h"
#include "Sunflower.h"
#include "Sunshroom.h"
#include "Puffshroom.h"
#include "Puff.h"
#include "Wallnut.h"
#include "CherryBomb.h"
#include "Zombie.h"
#include "Shovel.h"
#include "Bullet.h"
#include "Pea.h"
#include "SeedPacket.h"
#include "Sun.h"
#include "PoleVaulter.h"
#include "FlagZombie.h"
#include "Imp.h"
#include "Gargantuar.h"
#include "GameMenu.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "audio/include/AudioEngine.h"

// CCRANDOM
#include "base/ccRandom.h"

USING_NS_CC;

GameWorld* GameWorld::create(bool isNightMode)
{
    GameWorld* instance = new (std::nothrow) GameWorld();
    if (instance)
    {
        instance->_isNightMode = isNightMode;
        if (instance->init())
        {
            instance->autorelease();
            return instance;
        }
        delete instance;
    }
    return nullptr;
}

Scene* GameWorld::createScene(bool isNightMode)
{
    return GameWorld::create(isNightMode);
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
    _isSpeedMode = false;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
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
    _sunCount = 10000; // Initial sun count
    _sunCountLabel = nullptr;

    // Initialize wave spawning system
    _currentWave = 0;
    _tickCount = 0;
    _nextWaveTickCount = FIRST_WAVE_TICK;
    _gameStarted = true;

    // Initialize sun spawning system
    _sunSpawnTimer = 0.0f;
    _zombieGroanTimer = 3.0f;

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

    // Create seed packets using template method (no need for separate subclasses!)
    auto sunflowerPacket = SeedPacket::create<Sunflower>("seedpacket_sunflower.png", 3.0f, 50);
    auto sunshroomPacket = SeedPacket::create<Sunshroom>("seedpacket_sunshroom.png", 3.0f, 25);
    auto peashooterPacket = SeedPacket::create<PeaShooter>("seedpacket_peashooter.png", 7.5f, 100);
    auto repeaterPacket = SeedPacket::create<Repeater>("seedpacket_repeater.png", 3.0f, 200);
    auto threepeaterPacket = SeedPacket::create<ThreePeater>("Threepeater_Seed_Packet_PC.png", 3.0f, 325);
    auto puffshroomPacket = SeedPacket::create<Puffshroom>("seedpacket_puffshroom.png", 3.0f, 0);
    auto wallnutPacket = SeedPacket::create<Wallnut>("seedpacket_wallnut.png", 30.0f, 50);
    auto cherryBombPacket = SeedPacket::create<CherryBomb>("seedpacket_cherry_bomb.png", 1.0f, 150);

    if (sunflowerPacket && sunshroomPacket && peashooterPacket && repeaterPacket && threepeaterPacket && puffshroomPacket && wallnutPacket && cherryBombPacket)
    {
        _seedPackets.push_back(sunflowerPacket);
        _seedPackets.push_back(sunshroomPacket);
        _seedPackets.push_back(peashooterPacket);
        _seedPackets.push_back(repeaterPacket);
        _seedPackets.push_back(threepeaterPacket);
        _seedPackets.push_back(puffshroomPacket);
        _seedPackets.push_back(wallnutPacket);
        _seedPackets.push_back(cherryBombPacket);

        // Set positions for seed packets (more compact spacing)
        float baseX = 187.0f;
        float baseY = 667.0f;
        float spacing = 65.0f;  // Reduced spacing for more compact layout

        for (size_t i = 0; i < _seedPackets.size(); ++i)
        {
            _seedPackets[i]->setPosition(Vec2(baseX + i * spacing, baseY));
            this->addChild(_seedPackets[i], SEEDPACKET_LAYER);
        }
    }

    // Create sun counter label
    _sunCountLabel = Label::createWithSystemFont(std::to_string(_sunCount), "Arial", 20);
    if (_sunCountLabel)
    {
        _sunCountLabel->setPosition(Vec2(100, visibleSize.height - 90));
        _sunCountLabel->setColor(Color3B::BLACK);
        this->addChild(_sunCountLabel, UI_LAYER);
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
    auto speedOnItem = MenuItemFont::create("2x Speed", CC_CALLBACK_1(GameWorld::toggleSpeedMode, this));
    auto speedOffItem = MenuItemFont::create("Normal Speed", CC_CALLBACK_1(GameWorld::toggleSpeedMode, this));
    _speedToggleButton = MenuItemToggle::createWithCallback(
        [this](Ref* sender) {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            toggleSpeedMode(sender);
        },
        speedOffItem,
        speedOnItem,
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
                    int sunValue = sun->collect();
                    _sunCount += sunValue;
                    updateSunDisplay();
                    cocos2d::AudioEngine::play2d("sun_pickup_sound.mp3", false);
                    CCLOG("Sun collected! +%d sun (Total: %d)", sunValue, _sunCount);
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
                    cocos2d::AudioEngine::play2d("buttonclick.mp3", false);

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
                int audioId = cocos2d::AudioEngine::play2d("planting.mp3", false);
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
                    int audioId = cocos2d::AudioEngine::play2d("planting.mp3", false);
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

    if (_plantGrid[row][col] != nullptr) return false;

    // Use seed packet to create plant
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

    this->removeChild(plantToRemove);
    _plantGrid[row][col] = nullptr;

    return true;
}

void GameWorld::update(float delta)
{
    if (!_gameStarted || _isPaused)
        return;

    _tickCount++;

    if (_tickCount >= _nextWaveTickCount)
    {
        _currentWave++;
        spawnZombieWave(_currentWave);

        int interval = std::max(MIN_WAVE_INTERVAL, 900 - 30 * _currentWave);
        _nextWaveTickCount = _tickCount + interval;
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

    // Cleanup
    removeDeadPlants();
    removeDeadZombies();
    removeInactiveBullets();
    removeExpiredSuns();
}

void GameWorld::spawnZombieWave(int waveNumber)
{
    int zombieCount = static_cast<int>(5 + 3 * waveNumber);

    auto visibleSize = Director::getInstance()->getVisibleSize();

    for (int i = 0; i < zombieCount; ++i)
    {
        auto zombie = Zombie::createZombie();
        auto pole_vaulter = PoleVaulter::createZombie();
        if (zombie&&pole_vaulter)
        {
            int row = rand() % MAX_ROW;
            int type = rand() % 2;
            const float ZOMBIE_Y_OFFSET = 0.7f;
            float y = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * ZOMBIE_Y_OFFSET;
            float x = visibleSize.width + 10;

            if (type == 0) {
                CCLOG("normal zombie created");
                zombie->setPosition(Vec2(x, y));
                this->addChild(zombie, ENEMY_LAYER);
                _zombiesInRow[row].push_back(zombie);
            }
            else if (type == 1) {
                CCLOG("pole vaulter created");
                pole_vaulter->setPosition(Vec2(x, y));
                this->addChild(pole_vaulter, ENEMY_LAYER);
                _zombiesInRow[row].push_back(pole_vaulter);
            }
            
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
                    SunProducingPlant* sunPlant = static_cast<SunProducingPlant*>(plant);
                    Sun* sun = sunPlant->produceSun();
                    if (sun)
                    {
                        this->addChild(sun, SUN_LAYER);
                        _suns.push_back(sun);
                        CCLOG("Sun-producing plant produced sun at position (%.2f, %.2f)",
                              sun->getPositionX(), sun->getPositionY());
                    }
                    break;
                }

                case PlantCategory::ATTACKING:
                {
                    // Attacking plants (e.g., PeaShooter, Repeater, ThreePeater, Wallnut)
                    // Pass all zombies to plant, let plant decide which rows to check
                    AttackingPlant* attackPlant = static_cast<AttackingPlant*>(plant);
                    
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
                    BombPlant* bombPlant = static_cast<BombPlant*>(plant);
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
                // Iterate zombies in this row
                for (auto zombie : _zombiesInRow[row])
                {
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
                                cocos2d::AudioEngine::play2d("bullet_hit.mp3", false);
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

        for (auto zombie : _zombiesInRow[row])
        {
            if (zombie && !zombie->isDead())
            {
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
            if ((*it) && (*it)->isDead())
            {
                it = zombiesInThisRow.erase(it);
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

void GameWorld::removeExpiredSuns()
{
    _suns.erase(
        std::remove_if(
            _suns.begin(),
            _suns.end(),
            [&](Sun* sun)
            {
                if (!sun) return true;  // �Ƴ���Чָ��

                if (sun->shouldRemove())
                {
                    sun->removeFromParent();
                    return true; // �� vector ɾ��
                }
                return false;
            }
        ),
        _suns.end()
    );
}

void GameWorld::spawnSunFromSky()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    // Random grid column
    int targetCol = rand() % MAX_COL;
    
    // Start position: above seed packets (around y = 700)
    float startY = 700.0f;
    
    // Create sun from sky
    Sun* sun = Sun::createFromSky(targetCol, startY);
    if (sun)
    {
        this->addChild(sun, SUN_LAYER);
        _suns.push_back(sun);
        CCLOG("Sun spawned from sky, target column: %d", targetCol);
    }
}

void GameWorld::maybePlayZombieGroan(float delta)
{
    bool hasZombie = false;
    for (int row = 0; row < MAX_ROW && !hasZombie; ++row)
    {
        for (auto zombie : _zombiesInRow[row])
        {
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
        if (CCRANDOM_0_1() < 0.05f)  // Reduced to 5% chance
        {
            cocos2d::AudioEngine::play2d("zombie_groan.mp3", false);
        }
        _zombieGroanTimer = 10.0f;  // Fixed 10 second interval
    }
}

void GameWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}

void GameWorld::toggleSpeedMode(Ref* sender)
{
    _isSpeedMode = !_isSpeedMode;
    // Only apply time scale when not paused
    if (!_isPaused)
    {
        float timeScale = _isSpeedMode ? _speedScale : 1.0f;
        Director::getInstance()->getScheduler()->setTimeScale(timeScale);
    }

    CCLOG("Speed mode %s, time scale: %.1f", _isSpeedMode ? "ON" : "OFF", _isSpeedMode ? _speedScale : 1.0f);
}

void GameWorld::showPauseMenu(Ref* sender)
{
    if (_isPaused) return; // Already paused

    _isPaused = true;

    // Pause the entire scene to stop all game updates
    Director::getInstance()->pause();

    // Create pause menu layer
    _pauseMenuLayer = Layer::create();
    _pauseMenuLayer->setPosition(Vec2::ZERO);
    this->addChild(_pauseMenuLayer, UI_LAYER + 10);

    // Create menu background image
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto background = Sprite::create("Menu.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        // Scale to fit screen while maintaining aspect ratio
        float scaleX = visibleSize.width * 0.8f / background->getContentSize().width;
        float scaleY = visibleSize.height * 0.8f / background->getContentSize().height;
        background->setScale(MIN(scaleX, scaleY));
        _pauseMenuLayer->addChild(background, 0);
    }
    else
    {
        // Fallback to semi-transparent background if image fails to load
        auto fallbackBackground = LayerColor::create(Color4B(0, 0, 0, 128));
        _pauseMenuLayer->addChild(fallbackBackground, 0);
    }

    // Create pause menu items
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

    // Volume controls with validity checking
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

    // Create volume display label
    _volumeLabel = Label::createWithSystemFont(StringUtils::format("Volume: %.0f%%", _musicVolume * 100), "Arial", 24);
    _volumeLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 100));
    _volumeLabel->setColor(Color3B::WHITE);
    _pauseMenuLayer->addChild(_volumeLabel);

    // Position menu items relative to screen center (they will appear on the menu background)
    resumeItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 120));
    restartItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 60));
    menuItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    volumeUpItem->setPosition(Vec2(visibleSize.width / 2 - 60, visibleSize.height / 2 - 60));
    volumeDownItem->setPosition(Vec2(visibleSize.width / 2 + 60, visibleSize.height / 2 - 60));

    // Scale items
    resumeItem->setScale(0.8f);
    restartItem->setScale(0.8f);
    menuItem->setScale(0.8f);
    volumeUpItem->setScale(0.7f);
    volumeDownItem->setScale(0.7f);

    // Create menu
    _pauseMenu = Menu::create(resumeItem, restartItem, menuItem, volumeUpItem, volumeDownItem, nullptr);
    _pauseMenu->setPosition(Vec2::ZERO);
    _pauseMenuLayer->addChild(_pauseMenu);

    CCLOG("Game paused");
}

void GameWorld::resumeGame(Ref* sender)
{
    if (!_isPaused) return;

    _isPaused = false;

    // Resume the entire scene
    Director::getInstance()->resume();

    // Restore speed mode time scale if speed mode is active
    if (_isSpeedMode)
    {
        Director::getInstance()->getScheduler()->setTimeScale(_speedScale);
    }

    // Remove pause menu
    if (_pauseMenuLayer)
    {
        this->removeChild(_pauseMenuLayer);
        _pauseMenuLayer = nullptr;
        _pauseMenu = nullptr;
    }

    CCLOG("Game resumed");
}

void GameWorld::restartGame(Ref* sender)
{
    // Stop current background music
    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
        _backgroundMusicId = cocos2d::AudioEngine::INVALID_AUDIO_ID;
    }

    // Ensure game is not paused when restarting
    if (_isPaused)
    {
        Director::getInstance()->resume();
        _isPaused = false;
    }

    // Reset time scale to normal
    Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    _isSpeedMode = false;

    // Create new game scene with smooth transition
    auto newScene = GameWorld::createScene(_isNightMode);
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, newScene));
}

void GameWorld::returnToMenu(Ref* sender)
{
    // Stop current background music
    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
        _backgroundMusicId = cocos2d::AudioEngine::INVALID_AUDIO_ID;
    }

    // Ensure game is not paused when returning to menu
    if (_isPaused)
    {
        Director::getInstance()->resume();
        _isPaused = false;
    }

    // Reset time scale to normal
    Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    _isSpeedMode = false;

    // Stop all audio to ensure clean state
    cocos2d::AudioEngine::stopAll();

    // Return to main menu with smooth transition
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
    CCLOG("Background music volume increased to: %.1f", _musicVolume);
}

void GameWorld::decreaseMusicVolume(Ref* sender)
{
    _musicVolume = MAX(_musicVolume - 0.1f, 0.0f);
    cocos2d::AudioEngine::setVolume(_backgroundMusicId, _musicVolume);
    if (_volumeLabel)
    {
        _volumeLabel->setString(StringUtils::format("Volume: %.0f%%", _musicVolume * 100));
    }
    CCLOG("Background music volume decreased to: %.1f", _musicVolume);
}

void GameWorld::addZombie(Zombie* z)
{
   float y = z->getPositionY();
   int row = static_cast<int>((y - CELLSIZE.height * 0.7f - GRID_ORIGIN.y) / CELLSIZE.height);
   _zombiesInRow[row].push_back(z);
   this->addChild(z, ENEMY_LAYER);
}
