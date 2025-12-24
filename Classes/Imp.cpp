
#include "Imp.h"
#include "Plant.h"

USING_NS_CC;

const float Imp::MOVE_SPEED = 40.0f;

// Protected constructor
Imp::Imp()
    : _currentState(ZombieState::FLYING)
    , _walkAction(nullptr)
    , _eatAction(nullptr)
    , _flyAnimate(nullptr)
    , _hasBeenThrown(false)
    , _isFlying(false)
{
    CCLOG("Zombie created.");
}

// Destructor
Imp::~Imp()
{
    CC_SAFE_RELEASE(_walkAction);
    CC_SAFE_RELEASE(_eatAction);
    CC_SAFE_RELEASE(_flyAnimate);
    CCLOG("Zombie destroyed.");
}

// Initialization function
bool Imp::init()
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

// Static factory method to create zombie with animations
Imp* Imp::createZombie()
{
    Imp* z = new Imp();
    if (z && z->init())
    {
        z->autorelease();
        z->initWalkAnimation();
        z->initEatAnimation();
        z->initFlyAnimation();
        z->runAction(Sequence::create(z->_flyAnimate, nullptr));
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void Imp::initWalkAnimation()
{
    auto animation = initAnimate("imp_walk_spritesheet.png", 100.0f, 138.0f, 3, 5, 12, 0.08f);
    auto animate = Animate::create(animation);
    this->_walkAction = RepeatForever::create(animate);
    _walkAction->retain();
}

// Initialize eating animation
void Imp::initEatAnimation()
{
    auto animation = initAnimate("imp_eat_spritesheet.png", 100.0f, 128.0f, 2, 5, 7, 0.15f);
    auto animate = Animate::create(animation);
    this->_eatAction = RepeatForever::create(animate);
    _eatAction->retain();
}

void Imp::initFlyAnimation()
{
    auto animation = initAnimate("imp_fly_spritesheet.png", 145.0f, 200.0f, 4, 5, 23, 0.06f);
    this->_flyAnimate = Animate::create(animation);
    _flyAnimate->retain();
}

// Set zombie state
void Imp::setState(ZombieState newState)
{
    if (_currentState != newState)
    {
        _currentState = newState;
        CCLOG("Zombie state changed.");
        setAnimationForState(newState);
    }
}



// Set animation corresponding to state
void Imp::setAnimationForState(ZombieState state)
{
    switch (state)
    {
    case ZombieState::WALKING:
        CCLOG("Setting imp WALKING animation.");
        this->stopAllActions();
        this->runAction(_walkAction);
        break;
    case ZombieState::EATING:
        CCLOG("Setting EATING animation.");
        this->stopAllActions();
        this->runAction(_eatAction);
        break;
    case ZombieState::FLYING:
    {
        CCLOG("set state flying");
        this->_isFlying = true;
        this->stopAction(_walkAction);
        this->runAction(Sequence::create(
            MoveBy::create(1.2f, Vec2(0, -45.0f)),
            CallFunc::create([this]() {
                this->_currentSpeed = 120.0f;
                }), 
            _flyAnimate,          
            MoveBy::create(0.0001f, Vec2(-20, -30)),
            CallFunc::create([this]() {
                this->_isFlying = false;
                this->_currentSpeed = MOVE_SPEED;
                this->setState(ZombieState::WALKING);
                }),
            nullptr));
        break;
    }
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
    default:
        break;
    }
}

