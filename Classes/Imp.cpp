
#include "Imp.h"
#include "Plant.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const cocos2d::Rect Imp::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Imp::OBJECT_SIZE = Size(125.0f, 173.8f);
const float Imp::MOVE_SPEED = 40.0f;       // Move 20 pixels per second
const float Imp::ATTACK_DAMAGE = 10.0f;    // Damage per attack
const float Imp::ATTACK_RANGE = 50.0f;     // Attack range

// Protected constructor
Imp::Imp()
    : _currentState(ZombieState::WALKING)
    , _isDead(false)
    , _maxHealth(200)
    , _currentHealth(200)
    , _attackInterval(0.5f)
    , _accumulatedTime(0.0f)
    , _zombiePos(Vec2::ZERO)
    , _walkAction(nullptr)
    , _eatAction(nullptr)
    , _flyAnimate(nullptr)
    , _isEating(false)
    , _targetPlant(nullptr)
    , _speed(Vec2(MOVE_SPEED, 0))
    , _normalSpeed(MOVE_SPEED)
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
        z->setAnimation();
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void Imp::initWalkAnimation()
{
    const float frameWidth = 100.0f;
    const float frameHeight = 138.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (row == 2 && col == 2)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "imp_walk_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );
            frames.pushBack(frame);

        }
    }
    
    auto animation = Animation::createWithSpriteFrames(frames, 0.08f);
    auto animate = Animate::create(animation);

    this->_walkAction = RepeatForever::create(animate);
    _walkAction->retain();
}

// Initialize eating animation
void Imp::initEatAnimation()
{
    const float frameWidth = 100.0f;
    const float frameHeight = 138.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (row == 1 && col == 2)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "imp_eat_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.15f);
    auto animate = Animate::create(animation);

    this->_eatAction = RepeatForever::create(animate);
    _eatAction->retain();
}

void Imp::initFlyAnimation()
{
    const float frameWidth = 145;
    const float frameHeight = 200;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (row == 4 && col == 3)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "imp_fly_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );
            frames.pushBack(frame);
        }
    }

    auto animate = Animation::createWithSpriteFrames(frames, 0.06f);
    _flyAnimate = Animate::create(animate);
    _flyAnimate->retain();
}

// Update every frame
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
        Vec2 newPos = this->getPosition() - _speed * delta;
        this->setPosition(newPos);

        // Check if zombie reached the left edge (game over condition)
        if (newPos.x < -100)
        {
            // Zombie reached the house - game over
            CCLOG("Zombie reached the house!");
        }
    }
    else if(_isEating)
    {
        // If eating, deal damage periodically
        if (_accumulatedTime >= _attackInterval)
        {
            if (_targetPlant && !_targetPlant->isDead())
            {
                _targetPlant->takeDamage((int)ATTACK_DAMAGE);
                cocos2d::AudioEngine::play2d("zombie_eating.mp3", false);
                CCLOG("Zombie deals %f damage to plant", ATTACK_DAMAGE);
                _accumulatedTime = 0.0f;

                // Check if plant died
                if (_targetPlant->isDead())
                {
                    cocos2d::AudioEngine::play2d("zombie_gulp.mp3", false);
                    onPlantDied();
                }
            }
            else
            {
                // Target plant is null or already dead
                onPlantDied();
            }
        }
    }
}

// Get zombie state
Imp::ZombieState Imp::getState() const
{
    CCLOG("getstate����");
    return _currentState;
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

// Check if dead
bool Imp::isDead() const
{
    return _isDead;
}

// Take damage
void Imp::takeDamage(int damage)
{
    if (_isDead || _isDying)
    {
        return;
    }

    _currentHealth -= damage;
    // CCLOG("Zombie took %d damage, remaining health: %d", damage, _currentHealth); // Reduced logging

    if (_currentHealth <= 0)
    {
        _currentHealth = 0;
        
        // Mark as dying (playing death animation)
        _isDying = true;
        
        // CRITICAL: Clear target plant pointer to prevent dangling pointer access
        _targetPlant = nullptr;
        _isEating = false;
        
        setState(ZombieState::DYING);
        CCLOG("Zombie is dying.");
        
        // Stop all actions
        this->stopAllActions();
        
        // Play fade out animation, then mark as dead
        auto fadeOut = FadeOut::create(0.5f);
        auto markDead = CallFunc::create([this]() {
            _isDead = true;
            _isDying = false;
            CCLOG("Zombie death animation finished, marked as dead.");
        });
        auto sequence = Sequence::create(fadeOut, markDead, nullptr);
        this->runAction(sequence);
    }
}

// Set animation default implementation
void Imp::setAnimation()
{
    CCLOG("Zombie::setAnimation() called.");
    setAnimationForState(_currentState);
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
            CallFunc::create([this]() {
                this->_speed = Vec2(120.0f, 45.0f);
                }), 
            _flyAnimate,          
            MoveBy::create(0.0001f, Vec2(-20, -30)),
            CallFunc::create([this]() {
                this->_isFlying = false;
                this->_speed = Vec2(_normalSpeed, 0);
                this->setState(ZombieState::WALKING);
                }),
            nullptr));
        break;
    }
    case ZombieState::DYING:
        CCLOG("Setting imp DYING animation.");
        break;
    default:
        break;
    }
}

// Check and handle plant encounters
void Imp::encounterPlant(const std::vector<Plant*>& plants)
{
    checkCollision(plants);
}

// Check collision with plants
void Imp::checkCollision(const std::vector<Plant*>& plants)
{
    if (_isEating||_isFlying) return;

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
            Rect zombieRect = this->getBoundingBox();
            // Move the rect slightly to the left/right or adjust width
            // Here we check if the zombie's "eating mouth" (left side) overlaps with plant
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

// Start eating a plant
void Imp::startEating(Plant* plant)
{
    _isEating = true;
    _targetPlant = plant;
    _speed = Vec2(0,0);  // Stop moving
    setState(ZombieState::EATING);
    CCLOG("Zombie start eating plant!");
}

// Called when plant dies
void Imp::onPlantDied()
{
    _isEating = false;
    _speed = Vec2(_normalSpeed,0);
    _targetPlant = nullptr;
    setState(ZombieState::WALKING);
    CCLOG("Zombie resume walking");
}
