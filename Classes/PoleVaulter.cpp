
#include "PoleVaulter.h"
#include "Plant.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

Sprite* PoleVaulter::createShowcaseSprite(const Vec2& pos)
{
    auto animation = initAnimate("pole_vaulter_idle_spritesheet.png", 1250.0f, 785.0f, 3, 5, 12, 0.06f);
    auto animate = Animate::create(animation);
    auto _idleAction = RepeatForever::create(animate);    

    auto sp = Sprite::create();
    if (sp) {
        sp->setPosition(pos);
        sp->setScale(0.35f);
        sp->runAction(_idleAction);
    }
    return sp;
}


const float PoleVaulter::RUNNING_SPEED = 40.0f;
// Protected constructor
PoleVaulter::PoleVaulter()
    : _currentState(ZombieState::RUNNING)
    , _walkAction(nullptr)
    , _eatAction(nullptr)
    , _jumpAction(nullptr)
    , _runAction(nullptr)
    , _isJumping(false)
    , _hasJumped(false)
{
    CCLOG("Zombie created.");
}

// Destructor
PoleVaulter::~PoleVaulter()
{
    CC_SAFE_RELEASE(_walkAction);
    CC_SAFE_RELEASE(_eatAction);
    CC_SAFE_RELEASE(_runAction);
    CC_SAFE_RELEASE(_jumpAction);
    CCLOG("Zombie destroyed.");
}

// Initialization function
bool PoleVaulter::init()
{

    // Call parent class initialization
    if (!Sprite::init())
    {
        return false;
    }

    this->setScale(0.9f);

    // Enable per-frame update
    this->scheduleUpdate();
    return true;
}

// Static factory method to create zombie with animations
PoleVaulter* PoleVaulter::createZombie()
{
    PoleVaulter* z = new PoleVaulter();
    if (z && z->init())
    {
        z->autorelease();
        z->initWalkAnimation();
        z->initEatAnimation();
        z->initRunningAnimation();
        z->initJumpingAnimation();
        z->_currentSpeed = RUNNING_SPEED;
        z->runAction(z->_runAction);
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void PoleVaulter::initWalkAnimation()
{
    auto animation = initAnimate("pole_vaulter_walk_spritesheet.png", 125.0f, 225.0f, 5, 10, 44, 0.05f);
    auto animate = Animate::create(animation);
    this->_walkAction = RepeatForever::create(animate);
    _walkAction->retain(); 
}

// Initialize eating animation
void PoleVaulter::initEatAnimation()
{
    auto animation = initAnimate("pole_vaulter_eat_spritesheet.png", 125.0f, 225.0f, 3, 10, 37, 0.03f);
    auto animate = Animate::create(animation);
    this->_eatAction = RepeatForever::create(animate);
    _eatAction->retain();
}

void PoleVaulter::initRunningAnimation()
{
    auto animation = initAnimate("pole_vaulter_run_spritesheet.png", 375.0f, 225.0f, 6, 6, 36, 0.03f);
    auto animate = Animate::create(animation);
    this->_runAction = RepeatForever::create(animate);
    _runAction->retain();
}

void PoleVaulter::initJumpingAnimation()
{
    auto animation = initAnimate("pole_vaulter_jump_spritesheet.png", 625.0f, 225.0f, 4, 11, 42, 0.03f);
    this->_jumpAction = Animate::create(animation);
    _jumpAction->retain();
}


// Update every frame
void PoleVaulter::update(float delta)
{
    if (_isDead || _isDying)
    {
        return;
    }

    _accumulatedTime += delta;

    // If zombie is not eating, continue walking left
    if (!_isEating&&!_isJumping)
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
    else if(_isEating)
    {
        // If eating, deal damage periodically
        if (_accumulatedTime >= ATTACK_INTERVAL)
        {
            if (_targetPlant && !_targetPlant->isDead())
            {
                _targetPlant->takeDamage((int)ATTACK_DAMAGE);
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


// Set zombie state
void PoleVaulter::setState(ZombieState newState)
{
    if (_currentState != newState)
    {
        _currentState = newState;
        CCLOG("Zombie state changed.");
        setAnimationForState(newState);
    }
}




// Set animation corresponding to state
void PoleVaulter::setAnimationForState(ZombieState state)
{
    switch (state)
    {
        case ZombieState::WALKING:
            CCLOG("Setting WALKING animation.");
            this->stopAllActions();
            this->runAction(_walkAction);
            break;
        case ZombieState::EATING:
            CCLOG("Setting EATING animation.");
            this->stopAllActions();
            this->runAction(_eatAction);
            break;
        case ZombieState::DYING:
        {
            CCLOG("Setting DYING animation.");
            this->stopAllActions();
            auto fadeOut = FadeOut::create(0.5f);
            auto markDead = CallFunc::create([this]() {
                _isDead = true;
                _isDying = false;
                });
            auto sequence = Sequence::create(fadeOut, markDead, nullptr);
            this->runAction(sequence);
            break;
        }
        case ZombieState::JUMPING:
            CCLOG("Setting JUMPING animation.");
            this->stopAllActions();           
            this->runAction(
                Sequence::create(
                    _jumpAction,
                    CallFunc::create([this]() {
                        this->_isJumping = false;
                        this->_currentSpeed = MOVE_SPEED;
                        this->setState(ZombieState::WALKING);
                        }),
                    MoveBy::create(0.0001f, Vec2(-170, 0)),
                    nullptr
                )
            );
            break;
        case ZombieState::RUNNING:
            CCLOG("Setting RUNNING animation.");
            this->stopAllActions();
            this->runAction(_runAction);
            break;
        default:
            break;
    }
}


// Check and handle plant encounters
void PoleVaulter::encounterPlant(const std::vector<Plant*>& plants)
{
    if (_isEating || _isJumping) return;

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
            // Create a slightly offset collision box for the zombie
            // This allows the zombie to eat the plant when it's slightly overlapping
            // Adjust COLLISION_OFFSET_X to control how close the zombie needs to be
            float COLLISION_OFFSET_X = 40.0f;
            float SIZE_CORRECTION = 100.0f;
            if (!_hasJumped) {
                COLLISION_OFFSET_X = 110.0f;
                SIZE_CORRECTION = 160.0f;
            }
            Rect zombieRect = this->getBoundingBox();
            // Move the rect slightly to the left/right or adjust width
            // Here we check if the zombie's "eating mouth" (left side) overlaps with plant
            zombieRect.origin.x += COLLISION_OFFSET_X;
            zombieRect.size.width -= SIZE_CORRECTION;

            if (zombieRect.intersectsRect(plant->getBoundingBox()))
            {
                CCLOG("!!!should jump!!!");
                if (_hasJumped)
                    startEating(plant);
                else
                    startJumping();
                return;
            }
        }
    }
}



void PoleVaulter::startJumping()
{
    _hasJumped = true;
    _isJumping = true;
    _currentSpeed = 0;
    setState(ZombieState::JUMPING);
    CCLOG("Zombie start jumping!");
    cocos2d::AudioEngine::play2d("polevault.mp3");
}
