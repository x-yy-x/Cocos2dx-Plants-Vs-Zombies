
#include "Zombie.h"
#include "Plant.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// Initialization function
bool Zombie::init()
{
    // Call parent class initialization
    if (!Sprite::init())
    {
        return false;
    }

    // Enable per-frame update
    this->scheduleUpdate();

    return true;
}


// Set zombie state
void Zombie::setState(int newState)
{
    if (_currentState != newState)
    {
        _currentState = newState;
        setAnimationForState();
    }
}



// Take damage
void Zombie::takeDamage(float damage)
{
    if (_isDead || _isDying)
    {
        return;
    }

    _currentHealth -= static_cast<int>(damage);
    CCLOG("Zombie took %f damage, remaining health: %d", damage, _currentHealth); // Reduced logging

    if (_currentHealth <= 0)
    {
        _currentHealth = 0;
        
        // Mark as dying (playing death animation)
        _isDying = true;
        
        // CRITICAL: Clear target plant pointer to prevent dangling pointer access
        _targetPlant = nullptr;
        _isEating = false;
        
        setState(0);
    }
}


// Check and handle plant encounters
void Zombie::encounterPlant(const std::vector<Plant*>& plants)
{
    if (_isEating || _isDead || _isDying) return;

    for (auto plant : plants)
    {
        if (plant && !plant->isDead())
        {
            // Use virtual function instead of dynamic_cast to check if it's a spike plant
            if (plant->isSpike())
                continue;

            Rect zombieRect = this->getBoundingBox();

            zombieRect.origin.x += X_CORRECTION;
            zombieRect.size.width -= SIZE_CORRECTION;

            if (zombieRect.intersectsRect(plant->getBoundingBox()))
                startEating(plant);            
        }
    }
}

// Start eating a plant
void Zombie::startEating(Plant* plant)
{
    _isEating = true;
    _targetPlant = plant;
    _currentSpeed = 0;
    setState(2);
    CCLOG("Zombie start eating plant!");

}

// Called when plant dies
void Zombie::onPlantDied()
{
    _isEating = false;
    _currentSpeed = MOVE_SPEED;
    _targetPlant = nullptr;
    setState(1);
    CCLOG("Zombie resume walking");
}

void Zombie::update(float delta)
{
    if (_isDead || _isDying)
        return;   
    if (_isEating)
        updateEating(delta);
    else 
        updateMoving(delta);      
}

void Zombie::updateMoving(float delta)
{
    float newX = this->getPositionX() - _currentSpeed * delta;
    this->setPositionX(newX);
}

void Zombie::updateEating(float delta)
{
    if (!_targetPlant || _targetPlant->isDead()) {
        onPlantDied();
        return;
    }
    _accumulatedTime += delta;
    // If eating, deal damage periodically
    if (_accumulatedTime >= ATTACK_INTERVAL)
    {
        _targetPlant->takeDamage(ATTACK_DAMAGE);
        cocos2d::AudioEngine::play2d("zombie_eating.mp3");
        _accumulatedTime = 0.0f;

        // Check if plant died
        if (_targetPlant->isDead())
        {
            cocos2d::AudioEngine::play2d("zombie_gulp.mp3");
            onPlantDied();
        }
    }
}