
#include "BucketHeadZombie.h"
#include "Plant.h"

USING_NS_CC;

Sprite* BucketHeadZombie::createShowcaseSprite(const Vec2& pos)
{
    float frameWidth = 1200.0f;
    float frameHeight = 975.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "bucket_head_idle_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.06f);
    auto animate = Animate::create(animation);
    auto _idleAction = RepeatForever::create(animate);

    auto sp = Sprite::create("flag_zombie_idle_spritesheet.png", Rect(0, 0, frameWidth, frameHeight));
    if (sp) {
        sp->setPosition(pos);
        sp->setScale(0.2f);
        sp->runAction(_idleAction);
    }
    return sp;
}

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string BucketHeadZombie::IMAGE_FILENAME = "flag_zombie_walk_spritesheet.png";
const cocos2d::Rect BucketHeadZombie::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size BucketHeadZombie::OBJECT_SIZE = Size(125.0f, 173.8f);
const float BucketHeadZombie::MOVE_SPEED = 20.0f;       // Move 20 pixels per second
const float BucketHeadZombie::ATTACK_DAMAGE = 10.0f;    // Damage per attack
const float BucketHeadZombie::ATTACK_RANGE = 50.0f;     // Attack range

// Protected constructor
BucketHeadZombie::BucketHeadZombie()
    : _currentState(ZombieState::WALKING)
    , _isDead(false)
    , _maxHealth(200)
    , _currentHealth(200)
    , _bucketHealth(1000)
    , _attackInterval(0.5f)
    , _accumulatedTime(0.0f)
    , _zombiePos(Vec2::ZERO)
    , _walkAction(nullptr)
    , _eatAction(nullptr)
    , _isEating(false)
    , _normalEatAction(nullptr)
    , _normalWalkAction(nullptr)
    , _targetPlant(nullptr)
    , _speed(MOVE_SPEED)
    , _normalSpeed(MOVE_SPEED)
    , _useNormalZombie(false)
    , _groanAudioId(cocos2d::AudioEngine::INVALID_AUDIO_ID)
{
    CCLOG("Zombie created.");
}

// Destructor
BucketHeadZombie::~BucketHeadZombie()
{
    if (_groanAudioId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_groanAudioId);
    }
    CC_SAFE_RELEASE(_walkAction);
    CC_SAFE_RELEASE(_eatAction);
    CCLOG("Zombie destroyed.");
}

// Initialization function
bool BucketHeadZombie::init()
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
BucketHeadZombie* BucketHeadZombie::createZombie()
{
    BucketHeadZombie* z = new BucketHeadZombie();
    if (z && z->init())
    {
        z->autorelease();
        z->initWalkAnimation();
        z->initEatAnimation();
        z->runAction(z->_walkAction);
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void BucketHeadZombie::initWalkAnimation()
{
    const float frameWidth = 125.0f;
    const float frameHeight = 173.8f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == 4 && col == 6)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "bucket_head_walk_spritesheet.png",
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
void BucketHeadZombie::initEatAnimation()
{
    const float frameWidth = 125.0f;
    const float frameHeight = 173.8f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == 3 && col == 9)
                break;

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "bucket_head_eat_spritesheet.png",
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

// Update every frame
void BucketHeadZombie::update(float delta)
{
    if (_isDead || _isDying)
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
            //CCLOG("Zombie reached the house!");
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
BucketHeadZombie::ZombieState BucketHeadZombie::getState() const
{
    CCLOG("getstate����");
    return _currentState;
}

// Set zombie state
void BucketHeadZombie::setState(ZombieState newState)
{

    if (_currentState != newState)
    {
        _currentState = newState;
        CCLOG("Zombie state changed.");
        setAnimationForState(newState);
    }
}

// Check if dead
bool BucketHeadZombie::isDead() const
{
    return _isDead;
}

// Take damage
void BucketHeadZombie::takeDamage(int damage)
{
    if (_isDead || _isDying)
    {
        return;
    }
    
    if (_bucketHealth > 0)
    {
        _bucketHealth -= damage;
        if (_bucketHealth <= 0) {
            _currentHealth += _bucketHealth;
            onBucketBroken();
        }
    }
    else
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

        // Stop all actions
        this->stopAllActions();

        // Play fade out animation, then mark as dead
        auto fadeOut = FadeOut::create(0.5f);
        auto markDead = CallFunc::create([this]() {
            _isDead = true;
            _isDying = false;           
            });
        auto sequence = Sequence::create(fadeOut, markDead, nullptr);
        this->runAction(sequence);
    }
}

// Set animation default implementation
void BucketHeadZombie::setAnimation()
{
    CCLOG("Zombie::setAnimation() called.");
    setAnimationForState(_currentState);
}

// Set animation corresponding to state
void BucketHeadZombie::setAnimationForState(ZombieState state)
{
    switch (state)
    {
        case ZombieState::WALKING:
            CCLOG("Setting WALKING animation.");
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
        case ZombieState::DYING:
            CCLOG("Setting DYING animation.");
            break;
        default:
            break;
    }
}

// Check and handle plant encounters
void BucketHeadZombie::encounterPlant(const std::vector<Plant*>& plants)
{
    checkCollision(plants);
}

// Check collision with plants
void BucketHeadZombie::checkCollision(const std::vector<Plant*>& plants)
{
    if (_isEating) return;

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
void BucketHeadZombie::startEating(Plant* plant)
{
    _isEating = true;
    _targetPlant = plant;
    _speed = 0;  // Stop moving
    setState(ZombieState::EATING);
    CCLOG("Zombie start eating plant!");
}

// Called when plant dies
void BucketHeadZombie::onPlantDied()
{
    _isEating = false;
    _speed = _normalSpeed;
    _targetPlant = nullptr;
    setState(ZombieState::WALKING);
    CCLOG("Zombie resume walking");
}

void BucketHeadZombie::onBucketBroken()
{
    if (_useNormalZombie) return;

    _useNormalZombie = true;

    int frameIndex = -1;
    // 保持当前帧 index
    if (_isEating){
        auto action = dynamic_cast<RepeatForever*>(_eatAction);
        auto animate = dynamic_cast<Animate*>(action->getInnerAction());
        frameIndex = animate->getCurrentFrameIndex();
    }
    else {
        auto action = dynamic_cast<RepeatForever*>(_walkAction);
        auto animate = dynamic_cast<Animate*>(action->getInnerAction());
        frameIndex = animate->getCurrentFrameIndex();
    }
    log("frameindex=%d", frameIndex);
    stopAllActions();

    if (_isEating) {
        _walkAction = createNormalWalkActionFromFrame(1);
        _eatAction = createNormalEatActionFromFrame(frameIndex + 1);
        _walkAction->retain();
        _eatAction->retain();
        runAction(_eatAction);
    }
    else {
        _walkAction = createNormalWalkActionFromFrame(frameIndex + 1);
        _eatAction = createNormalEatActionFromFrame(1);
        _walkAction->retain();
        _eatAction->retain();
        runAction(_walkAction);
    }

}

RepeatForever* BucketHeadZombie::createNormalWalkActionFromFrame(int startFrame)
{
    const float w = 125.0f;
    const float h = 173.8f;

    Vector<SpriteFrame*> frames;

    for (int i = 0; i < 45; ++i)
    {
        int index = (startFrame + i) % 45;
        int row = index / 10;
        int col = index % 10;

        frames.pushBack(SpriteFrame::create(
            "zombie_walk_spritesheet.png",
            Rect(col * w, row * h, w, h)
        ));
    }

    auto anim = Animation::createWithSpriteFrames(frames, 0.05f);
    return RepeatForever::create(Animate::create(anim));
}

RepeatForever* BucketHeadZombie::createNormalEatActionFromFrame(int startFrame)
{
    const float w = 125.0f;
    const float h = 173.8f;

    Vector<SpriteFrame*> frames;

    for (int i = 0; i < 39; ++i)
    {
        int index = (startFrame + i) % 39;
        int row = index / 10;
        int col = index % 10;

        frames.pushBack(SpriteFrame::create(
            "zombie_eat_spritesheet.png",
            Rect(col * w, row * h, w, h)
        ));
    }

    auto anim = Animation::createWithSpriteFrames(frames, 0.03f);
    return RepeatForever::create(Animate::create(anim));
}