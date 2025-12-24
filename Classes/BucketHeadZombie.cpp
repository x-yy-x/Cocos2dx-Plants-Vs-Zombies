
#include "BucketHeadZombie.h"
#include "Plant.h"

USING_NS_CC;

Sprite* BucketHeadZombie::createShowcaseSprite(const Vec2& pos)
{
    auto animation = initAnimate("bucket_head_idle_spritesheet.png", 1200.0f, 975.0f, 7, 5, 35, 0.06f);
    auto animate = Animate::create(animation);
    auto _idleAction = RepeatForever::create(animate);

    auto sp = Sprite::create();
    if (sp) {
        sp->setPosition(pos);
        sp->setScale(0.2f);
        sp->runAction(_idleAction);
    }
    return sp;
}


// Destructor
BucketHeadZombie::~BucketHeadZombie()
{

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
    auto animation = initAnimate("bucket_head_walk_spritesheet.png", 125.0f, 173.8f, 5, 10, 46, 0.05f);
    auto animate = Animate::create(animation);
    this->_walkAction = RepeatForever::create(animate);
    _walkAction->retain();
}

// Initialize eating animation
void BucketHeadZombie::initEatAnimation()
{
    auto animation = initAnimate("bucket_head_eat_spritesheet.png", 125.0f, 173.8f, 4, 10, 39, 0.03f);
    auto animate = Animate::create(animation);
    this->_eatAction = RepeatForever::create(animate);
    _eatAction->retain();
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


// Take damage
void BucketHeadZombie::takeDamage(float damage)
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


    if (_currentHealth <= 0)
    {
        _currentHealth = 0;

        // Mark as dying (playing death animation)
        _isDying = true;

        // CRITICAL: Clear target plant pointer to prevent dangling pointer access
        _targetPlant = nullptr;
        _isEating = false;

        setState(ZombieState::DYING);
    }
}


// Set animation corresponding to state
void BucketHeadZombie::setAnimationForState(ZombieState state)
{
    switch (state)
    {
        case ZombieState::WALKING:
            CCLOG("Setting WALKING animation.");
            this->stopAllActions();
            this->runAction(_walkAction);            
            break;
        case ZombieState::EATING:
            CCLOG("Setting EATING animation.");
            this->stopAllActions();
            this->runAction(_eatAction);
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