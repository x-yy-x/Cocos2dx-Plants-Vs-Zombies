
#include "Gargantuar.h"
#include "GameWorld.h"
#include "Imp.h"
#include "Plant.h"

USING_NS_CC;

Sprite* Gargantuar::createShowcaseSprite(const Vec2& pos)
{
    auto animation = initAnimate("gargantuar_idle_spritesheet.png", 750.0f, 750.0f, 9, 5, 45, 0.05f);
    auto animate = Animate::create(animation);
    auto _idleAction = RepeatForever::create(animate);

    auto sp = Sprite::create();
    if (sp) {
        sp->setPosition(pos);
        sp->setScale(0.4f);
        sp->runAction(_idleAction);
    }
    return sp;
}


// Protected constructor
Gargantuar::Gargantuar()
    : _walkAction(nullptr)
    , _smashAction(nullptr)
    , _prethrowAction(nullptr)
    , _postthrowAction(nullptr)
    , _isThrowing(false)
    , _hasthrown(false)
{
    CCLOG("Zombie created.");
}

// Destructor
Gargantuar::~Gargantuar()
{
    CC_SAFE_RELEASE(_walkAction);
    CC_SAFE_RELEASE(_smashAction);
    CCLOG("Zombie destroyed.");
}

// Initialization function
bool Gargantuar::init()
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
Gargantuar* Gargantuar::createZombie()
{
    Gargantuar* z = new Gargantuar();
    if (z && z->init())
    {
        z->autorelease();
        z->initWalkAnimation();
        z->initSmashAnimation();
        z->initThrowAnimation();
        z->_currentHealth = static_cast<int>(3000.0f);
        z->ATTACK_DAMAGE = 1000.0f;
        z->ATTACK_INTERVAL = 2.64f;
        z->runAction(z->_walkAction);
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void Gargantuar::initWalkAnimation()
{
    auto animation = initAnimate("gargantuar_walk_spritesheet.png", 280.0f, 292.0f, 7, 6, 40, 0.08f);
    auto animate = Animate::create(animation);
    this->_walkAction = RepeatForever::create(animate);
    _walkAction->retain();
}

// Initialize eating animation
void Gargantuar::initSmashAnimation()
{
    auto animation = initAnimate("gargantuar_smash_spritesheet.png", 395.0f, 365.0f, 7, 5, 33, 0.08f);
    auto animate = Animate::create(animation);
    this->_smashAction = Animate::create(animation);
    _smashAction->retain();
}

void Gargantuar::initThrowAnimation()
{
    auto preThrowAnimation = initAnimate("gargantuar_throw_spritesheet.png", 469.0f, 400.0f, 4, 10, 0,30, 0.08f);
    this->_prethrowAction= Animate::create(preThrowAnimation);
    _prethrowAction->retain();

    auto postThrowAnimation= initAnimate("gargantuar_throw_spritesheet.png", 469.0f, 400.0f, 4, 10, 30, 37, 0.08f);
    this->_postthrowAction = Animate::create(postThrowAnimation);
    _postthrowAction->retain();
}


void Gargantuar::updateMoving(float delta)
{
    if (_isThrowing)
        return;
    if (_currentHealth <= 1500 && _currentHealth > 0 && _hasthrown == false && this->getPositionX() >= 500) {
        this->_currentSpeed = 0;
        this->_isThrowing = true;
        setState(static_cast<int>(ZombieState::THROWING));
        return;
    }
    float newX = this->getPositionX() - _currentSpeed * delta;
    this->setPositionX(newX);
}

void Gargantuar::updateEating(float delta)
{
    _accumulatedTime += delta;
    // If eating, deal damage periodically
    if (_accumulatedTime >= ATTACK_INTERVAL)
    {
        _targetPlant->takeDamage(ATTACK_DAMAGE);
        cocos2d::AudioEngine::play2d("gargantuar-thump.mp3");
        _accumulatedTime = 0.0f;
    }
}

// Set animation corresponding to state
void Gargantuar::setAnimationForState()
{
    switch (static_cast<ZombieState>(_currentState))
    {
        case ZombieState::WALKING:
            CCLOG("Setting WALKING animation.");
            this->stopAction(_smashAction);
            this->runAction(_walkAction);
            break;
        case ZombieState::SMASHING:
            CCLOG("Setting EATING animation.");
            this->stopAction(_walkAction);
            this->runAction(Sequence::create(
                MoveBy::create(0, Vec2(-20, 55)),
                _smashAction,
                MoveBy::create(0, Vec2(20, -55)),
                CallFunc::create([this]() {
                    this->_isEating = false;
                    this->_currentSpeed = MOVE_SPEED;
                    setState(static_cast<int>(ZombieState::WALKING));
                    }),
                nullptr
            ));
            break;
        case ZombieState::THROWING:
            CCLOG("setting throwing animation");           
            this->stopAllActions();
            this->runAction(
                Sequence::create(
                    MoveBy::create(0, Vec2(-46, 35)),
                    _prethrowAction,
                    CallFunc::create([this]() {
                        CCLOG("throw");
                        this->throwImp();
                        }),
                    _postthrowAction,
                    MoveBy::create(0, Vec2(46, -35)),
                    CallFunc::create([this]() {
                        this->_hasthrown = true;
                        this->_isThrowing = false;
                        this->_currentSpeed = MOVE_SPEED;
                        this->setState(static_cast<int>(ZombieState::WALKING));
                        }),
                    nullptr
                )
            );
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
        default:
            break;
    }
}

// Check and handle plant encounters
void Gargantuar::encounterPlant(const std::vector<Plant*>& plants)
{
    if (_isEating || _isThrowing)
        return;

    for (auto plant : plants)
    {
        if (plant && !plant->isDead())
        {
            float COLLISION_OFFSET_X = 60.0f;
            float SIZE_CORRECTION = 180.0f;
            Rect zombieRect = this->getBoundingBox();

            zombieRect.origin.x += COLLISION_OFFSET_X;
            zombieRect.size.width -= SIZE_CORRECTION;

            if (zombieRect.intersectsRect(plant->getBoundingBox()))
            {
                startEating(plant);
                return;
            }
        }
    }
}


void Gargantuar::throwImp()
{
    CCLOG("imp created");
    auto imp = Imp::createZombie();
    auto pos = this->getPosition();
    imp->setPosition(pos + Vec2(-250, 35));
    auto gameWorld = static_cast<GameWorld*>(Director::getInstance()->getRunningScene());
    gameWorld->addChild(imp, ENEMY_LAYER);
    gameWorld->addZombie(imp);
    cocos2d::AudioEngine::play2d("imp-pvz.mp3", false, 1.0f);
}
