
#include "PoleVaulter.h"
#include "Plant.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

Sprite* PoleVaulter::createShowcaseSprite(const Vec2& pos)
{
    float frameWidth = 1250.0f;
    float frameHeight = 785.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (row == 2 && col == 3)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "pole_vaulter_idle_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.06f);
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

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string PoleVaulter::IMAGE_FILENAME = "pole_vaulter_walk_spritesheet.png";
const cocos2d::Rect PoleVaulter::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size PoleVaulter::OBJECT_SIZE = Size(125.0f, 173.8f);
const float PoleVaulter::MOVE_SPEED = 20.0f;       // Move 20 pixels per second
const float PoleVaulter::ATTACK_DAMAGE = 10.0f;    // Damage per attack
const float PoleVaulter::ATTACK_RANGE = 50.0f;     // Attack range
const float PoleVaulter::RUNNING_SPEED = 40.0f;
// Protected constructor
PoleVaulter::PoleVaulter()
    : _currentState(PoleVaulterState::RUNNING)
    , _isDead(false)
    , _maxHealth(300)
    , _currentHealth(300)
    , _attackInterval(0.5f)
    , _accumulatedTime(0.0f)
    , _zombiePos(Vec2::ZERO)
    , _walkAction(nullptr)
    , _eatAction(nullptr)
    , _jumpAction(nullptr)
    , _runAction(nullptr)
    , _isEating(false)
    , _isJumping(false)
    , _targetPlant(nullptr)
    , _speed(RUNNING_SPEED)
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
        z->runAction(z->_runAction);
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void PoleVaulter::initWalkAnimation()
{
    const float frameWidth = 125;
    const float frameHeight = 225;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == 4 && col == 4)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "pole_vaulter_walk_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.05f);
    auto animate = Animate::create(animation);

    this->_walkAction = RepeatForever::create(animate);
    _walkAction->retain();
}

// Initialize eating animation
void PoleVaulter::initEatAnimation()
{
    const float frameWidth = 125;
    const float frameHeight = 225;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == 2 && col == 7)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "pole_vaulter_eat_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.03f);
    auto animate = Animate::create(animation);

    this->_eatAction = RepeatForever::create(animate);
    _eatAction->retain();
}

void PoleVaulter::initRunningAnimation()
{
    const float frameWidth = 375;
    const float frameHeight = 225;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 6; col++)
        {

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "pole_vaulter_run_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.03f);
    auto animate = Animate::create(animation);

    this->_runAction = RepeatForever::create(animate);
    _runAction->retain();
}

void PoleVaulter::initJumpingAnimation()
{
    const float frameWidth = 625;
    const float frameHeight = 225;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 11; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (row == 10 && col == 2)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "pole_vaulter_jump_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.03f);
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
        float newX = this->getPositionX() - _speed * delta;
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
PoleVaulter::PoleVaulterState PoleVaulter::getState() const
{
    CCLOG("getstated����");
    return _currentState;
}

// Set zombie state
void PoleVaulter::setState(PoleVaulterState newState)
{
    if (_currentState != newState)
    {
        _currentState = newState;
        CCLOG("Zombie state changed.");
        setAnimationForState(newState);
    }
}

// Check if dead
bool PoleVaulter::isDead() const
{
    return _isDead;
}

// Take damage
void PoleVaulter::takeDamage(int damage)
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
        _isJumping = false;
        
        setState(PoleVaulterState::DYING);
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
void PoleVaulter::setAnimation()
{
    CCLOG("Zombie::setAnimation() called.");
    setAnimationForState(_currentState);
}

// Set animation corresponding to state
void PoleVaulter::setAnimationForState(PoleVaulterState state)
{
    // ���� switch ��!
    auto jumpAnim = _jumpAction;

    switch (state)
    {
        case PoleVaulterState::WALKING:
            CCLOG("Setting WALKING animation.");
            this->runAction(_walkAction);
            this->stopAction(_eatAction);
            this->stopAction(_runAction);
            this->stopAction(_jumpAction);
            break;

        case PoleVaulterState::EATING:
            CCLOG("Setting EATING animation.");
            this->stopAction(_walkAction);
            this->runAction(_eatAction);
            this->stopAction(_runAction);
            this->stopAction(_jumpAction);
            break;

        case PoleVaulterState::DYING:
            CCLOG("Setting DYING animation.");
            break;

        case PoleVaulterState::JUMPING:
            CCLOG("Setting JUMPING animation.");
            this->stopAllActions();

            // ��Ծ����д��һ���� Sequence
            this->runAction(
                Sequence::create(
                    jumpAnim,
                    CallFunc::create([this]() {
                        this->_isJumping = false;
                        this->_speed = _normalSpeed;
                        this->setState(PoleVaulterState::WALKING);  // ������� rush ״̬
                        }),
                    MoveBy::create(0.0001f, Vec2(-170, 0)),
                    nullptr
                )
            );
            break;

        case PoleVaulterState::RUNNING:
            CCLOG("Setting RUNNING animation.");
            this->stopAction(_walkAction);
            this->stopAction(_eatAction);
            this->runAction(_runAction);
            this->stopAction(_jumpAction);
            break;

        default:
            break;
    }
}


// Check and handle plant encounters
void PoleVaulter::encounterPlant(const std::vector<Plant*>& plants)
{
    checkCollision(plants);
}

// Check collision with plants
void PoleVaulter::checkCollision(const std::vector<Plant*>& plants)
{
    if (_isEating||_isJumping) return;

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

// Start eating a plant
void PoleVaulter::startEating(Plant* plant)
{
    _isEating = true;
    _targetPlant = plant;
    _speed = 0;  // Stop moving
    setState(PoleVaulterState::EATING);
    CCLOG("Zombie start eating plant!");
}

void PoleVaulter::startJumping()
{
    _hasJumped = true;
    _isJumping = true;
    _speed = 0;
    setState(PoleVaulterState::JUMPING);
    CCLOG("Zombie start jumping!");
    cocos2d::AudioEngine::play2d("polevault.mp3", false);
}

// Called when plant dies
void PoleVaulter::onPlantDied()
{
    _isEating = false;
    _speed = _normalSpeed;
    _targetPlant = nullptr;
    setState(PoleVaulterState::WALKING);
    CCLOG("Zombie resume walking");
}
