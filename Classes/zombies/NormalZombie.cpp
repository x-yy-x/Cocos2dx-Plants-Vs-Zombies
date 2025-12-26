
#include "NormalZombie.h"
#include "Plant.h"
#include "SpikeWeed.h"
#include "SpikeRock.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;


// Destructor
NormalZombie::~NormalZombie()
{
    CC_SAFE_RELEASE(_walkAction);
    CC_SAFE_RELEASE(_eatAction);
    CCLOG("NormalZombie destroyed.");
}

// Initialization function
bool NormalZombie::init()
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
NormalZombie* NormalZombie::createZombie()
{
    NormalZombie* z = new NormalZombie();
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

Sprite* NormalZombie::createShowcaseSprite(const Vec2& pos)
{
    auto animation = initAnimate("zombie_idle_spritesheet.png", 235.0f, 225.0f, 6, 5, 29, 0.05f);
    auto animate = Animate::create(animation);
    auto _idleAction = RepeatForever::create(animate);

    auto sp = Sprite::create();
    if (sp) {
        sp->setPosition(pos);
        sp->runAction(_idleAction);
    }
    return sp;
}

// Initialize walking animation
void NormalZombie::initWalkAnimation()
{
    auto animation = initAnimate("zombie_walk_spritesheet.png", 125.0f, 173.8f, 5, 10, 46, 0.05f);
    auto animate = Animate::create(animation);
    this->_walkAction = RepeatForever::create(animate);
    _walkAction->retain();
}

// Initialize eating animation
void NormalZombie::initEatAnimation()
{
    auto animation = initAnimate("zombie_eat_spritesheet.png", 125.0f, 173.8f, 4, 10, 39, 0.03f);
    auto animate = Animate::create(animation);
    this->_eatAction = RepeatForever::create(animate);
    _eatAction->retain();
}



// Set animation corresponding to state
void NormalZombie::setAnimationForState()
{
    switch (static_cast<ZombieState>(_currentState))
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
