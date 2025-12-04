#include "GameWorld.h"
#include "BackGround.h"
#include "Plant.h"
#include "PeaShooter.h"
#include "Sunflower.h"
#include "Wallnut.h"
#include "Zombie.h"
#include "Shovel.h"
#include "Bullet.h"
#include "Pea.h"
#include "SeedPacket.h"
#include "PeaShooterSeedPacket.h"
#include "SunflowerSeedPacket.h"
#include "WallnutSeedPacket.h"
#include "Sun.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

USING_NS_CC;

Scene* GameWorld::createScene()
{
    return GameWorld::create();
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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Create background
    auto backGround = BackGround::create();
    if (backGround == nullptr)
    {
        problemLoading("'background.png'");
        return false;
    }
    this->addChild(backGround, BACKGROUND_LAYER);

    // Create seed packets
    auto sunflowerPacket = SunflowerSeedPacket::create();
    auto peashooterPacket = PeaShooterSeedPacket::create();
    auto wallnutPacket = WallnutSeedPacket::create();

    if (sunflowerPacket && peashooterPacket && wallnutPacket)
    {
        _seedPackets.push_back(sunflowerPacket);
        _seedPackets.push_back(peashooterPacket);
        _seedPackets.push_back(wallnutPacket);

        // Set positions for seed packets (more compact spacing)
        float baseX = 187.0f;
        float baseY = 667.0f;
        float spacing = 65.0f;  // Reduced spacing for more compact layout

        for (size_t i = 0; i < _seedPackets.size(); ++i)
        {
            _seedPackets[i]->setPosition(Vec2(baseX + i * spacing, baseY));
            this->addChild(_seedPackets[i], 5); // High layer for UI
        }
    }

    // Create sun counter label
    _sunCountLabel = Label::createWithSystemFont(std::to_string(_sunCount), "Arial", 20);
    if (_sunCountLabel)
    {
        _sunCountLabel->setPosition(Vec2(100, visibleSize.height - 90));
        _sunCountLabel->setColor(Color3B::BLACK);
        this->addChild(_sunCountLabel, 10); // Very high layer for UI
    }

    // Create shovel background
    _shovelBack = Sprite::create("ShovelBack.png");
    if (_shovelBack == nullptr)
    {
        problemLoading("'ShovelBack.png'");
    }
    else
    {
        _shovelBack->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 80));
        this->addChild(_shovelBack, 5);
    }

    // Create shovel
    _shovel = Shovel::create();
    if (_shovel == nullptr)
    {
        problemLoading("'Shovel.png'");
    }
    else
    {
        Vec2 shovelPos = _shovelBack ? _shovelBack->getPosition() : Vec2(visibleSize.width - 100, visibleSize.height - 80);
        _shovel->setOriginalPosition(shovelPos);
        this->addChild(_shovel, 6);
    }

    // DEBUG: Spawn one zombie at start for testing
    // TODO: Remove this before final release
    {
        auto debugZombie = Zombie::createZombie();
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

    return true;
}

void GameWorld::setupUserInteraction()
{
    auto unifiedListener = EventListenerTouchOneByOne::create();
    unifiedListener->setSwallowTouches(true); 

    unifiedListener->onTouchBegan = [this](Touch* touch, Event* event) {
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
                    CCLOG("Sun collected! +%d sun (Total: %d)", sunValue, _sunCount);
                    return true;
                }
            }
        }

        // Check if touched shovel
        if (_shovel && _shovel->containsPoint(pos)) {
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
                    _plantSelected = true;
                    _selectedSeedPacketIndex = i;

                    // Create preview plant
                    _previewPlant = packet->createPreviewPlant();
                    if (_previewPlant)
                    {
                        _previewPlant->setPosition(touch->getLocation());
                        this->addChild(_previewPlant, 10); // Very high layer
                    }

                    CCLOG("Seed packet %d selected", (int)i);
                    return true;
                }
                else
                {
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
            
            if (removed) CCLOG("Plant removed!");
            else CCLOG("No plant!");

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
                    // Start cooldown
                    selectedPacket->startCooldown();
                }
                else
                {
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
    int col = (globalPos.x - GRID_ORIGIN.x) / CELLSIZE.width;
    int row = (globalPos.y - GRID_ORIGIN.y) / CELLSIZE.height;

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
    if (!_gameStarted)
        return;

    _tickCount++;

    if (_tickCount >= _nextWaveTickCount)
    {
        _currentWave++;
        spawnZombieWave(_currentWave);

        int interval = std::max(MIN_WAVE_INTERVAL, 900 - 30 * _currentWave);
        _nextWaveTickCount = _tickCount + interval;
    }

    // Sun spawning system (every 5 seconds)
    _sunSpawnTimer += delta;
    if (_sunSpawnTimer >= 5.0f)
    {
        spawnSunFromSky();
        _sunSpawnTimer = 0.0f;
    }

    // Update Plants (Firing logic)
    updatePlants(delta);

    // Update Bullets (Movement and Collision)
    updateBullets(delta);

    // Update Zombies (Movement and Eating)
    updateZombies(delta);

    // Update Suns (Movement and lifetime)
    updateSuns(delta);

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
        if (zombie)
        {
            int row = rand() % MAX_ROW;
            const float ZOMBIE_Y_OFFSET = 0.7f;
            float y = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * ZOMBIE_Y_OFFSET;
            float x = visibleSize.width + 50 + (i * 50);

            zombie->setPosition(Vec2(x, y));
            this->addChild(zombie, ENEMY_LAYER);
            _zombiesInRow[row].push_back(zombie);
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
                // Check if plant is a Sunflower
                Sunflower* sunflower = dynamic_cast<Sunflower*>(plant);
                if (sunflower)
                {
                    // Try to produce sun
                    Sun* sun = sunflower->produceSun();
                    if (sun)
                    {
                        this->addChild(sun, SUNLAYER);
                        _suns.push_back(sun);
                        CCLOG("Sunflower produced sun at position (%.2f, %.2f)", sun->getPositionX(), sun->getPositionY());
                    }
                }
                else
                {
                    // For attacking plants, check for zombies
                    bool zombieDetected = false;
                    float plantX = plant->getPositionX();
                    
                    for (auto zombie : _zombiesInRow[row])
                    {
                        if (zombie && !zombie->isDead() && zombie->getPositionX() > plantX)
                        {
                            zombieDetected = true;
                            break;
                        }
                    }

                    if (zombieDetected)
                    {
                        // Try to attack (generic for all plants)
                        Bullet* bullet = plant->attack();
                        if (bullet)
                        {
                            // Add bullet to scene and container
                            this->addChild(bullet, BULLET_LAYER);
                            _bullets.push_back(bullet);
                        }
                    }
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
            int row = (bullet->getPositionY() - GRID_ORIGIN.y) / CELLSIZE.height;
            
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
    auto it = _suns.begin();
    while (it != _suns.end())
    {
        if ((*it) && (*it)->shouldRemove())
        {
            Sun* sun = *it;
            it = _suns.erase(it);
            if (sun) sun->removeFromParent();
        }
        else
        {
            ++it;
        }
    }
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
        this->addChild(sun, SUNLAYER);
        _suns.push_back(sun);
        CCLOG("Sun spawned from sky, target column: %d", targetCol);
    }
}

void GameWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
