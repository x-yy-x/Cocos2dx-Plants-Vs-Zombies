
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

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------

const float Zomboni::MOVE_SPEED = 20.0f;
const float Zomboni::ICE_STEP = 10.0f;
const int Zomboni::ICE_COUNT = 15;
const float Zomboni::ICE_LENGTH = 150.0f;

// Protected constructor
Zomboni::Zomboni()
    : _currentState(ZombieState::DRIVING)
    , _isDead(false)
    , _maxHealth(1000)
    , _currentHealth(1000)
    , _zombiePos(Vec2::ZERO)
    , _driveAction(nullptr)
    , _specialDieAction(nullptr)
    , _targetPlant(nullptr)
    , _speed(MOVE_SPEED)
    , _iceAccumulate(0.0f)
    , _iceIndex(0)
{
    CCLOG("Zombie created.");
}

// Destructor
Zomboni::~Zomboni()
{
    CC_SAFE_RELEASE(_driveAction);
    CC_SAFE_RELEASE(_eatAction);
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
    cocos2d::AudioEngine::play2d("zomboni.mp3", false, 1.0f);
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
    const float frameWidth = 470.0f;
    const float frameHeight = 450.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 4; col++)
        {

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "zomboni_drive_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.08f);
    auto animate = Animate::create(animation);

    this->_driveAction = RepeatForever::create(animate);
    _driveAction->retain();
}

// Initialize eating animation
void Zomboni::initSpecialDieAnimation()
{
    const float frameWidth = 600.0f;
    const float frameHeight = 525.0f;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 6; col++)
        {

            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "zomboni_special_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.08f);
    this->_specialDieAction = Animate::create(animation);
    _specialDieAction->retain();
}

// Update every frame
void Zomboni::update(float delta)
{
    if (_isDead || _isDying)
        return;

    float dx = _speed * delta;
    float newX = this->getPositionX() - dx;
    this->setPositionX(newX);

    // 累积移动距离
    _iceAccumulate += dx;

    if (_iceAccumulate >= ICE_STEP)
    {
        _iceAccumulate -= ICE_STEP;
        spawnIce();   // 铺一块冰
    }

    if (newX < -100)
    {
        CCLOG("Zombie reached the house!");
    }
}


// Set zombie state
void Zomboni::setState(ZombieState newState)
{

    if (_currentState != newState)
    {
        _currentState = newState;
        CCLOG("Zombie state changed.");
        setAnimationForState(newState);
    }
}

// Check if dead
bool Zomboni::isDead() const
{
    return _isDead;
}

// Take damage
void Zomboni::takeDamage(int damage)
{
    if (_isDead || _isDying)
    {
        return;
    }
    int r = cocos2d::random(1, 3);
    switch (r) {
        case 1:
            cocos2d::AudioEngine::play2d("hittingiron1.mp3");
            break;
        case 2:
            cocos2d::AudioEngine::play2d("hittingiron2.mp3");
            break;
        case 3:
            cocos2d::AudioEngine::play2d("hittingiron3.mp3");
            break;
        default:
            break;
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
               
    }
}

// Set animation default implementation
void Zomboni::setAnimation()
{
    CCLOG("Zombie::setAnimation() called.");
    setAnimationForState(_currentState);
}

// Set animation corresponding to state
void Zomboni::setAnimationForState(ZombieState state)
{
    switch (state)
    {
    case ZombieState::DRIVING:
        CCLOG("Setting DRIVING animation.");
        this->stopAllActions();
        this->runAction(_driveAction);
        break;
    case ZombieState::DYING:
    {
        CCLOG("Setting EATING animation.");
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
        this->stopAllActions();
        this->_isDying = true;
        this->runAction(Sequence::create(_specialDieAction, CallFunc::create([this]() {
            _isDying = true;
            this->setState(ZombieState::DYING);
            }),nullptr));
        break;
    default:
        break;
    }
}

// Check and handle plant encounters
void Zomboni::encounterPlant(const std::vector<Plant*>& plants)
{
    checkCollision(plants);
}

// Check collision with plants
void Zomboni::checkCollision(const std::vector<Plant*>& plants)
{

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
            float COLLISION_OFFSET_X = 45.0f;
            float SIZE_CORRECTION = 100.0f;
            Rect zombieRect = this->getBoundingBox();
            // Move the rect slightly to the left/right or adjust width
            // Here we check if the zombie's "eating mouth" (left side) overlaps with plant
            zombieRect.origin.x += COLLISION_OFFSET_X; 
            zombieRect.size.width -= SIZE_CORRECTION;

            if (zombieRect.intersectsRect(plant->getBoundingBox()))
            {               
                plant->takeDamage(1000);
                return;
            }
        }
    }
}

void Zomboni::spawnIce()
{    
    // 计算裁剪区域（从右往左）
    float rectX = ICE_LENGTH - (_iceIndex + 1) * ICE_STEP;
    Rect iceRect(rectX, 0, ICE_STEP, 90.0f); // 90 = 冰图片高度

    // 创建冰 sprite
    auto ice = IceTile::create(this->getPosition(), _iceIndex);
    if (!ice) return;


    // 交给 GameWorld 管理（⚠ 不要自己留指针）
    auto gameWorld = dynamic_cast<GameWorld*>(this->getParent());
    if (gameWorld)
    {
        gameWorld->addIceTile(ice);
    }

    // index 循环
    _iceIndex = (_iceIndex + 1) % ICE_COUNT;
}

void Zomboni::setSpecialDeath()
{
    this->_currentHealth -= 10000.0f;
    this->setState(ZombieState::SPECIAL);
}

