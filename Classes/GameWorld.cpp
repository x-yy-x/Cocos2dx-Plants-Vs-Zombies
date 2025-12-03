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
    _shovelSelected = false;
    _shovel = nullptr;
    _shovelBack = nullptr;

    // Initialize wave spawning system
    _currentWave = 0;
    _tickCount = 0;
    _nextWaveTickCount = FIRST_WAVE_TICK;
    _gameStarted = true;

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

    // Create peashooter seed packet sprite
    auto sprite_seedpacket_peashooter = Sprite::create("seedpacket_peashooter.png");
    if (sprite_seedpacket_peashooter == nullptr)
    {
        problemLoading("'seedpacket_peashooter.png'");
    }
    else
    {
        sprite_seedpacket_peashooter->setPosition(Vec2(187, 667));
        this->addChild(sprite_seedpacket_peashooter, 0);
        sprite_seedpacket.push_back(sprite_seedpacket_peashooter);
    }

    auto sprite_seedpacket_sunflower = Sprite::create("seedpacket_sunflower.png");
    if (sprite_seedpacket_sunflower == nullptr)
    {
        problemLoading("'seedpacket_sunflower.png'");
    }
    else
    {
        sprite_seedpacket_sunflower->setPosition(Vec2(287, 667));
        this->addChild(sprite_seedpacket_sunflower, 0);
        sprite_seedpacket.push_back(sprite_seedpacket_sunflower);
    }

    auto sprite_seedpacket_wallnut = Sprite::create("seedpacket_wallnut.png");
    if (sprite_seedpacket_wallnut == nullptr)
    {
        problemLoading("'seedpacket_wallnut.png'");
    }
    else
    {
        sprite_seedpacket_wallnut->setPosition(Vec2(387, 667));
        this->addChild(sprite_seedpacket_wallnut, 0);
        sprite_seedpacket.push_back(sprite_seedpacket_wallnut);
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
        this->addChild(_shovelBack, 1);
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
        this->addChild(_shovel, 2);
    }

    // DEBUG: Spawn one zombie at start for testing
    {
        auto debugZombie = Zombie::createZombie();
        if (debugZombie)
        {
            int row = 2; // Middle row for testing
            const float ZOMBIE_Y_OFFSET = 0.7f;
            float y = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * ZOMBIE_Y_OFFSET;
            float x = visibleSize.width - 200; 
            debugZombie->setPosition(Vec2(x, y));
            this->addChild(debugZombie, ENEMY_LAYER);
            _zombiesInRow[row].push_back(debugZombie);
            CCLOG("DEBUG: Spawned test zombie at row %d", row);
        }
    }

    // Setup user interaction for planting and shoveling
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

        if (_shovel && _shovel->containsPoint(pos)) {
            _shovelSelected = true;
            _shovel->setDragging(true);
            return true;
        }

        if (sprite_seedpacket[0] && sprite_seedpacket[0]->getBoundingBox().containsPoint(pos)) {
            this->_plantSelected = true;
            plantType = 0;
            return true;
        }

        if (sprite_seedpacket[1] && sprite_seedpacket[1]->getBoundingBox().containsPoint(pos)) {
            this->_plantSelected = true;
            plantType = 1;
            return true;
        }

        if (sprite_seedpacket[2] && sprite_seedpacket[2]->getBoundingBox().containsPoint(pos)) {
            this->_plantSelected = true;
            plantType = 2;
            return true;
        }

        return true;
    };

    unifiedListener->onTouchMoved = [this](Touch* touch, Event* event) {
        if (_shovelSelected && _shovel)
        {
            Vec2 pos = touch->getLocation();
            _shovel->updatePosition(pos);
        }
    };

    unifiedListener->onTouchEnded = [this](Touch* touch, Event* event) {
        Vec2 pos = touch->getLocation();

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

        if (_plantSelected)
        {
            bool planted = this->tryPlantAtPosition(pos, plantType);
            
            if (planted) CCLOG("Plant placed!");
            else CCLOG("Cannot plant!");

            _plantSelected = false;
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(unifiedListener, this);
}

bool GameWorld::tryPlantAtPosition(const Vec2& globalPos,int plantType)
{
    int row, col;
    CCLOG("plantType=%d", plantType);
    if (!getGridCoordinates(globalPos, row, col)) return false;

    if (_plantGrid[row][col] != nullptr) return false;

    if (plantType == 0) {
        plantType = -1;
        auto peaShooter = PeaShooter::plantAtPosition(globalPos);
        if (peaShooter)
        {
            this->addChild(peaShooter, PLANT_LAYER);
            _plantGrid[row][col] = peaShooter;
            _plants.push_back(peaShooter); // Add to vector for easier iteration if needed
            return true;
        }
    }
    else if (plantType == 1) {
        plantType = -1;
        auto sunflower = Sunflower::plantAtPosition(globalPos);
        if (sunflower)
        {
            this->addChild(sunflower, PLANT_LAYER);
            _plantGrid[row][col] = sunflower;
            _plants.push_back(sunflower); // Add to vector for easier iteration if needed
            return true;
        }
    }
    else if (plantType == 2) {
        plantType = -1;
        auto wallnut = Wallnut::plantAtPosition(globalPos);
        if (wallnut)
        {
            this->addChild(wallnut, PLANT_LAYER);
            _plantGrid[row][col] = wallnut;
            _plants.push_back(wallnut); // Add to vector for easier iteration if needed
            return true;
        }
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
    
    // Also remove from _plants vector
    auto it = std::find(_plants.begin(), _plants.end(), plantToRemove);
    if (it != _plants.end()) _plants.erase(it);
    
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

    // Update Plants (Firing logic)
    updatePlants(delta);

    // Update Bullets (Movement and Collision)
    updateBullets(delta);

    // Update Zombies (Movement and Eating)
    updateZombies(delta);

    // Cleanup
    removeDeadPlants();
    removeDeadZombies();
    removeInactiveBullets();
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
                // Check if any zombie exists to the right of this plant in the same row
                // Optimization: Only check if plant can attack? 
                // For now, we check zombies first, then try to attack.
                // Alternatively, we can let plant->attack() decide, but passing zombies to it might be complex.
                // Current logic: Check zombie presence -> Call attack() -> If bullet returned, add it.
                
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
                    // Try to attack
                    // This is generic for all plants. If a plant doesn't attack, it returns nullptr.
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
                
                auto it = std::find(_plants.begin(), _plants.end(), plant);
                if (it != _plants.end()) _plants.erase(it);
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

void GameWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
