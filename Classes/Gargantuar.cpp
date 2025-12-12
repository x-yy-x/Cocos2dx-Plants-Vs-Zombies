
#include "Gargantuar.h"
#include "GameWorld.h"
#include "Imp.h"
#include "Plant.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Gargantuar::IMAGE_FILENAME = "gargantuar_walk_spritesheet.png";
const cocos2d::Rect Gargantuar::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Gargantuar::OBJECT_SIZE = Size(125.0f, 173.8f);
const float Gargantuar::MOVE_SPEED = 20.0f;       // Move 20 pixels per second
const float Gargantuar::ATTACK_DAMAGE = 1000.0f;    // Damage per attack
const float Gargantuar::ATTACK_RANGE = 50.0f;     // Attack range

// Protected constructor
Gargantuar::Gargantuar()
    : _currentState(ZombieState::WALKING)
    , _isDead(false)
    , _maxHealth(3000)
    , _currentHealth(3000)
    , _attackInterval(2.64f)
    , _accumulatedTime(0.0f)
    , _zombiePos(Vec2::ZERO)
    , _walkAction(nullptr)
    , _smashAction(nullptr)
    , _throwAction(nullptr)
    , _isSmashing(false)
    , _isThrowing(false)
    , _targetPlant(nullptr)
    , _speed(MOVE_SPEED)
    , _normalSpeed(MOVE_SPEED)
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
        z->runAction(z->_walkAction);
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void Gargantuar::initWalkAnimation()
{
    const float frameWidth = 280.0f;
    const float frameHeight = 292.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            if (row == 6 && col == 4)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "gargantuar_walk_spritesheet.png",
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
void Gargantuar::initSmashAnimation()
{
    const float frameWidth = 395.0f;
    const float frameHeight = 365.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (row == 6 && col == 3)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "gargantuar_smash_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.08f);
    auto animate = Animate::create(animation);

    this->_smashAction = Animate::create(animation);
    _smashAction->retain();

}

void Gargantuar::initThrowAnimation()
{
    const float frameWidth = 469.0f;
    const float frameHeight = 400.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == 3 && col == 8)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "gargantuar_throw_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.08f);
    auto animate = Animate::create(animation);

    this->_throwAction = Animate::create(animation);
    _throwAction->retain();
}

// Update every frame
void Gargantuar::update(float delta)
{
    if (_isDead)
    {
        return;
    }

    // If zombie is not eating, continue walking left
    if (!_isSmashing&&!_isThrowing)
    {
        if (_currentHealth <= 1500 && _currentHealth > 0&&_hasthrown==false) {
            this->_speed = 0;
            this->_isThrowing = true;
            setState(ZombieState::THROWING);
        }
        float newX = this->getPositionX() - _speed * delta;
        this->setPositionX(newX);

        // Check if zombie reached the left edge (game over condition)
        if (newX < -100)
        {
            // Zombie reached the house - game over
            CCLOG("Zombie reached the house!");
        }
    }
    else if(_isSmashing)
    {
        _accumulatedTime += delta;
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
                _accumulatedTime = 0.0f;
            }
        }
    }
}

// Get zombie state
Gargantuar::ZombieState Gargantuar::getState() const
{
    CCLOG("getstate����");
    return _currentState;
}

// Set zombie state
void Gargantuar::setState(ZombieState newState)
{
    if (_currentState != newState)
    {
        _currentState = newState;
        CCLOG("Zombie state changed.");
        setAnimationForState(newState);
        
    }
}

// Check if dead
bool Gargantuar::isDead() const
{
    return _isDead;
}

// Take damage
void Gargantuar::takeDamage(int damage)
{
    if (_isDead)
    {
        return;
    }

    _currentHealth -= damage;
     CCLOG("Zombie took %d damage, remaining health: %d", damage, _currentHealth); // Reduced logging

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
void Gargantuar::setAnimation()
{
    CCLOG("Zombie::setAnimation() called.");
    setAnimationForState(_currentState);
}

// Set animation corresponding to state
void Gargantuar::setAnimationForState(ZombieState state)
{
    switch (state)
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
                MoveBy::create(0, Vec2(-20, 55)), _smashAction, MoveBy::create(0, Vec2(20, -55)), nullptr
            ));
            break;
        case ZombieState::THROWING:
            CCLOG("setting throwing animation");
            this->stopAllActions();
            this->runAction(
                Sequence::create(
                    MoveBy::create(0, Vec2(-46, 35)),
                    _throwAction,
                    CallFunc::create([this]() {
                        this->_hasthrown = true;
                        this->_isThrowing = false;
                        this->_speed = _normalSpeed;
                        this->throwImp();
                        this->setState(ZombieState::WALKING);
                        }),
                    MoveBy::create(0, Vec2(46, -35)),
                    nullptr
                )
            );
        case ZombieState::DYING:
            CCLOG("Setting gargantuar DYING animation.");
            break;
        default:
            break;
    }
}

// Check and handle plant encounters
void Gargantuar::encounterPlant(const std::vector<Plant*>& plants)
{
    checkCollision(plants);
}

// Check collision with plants
void Gargantuar::checkCollision(const std::vector<Plant*>& plants)
{
    if (_isSmashing||_isThrowing)
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

// Start eating a plant
void Gargantuar::startEating(Plant* plant)
{
    _isSmashing = true;
    _targetPlant = plant;
    _speed = 0;
    setState(ZombieState::SMASHING);
}

// Called when plant dies
void Gargantuar::onPlantDied()
{
    _isSmashing = false;
    _speed = _normalSpeed;
    _targetPlant = nullptr;
    setState(ZombieState::WALKING);
    CCLOG("Zombie resume walking");
}

void Gargantuar::throwImp()
{
    CCLOG("imp created");
    auto imp = Imp::createZombie();
    imp->setState(Imp::ZombieState::FLYING);
    if (imp)
    {
        CCLOG("imp!=null");
        auto pos = this->getPosition();
        imp->setPosition(pos + Vec2(-200, 0));
        CCLOG("imp pos%f,%f", imp->getPositionX(), imp->getPositionY());
        auto gameWorld = dynamic_cast<GameWorld*>(Director::getInstance()->getRunningScene());
        if (gameWorld)
        {
            CCLOG("gameworld!=null");
            gameWorld->addZombie(imp);
        }
    }
}

