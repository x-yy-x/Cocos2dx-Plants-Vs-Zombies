
#include "Imp.h"
#include "Plant.h"

USING_NS_CC;

// Protected constructor
Imp::Imp()
    : _walkAction(nullptr)
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
        z->MOVE_SPEED = 40.0f;
        z->_currentSpeed = z->MOVE_SPEED;
        z->_currentHealth = 100;
        z->_currentState = static_cast<int>(ZombieState::FLYING);
        //z->runAction(z->_flyAnimate);
        z->setAnimationForState();
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


// Set animation corresponding to state
void Imp::setAnimationForState()
{
    switch (static_cast<ZombieState>(_currentState))
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
        this->stopAllActions();
        this->runAction(Sequence::create(
            CallFunc::create([this]() {
                this->_currentSpeed = 120.0f;
                }), 
            _flyAnimate,          
            CallFunc::create([this]() {
                this->_isFlying = false;
                this->_currentSpeed = MOVE_SPEED;
                this->setState(static_cast<int>(ZombieState::WALKING));
                this->setPosition(this->getPosition() + Vec2(0, -20));
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

void Imp::update(float delta)
{
    if (_isDead || _isDying)
    {
        return;
    }

    _accumulatedTime += delta;
    // If zombie is not eating, continue walking left
    if (!_isEating)
    {
        float newX = this->getPositionX() - _currentSpeed * delta;
        this->setPositionX(newX);
        if (_isFlying) {
            float newY = this->getPositionY() - 50 * delta;
            this->setPositionY(newY);
        }

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

