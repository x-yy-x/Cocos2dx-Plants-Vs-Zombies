
#include "Zomboni.h"
#include "Plant.h"
#include "GameWorld.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

Sprite* Zomboni::createShowcaseSprite(const Vec2& pos)
{
    auto sp = Sprite::create();
    if (sp) {
        sp->setScale(0.45f);
        sp->setPosition(pos);
        sp->runAction(_driveAction);
    }
    return sp;
}


const float Zomboni::MOVE_SPEED = 20.0f;
const float Zomboni::ICE_STEP = 10.0f;
const int Zomboni::ICE_COUNT = 15;
const float Zomboni::ICE_LENGTH = 150.0f;
const int Zomboni::MAX_HEALTH = 1300;

// Protected constructor
Zomboni::Zomboni()
    : _driveAction(nullptr)
    , _specialDieAction(nullptr)
    , _iceAccumulate(0.0f)
    , _iceIndex(0)
    , _hasBeenAttackedBySpike(false)
{
    CCLOG("Zombie created.");
}

// Destructor
Zomboni::~Zomboni()
{
    CC_SAFE_RELEASE(_driveAction);
    CCLOG("Zombie destroyed.");
}

// Initialization function
bool Zomboni::init()
{
    // Call parent class initialization
    if (!Sprite::init())
    {
        return false;
    }

    // Enable per-frame update
    cocos2d::AudioEngine::play2d("zomboni.mp3");
    this->_currentHealth = MAX_HEALTH;
    this->_hasBeenAttackedBySpike = false; // Reset spike attack flag on initialization
    this->setScale(0.45f);
    this->scheduleUpdate();

    return true;
}

// Static factory method to create zombie with animations
Zomboni* Zomboni::createZombie()
{
    Zomboni* z = new Zomboni();
    if (z && z->init())
    {
        z->autorelease();
        z->initDriveAnimation();
        z->initSpecialDieAnimation();
        z->runAction(z->_driveAction);
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void Zomboni::initDriveAnimation()
{
    auto animation = initAnimate("zomboni_drive_spritesheet.png", 470.0f, 450.0f, 3, 4, 12, 0.08f);
    auto animate = Animate::create(animation);
    this->_driveAction = RepeatForever::create(animate);
    _driveAction->retain();
}

// Initialize eating animation
void Zomboni::initSpecialDieAnimation()
{
    auto animation = initAnimate("zomboni_special_spritesheet.png", 600.0f, 525.0f, 2, 6, 12, 0.08f);
    this->_specialDieAction = Animate::create(animation);
    _specialDieAction->retain();
}

// Update every frame
void Zomboni::update(float delta)
{
    if (_isDead || _isDying)
        return;

    float dx = _currentSpeed * delta;
    float newX = this->getPositionX() - dx;
    this->setPositionX(newX);

    // Accumulate movement distance
    _iceAccumulate += dx;

    if (_iceAccumulate >= ICE_STEP)
    {
        _iceAccumulate -= ICE_STEP;
        spawnIce();   // Lay a piece of ice
    }

    if (newX < -100)
    {
        CCLOG("Zombie reached the house!");
    }

    if (_isEating)
    {
        if(_targetPlant && !_targetPlant->isDead())
        {
            _targetPlant->takeDamage(10000.0f);
            _isEating = false;
            _targetPlant = nullptr;
            this->_currentSpeed = MOVE_SPEED;
        }
    }
}




// Set animation corresponding to state
void Zomboni::setAnimationForState()
{
    switch (static_cast<ZombieState>(_currentState))
    {
    case ZombieState::DRIVING:
        CCLOG("Setting DRIVING animation.");
        this->stopAllActions();
        this->runAction(_driveAction);
        break;
    case ZombieState::DYING:
    {
        log("Setting dying animation.");
        this->stopAllActions();
        auto fadeOut = FadeOut::create(0.5f);
        auto markDead = CallFunc::create([this]() {
            _isDead = true;
            _isDying = false;
            CCLOG("Zombie death animation finished, marked as dead.");
            });
        auto sequence = Sequence::create(fadeOut, markDead, nullptr);
        this->runAction(sequence);
        break;
    }
    case ZombieState::SPECIAL:
        CCLOG("settting special animation");
        cocos2d::AudioEngine::play2d("Explosion.mp3");
        this->stopAllActions();
        this->_isDying = true;
        this->runAction(Sequence::create(_specialDieAction, CallFunc::create([this]() {
            _isDying = true;
            this->setState(static_cast<int>(ZombieState::DYING));
            }),nullptr));
        break;
    default:
        break;
    }
}



void Zomboni::spawnIce()
{    
    // Calculate clipping area (from right to left)
    float rectX = ICE_LENGTH - (_iceIndex + 1) * ICE_STEP;
    Rect iceRect(rectX, 0, ICE_STEP, 90.0f); // 90 = ice image height

    // Create ice sprite
    auto ice = IceTile::create(this->getPosition(), _iceIndex);
    if (!ice) return;


    // Hand over to GameWorld management
    auto gameWorld = static_cast<GameWorld*>(this->getParent());
    if (gameWorld)
    {
        gameWorld->addIceTile(ice);
    }

    // Index loop
    _iceIndex = (_iceIndex + 1) % ICE_COUNT;
}

void Zomboni::setSpecialDeath()
{
    // Mark that this zomboni has been attacked by spikeweed to prevent multiple damage
    _hasBeenAttackedBySpike = true;
    this->_currentHealth -= static_cast<int>(10000.0f);
    this->setState(static_cast<int>(ZombieState::SPECIAL));
}