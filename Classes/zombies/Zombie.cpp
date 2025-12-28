
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
    if (current_state != newState)
    {
        current_state = newState;
        setAnimationForState();
    }
}



// Take damage
void Zombie::takeDamage(float damage)
{
    if (is_dead || is_dying)
    {
        return;
    }

    current_health -= static_cast<int>(damage);
    CCLOG("Zombie took %f damage, remaining health: %d", damage, current_health); // Reduced logging

    if (current_health <= 0)
    {
        current_health = 0;
        
        // Mark as dying (playing death animation)
        is_dying = true;
        
        // CRITICAL: Clear target plant pointer to prevent dangling pointer access
        target_plant = nullptr;
        is_eating = false;
        
        setState(0);
    }
}


// Check and handle plant encounters
void Zombie::encounterPlant(const std::vector<Plant*>& plants)
{
    if (is_eating || is_dead || is_dying) return;

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
    is_eating = true;
    target_plant = plant;
    current_speed = 0;
    setState(2);
    CCLOG("Zombie start eating plant!");

}

// Called when plant dies
void Zombie::onPlantDied()
{
    is_eating = false;
    current_speed = MOVE_SPEED;
    target_plant = nullptr;
    setState(1);
    CCLOG("Zombie resume walking");
}

// update
void Zombie::update(float delta)
{
    if (is_dead || is_dying)
        return;   
    if (is_eating)
        updateEating(delta);
    else 
        updateMoving(delta);      
}

// update for moving state
void Zombie::updateMoving(float delta)
{
    float newX = this->getPositionX() - current_speed * delta;
    this->setPositionX(newX);
}

// update for eating state
void Zombie::updateEating(float delta)
{
    if (!target_plant || target_plant->isDead()) {
        onPlantDied();
        return;
    }
    accumulated_time += delta;
    // If eating, deal damage periodically
    if (accumulated_time >= ATTACK_INTERVAL)
    {
        target_plant->takeDamage(ATTACK_DAMAGE);
        cocos2d::AudioEngine::play2d("zombie_eating.mp3");
        accumulated_time = 0.0f;

        // Check if plant died
        if (target_plant->isDead())
        {
            cocos2d::AudioEngine::play2d("zombie_gulp.mp3");
            onPlantDied();
        }
    }
}