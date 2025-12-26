
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
        // --- 新增代码 ---
        this->_isFlying = false;          // 停止飞行逻辑
        this->_currentSpeed = MOVE_SPEED; // 恢复正常行走速度
        // ----------------
        this->runAction(_eatAction);
        break;
    case ZombieState::FLYING:
    {
        this->_isFlying = true;
        this->stopAllActions();
        this->runAction(Sequence::create(
            CallFunc::create([this]() {
                this->_currentSpeed = 120.0f; // 飞行速度
                }),
            _flyAnimate,
            CallFunc::create([this]() {
                this->_isFlying = false;     // 落地
                this->_currentSpeed = MOVE_SPEED;
                this->setState(static_cast<int>(ZombieState::WALKING));

                // 确保回到行走的基准线上（根据你的网格调整）
                // 如果之前 update 里减了 Y，这里可以设回固定行高度
                // 或者：this->setPositionY(original_row_y); 
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
        // 只有不在吃东西时才移动
        float newX = this->getPositionX() - _currentSpeed * delta;
        this->setPositionX(newX);

        // 只有明确在飞行状态时才计算纵向位移
        if (_isFlying) {
            float newY = this->getPositionY() - 50.0f * delta; // 这里的50可以根据需要调整
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

void Imp::encounterPlant(const std::vector<Plant*>& plants)
{
    // 如果正在飞行，或者已经死亡，或者正在吃，直接返回
    if (_isFlying || _isEating || _isDead || _isDying)
    {
        return;
    }

    // 以下是正常的碰撞逻辑
    for (auto plant : plants)
    {
        if (plant && !plant->isDead())
        {
            // 这里的判定范围可以根据 Imp 的体型微调
            Rect zombieRect = this->getBoundingBox();
            if (zombieRect.intersectsRect(plant->getBoundingBox()))
            {
                startEating(plant);
                return;
            }
        }
    }
}
