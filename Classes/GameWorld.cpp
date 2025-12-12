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

    // Create seed packets using template method (no need for separate subclasses!)
    auto sunflowerPacket = SeedPacket::create<Sunflower>("seedpacket_sunflower.png", 3.0f, 50);
    auto peashooterPacket = SeedPacket::create<PeaShooter>("seedpacket_peashooter.png", 7.5f, 100);
    auto repeaterPacket = SeedPacket::create<Repeater>("seedpacket_repeater.png", 3.0f, 200);
    auto threepeaterPacket = SeedPacket::create<ThreePeater>("seedpacket_threepeater(1).png", 3.0f, 325);
    auto wallnutPacket = SeedPacket::create<Wallnut>("seedpacket_wallnut.png", 30.0f, 50);
    auto cherryBombPacket = SeedPacket::create<CherryBomb>("seedpacket_cherry_bomb.png", 1.0f, 150);

    if (sunflowerPacket && peashooterPacket && repeaterPacket && threepeaterPacket && wallnutPacket && cherryBombPacket)
    {
        _seedPackets.push_back(sunflowerPacket);
        _seedPackets.push_back(peashooterPacket);
        _seedPackets.push_back(repeaterPacket);
        _seedPackets.push_back(threepeaterPacket);
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
        _shovelBack->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 80));
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
        Vec2 shovelPos = _shovelBack ? _shovelBack->getPosition() : Vec2(visibleSize.width - 100, visibleSize.height - 80);
        _shovel->setOriginalPosition(shovelPos);
        this->addChild(_shovel, SEEDPACKET_LAYER + 1);
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
                        this->addChild(_previewPlant, UI_LAYER);
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

void GameWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}

void GameWorld::addZombie(Zombie* z)
{
    //float y = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * ZOMBIE_Y_OFFSET
   float y = z->getPositionY();
   int row = (y - CELLSIZE.height * 0.7f - GRID_ORIGIN.y) / CELLSIZE.height;
   _zombiesInRow[row].push_back(z);
   this->addChild(z, ENEMY_LAYER);
}
