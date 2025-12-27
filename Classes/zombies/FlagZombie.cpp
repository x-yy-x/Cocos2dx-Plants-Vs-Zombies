
#include "FlagZombie.h"
#include "Plant.h"

USING_NS_CC;

Sprite* FlagZombie::createShowcaseSprite(const Vec2& pos)
{
    auto animation = initAnimate("flag_zombie_idle_spritesheet.png", 250.0f, 250.0f, 6, 5, 29, 0.05f);
    auto animate = Animate::create(animation);
    auto _idleAction = RepeatForever::create(animate);

    auto sp = Sprite::create();
    if (sp) {
        sp->setPosition(pos);
        sp->runAction(_idleAction);
    }
    return sp;
}


// Destructor
FlagZombie::~FlagZombie()
{
    CC_SAFE_RELEASE(_walkAction);
    CC_SAFE_RELEASE(_eatAction);
    CCLOG("Zombie destroyed.");
}

// Initialization function
bool FlagZombie::init()
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
FlagZombie* FlagZombie::createZombie()
{
    FlagZombie* z = new FlagZombie();
    if (z && z->init())
    {
        z->autorelease();
        z->initWalkAnimation();
        z->initEatAnimation();
        z->X_CORRECTION = 120.0f;
        z->SIZE_CORRECTION = 200.0f;
        z->runAction(z->_walkAction);
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void FlagZombie::initWalkAnimation()
{
    auto animation = initAnimate("flag_zombie_walk_spritesheet.png", 208.0f, 180.0f, 3, 5, 12, 0.18f);
    auto animate = Animate::create(animation);
    this->_walkAction = RepeatForever::create(animate);
    _walkAction->retain();
}

// Initialize eating animation
void FlagZombie::initEatAnimation()
{
    auto animation = initAnimate("flag_zombie_eat_spritesheet.png", 208.0f, 180.0f, 3, 5, 11, 0.1f);
    auto animate = Animate::create(animation);
    this->_eatAction = RepeatForever::create(animate);
    _eatAction->retain();
}



// Set animation corresponding to state
void FlagZombie::setAnimationForState()
{
    switch (static_cast<ZombieState>(current_state))
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
                is_dead = true;
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