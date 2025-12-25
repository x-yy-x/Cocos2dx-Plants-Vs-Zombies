
#include "Zombie.h"
#include "Plant.h"
#include "SpikeWeed.h"
#include "SpikeRock.h"
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

    _currentHealth -= damage;
    log("Zombie took %f damage, remaining health: %d", damage, _currentHealth); // Reduced logging

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
            auto spikeweed = dynamic_cast<SpikeWeed*>(plant);
            if (spikeweed)
                continue;
            auto spikerock = dynamic_cast<SpikeRock*>(plant);
            if (spikerock)
                continue;

            Rect zombieRect = this->getBoundingBox();

            zombieRect.origin.x += X_CORRECTION;
            zombieRect.size.width -= SIZE_CORRECTION;

            if (zombieRect.intersectsRect(plant->getBoundingBox()))
            {
                log("start eating");
                startEating(plant);
                return;
            }
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

cocos2d::Animation* Zombie::initAnimate(const std::string &fileName,float frameWidth,float frameHeight,int row,int col,int frameCount,float delay)
{

    Vector<SpriteFrame*> frames;

    int currentFrameCount = 0;
    for (int currentRow = 0; currentRow < row; currentRow++)
    {
        for (int currentCol = 0; currentCol < col; currentCol++)
        {
            
            float x = currentCol * frameWidth;
            float y = currentRow * frameHeight;

            auto frame = SpriteFrame::create( fileName,Rect(x, y, frameWidth, frameHeight));

            frames.pushBack(frame);
            if (++currentFrameCount >= frameCount)
                break;
        }
    }

    return  Animation::createWithSpriteFrames(frames, delay);
}

void Zombie::update(float delta)
{
    if (_isDead || _isDying)
    {
        return;
    }

    // If zombie is not eating, continue walking left
    if (!_isEating)
    {
        float newX = this->getPositionX() - _currentSpeed * delta;
        this->setPositionX(newX);

        // Check if zombie reached the left edge (game over condition)
        if (newX < -100)
        {
            // Zombie reached the house - game over
            CCLOG("Zombie reached the house!");
        }
    }
    else
    {
        if (!_targetPlant || _targetPlant->isDead()) {
            onPlantDied();
            return;
        }
        _accumulatedTime += delta;
        // If eating, deal damage periodically
        if (_accumulatedTime >= ATTACK_INTERVAL)
        {
            if (_targetPlant && !_targetPlant->isDead())
            {
                _targetPlant->takeDamage(ATTACK_DAMAGE);
                cocos2d::AudioEngine::play2d("zombie_eating.mp3");
                CCLOG("Zombie deals %f damage to plant", ATTACK_DAMAGE);
                _accumulatedTime = 0.0f;

                // Check if plant died
                if (_targetPlant->isDead())
                {
                    cocos2d::AudioEngine::play2d("zombie_gulp.mp3");
                    onPlantDied();
                }
            }
        }
    }
}