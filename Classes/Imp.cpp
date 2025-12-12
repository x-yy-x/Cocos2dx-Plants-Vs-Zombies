
#include "Imp.h"
#include "Plant.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Imp::IMAGE_FILENAME = "flag_zombie_walk_spritesheet.png";
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
    , _flyAction(nullptr)
    , _isEating(false)
    , _targetPlant(nullptr)
    , _speed(MOVE_SPEED)
    , _normalSpeed(MOVE_SPEED)
    ,_hasBeenThrown(false)
{
    CCLOG("Zombie created.");
}

// Destructor
Imp::~Imp()
{
    CC_SAFE_RELEASE(_walkAction);
    CC_SAFE_RELEASE(_eatAction);
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
                "lmp_walk_spritesheet.png",
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
                "lmp_eat_spritesheet.png",
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
    const float frameWidth = 100;
    const float frameHeight = 138;

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

    auto animation = Animation::createWithSpriteFrames(frames, 0.08f);
    this->_flyAction = Animate::create(animation);
    _flyAction->retain();
}

// Update every frame
void Imp::update(float delta)
{
    if (_isDead)
    {
        return;
    }

    _accumulatedTime += delta;

    // If zombie is not eating, continue walking left
    if (!_isEating)
    {
        float newX = this->getPositionX() - _speed * delta;
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
        // If eating, deal damage periodically
        if (_accumulatedTime >= _attackInterval)
        {
            if (_targetPlant && !_targetPlant->isDead())
            {
                _targetPlant->takeDamage((int)ATTACK_DAMAGE);
                CCLOG("Zombie deals %f damage to plant", ATTACK_DAMAGE);
                _accumulatedTime = 0.0f;

                // Check if plant died
                if (_targetPlant->isDead())
                {
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
    if (_isDead)
    {
        return;
    }

    _currentHealth -= damage;
    // CCLOG("Zombie took %d damage, remaining health: %d", damage, _currentHealth); // Reduced logging

    if (_currentHealth <= 0)
    {
        _currentHealth = 0;
        _isDead = true;
        setState(ZombieState::DYING);
        CCLOG("Zombie is dead.");
        
        // Stop all actions and remove from scene after a delay
        this->stopAllActions();
        
        // IMPORTANT: Ensure we don't have lingering pointers or actions
        auto fadeOut = FadeOut::create(0.5f);
        auto remove = RemoveSelf::create();
        auto sequence = Sequence::create(fadeOut, remove, nullptr);
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
        if (_eatAction)
        {
            this->stopAction(_eatAction);
        }
        if (_walkAction)
        {
            this->runAction(_walkAction);
        }
        break;
    case ZombieState::EATING:
        CCLOG("Setting EATING animation.");
        if (_walkAction)
        {
            this->stopAction(_walkAction);
        }
        if (_eatAction)
        {
            this->runAction(_eatAction);
        }
        break;
    case ZombieState::FLYING:
        CCLOG("set state flying");
        this->stopAllActions();
        this->setScale(1.45f);
        this->_speed = 70.0f;
        this->runAction(Sequence::create( _flyAction, 
            CallFunc::create([this]() {
            this->_speed = _normalSpeed;
            this->setState(ZombieState::WALKING);
            this->setScale(1.0f);
            }), 
            MoveBy::create(0.0001f, Vec2(0, -40)), nullptr));
        
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
    if (_isEating) return;

    for (auto plant : plants)
    {
        if (plant && !plant->isDead())
        {
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
    _speed = 0;  // Stop moving
    setState(ZombieState::EATING);
    CCLOG("Zombie start eating plant!");
}

// Called when plant dies
void Imp::onPlantDied()
{
    _isEating = false;
    _speed = _normalSpeed;
    _targetPlant = nullptr;
    setState(ZombieState::WALKING);
    CCLOG("Zombie resume walking");
}
