
#include "PoleVaulter.h"
#include "Plant.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------

const float PoleVaulter::RUNNING_SPEED = 40.0f;

// Protected constructor
PoleVaulter::PoleVaulter()
    : walk_action(nullptr)
    , eat_action(nullptr)
    , jump_action(nullptr)
    , run_action(nullptr)
    , is_jumping(false)
    , has_jumped(false)
{
    CCLOG("Zombie created.");
}

// Destructor
PoleVaulter::~PoleVaulter()
{
    CC_SAFE_RELEASE(walk_action);
    CC_SAFE_RELEASE(eat_action);
    CC_SAFE_RELEASE(run_action);
    CC_SAFE_RELEASE(jump_action);
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
        z->current_state = static_cast<int>(ZombieState::RUNNING);
        z->current_speed = RUNNING_SPEED;
        z->runAction(z->run_action);
        return z;
    }
    delete z;
    return nullptr;
}

Sprite* PoleVaulter::createShowcaseSprite(const Vec2& pos)
{
    auto animation = initAnimate("pole_vaulter_idle_spritesheet.png", 1250.0f, 785.0f, 3, 5, 13, 0.06f);  
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

// Initialize walking animation
void PoleVaulter::initWalkAnimation()
{
    auto animation = initAnimate("pole_vaulter_walk_spritesheet.png", 125.0f, 225.0f, 5, 10, 44, 0.05f);
    auto animate = Animate::create(animation);
    this->walk_action = RepeatForever::create(animate);
    walk_action->retain();
}

// Initialize eating animation
void PoleVaulter::initEatAnimation()
{
    auto animation = initAnimate("pole_vaulter_eat_spritesheet.png", 125.0f, 225.0f, 3, 10, 37, 0.03f);
    auto animate = Animate::create(animation);
    this->eat_action = RepeatForever::create(animate);
    eat_action->retain();
}

// Initialize running animation
void PoleVaulter::initRunningAnimation()
{
    auto animation = initAnimate("pole_vaulter_run_spritesheet.png", 375.0f, 225.0f, 6, 6, 36, 0.03f);
    auto animate = Animate::create(animation);
    this->run_action = RepeatForever::create(animate);
    run_action->retain();
}

// Initialize jumping animation
void PoleVaulter::initJumpingAnimation()
{
    auto animation = initAnimate("pole_vaulter_jump_spritesheet.png", 625.0f, 225.0f, 11, 4, 42, 0.03f);
    this->jump_action = Animate::create(animation);
    jump_action->retain();
}

// Set animation corresponding to state
void PoleVaulter::setAnimationForState()
{
    auto jumpAnim = jump_action;

    switch (static_cast<ZombieState>(current_state))
    {
        case ZombieState::WALKING:
            CCLOG("Setting WALKING animation.");
            this->runAction(walk_action);
            this->stopAction(eat_action);
            this->stopAction(run_action);
            this->stopAction(jump_action);
            break;
        case ZombieState::EATING:
            CCLOG("Setting EATING animation.");
            this->stopAction(walk_action);
            this->runAction(eat_action);
            this->stopAction(run_action);
            this->stopAction(jump_action);
            break;

        case ZombieState::DYING:
        {
            CCLOG("Setting DYING animation.");
            this->stopAllActions();
            auto fadeOut = FadeOut::create(0.5f);
            auto markDead = CallFunc::create([this]() {
                is_dead = true;
                is_dying = false;
                });
            auto sequence = Sequence::create(fadeOut, markDead, nullptr);
            this->runAction(sequence);
            break;
        }
        case ZombieState::JUMPING:
            CCLOG("Setting JUMPING animation.");
            this->stopAllActions();
            this->runAction(
                Sequence::create(
                    jumpAnim,
                    CallFunc::create([this]() {
                        this->is_jumping = false;
                        this->current_speed = MOVE_SPEED;
                        this->setState(static_cast<int>(ZombieState::WALKING));
                        }),
                    MoveBy::create(0.0001f, Vec2(-170, 0)),
                    nullptr
                )
            );
            break;

        case ZombieState::RUNNING:
            CCLOG("Setting RUNNING animation.");
            this->stopAction(walk_action);
            this->stopAction(eat_action);
            this->runAction(run_action);
            this->stopAction(jump_action);
            break;
        default:
            break;
    }
}


// Check and handle plant encounters
void PoleVaulter::encounterPlant(const std::vector<Plant*>& plants)
{
    // Return if zombie is eating or jumping
    if (is_eating || is_jumping) return;

    for (auto plant : plants)
    {
        if (plant && !plant->isDead())
        {            
            // Skip spike plants (cannot be jumped over)
            if (plant->isSpike())
                continue;
            
            // Adjust collision detection based on jump state
            float COLLISION_OFFSET_X = 40.0f;
            float SIZE_CORRECTION = 100.0f;
            if (!has_jumped) {
                COLLISION_OFFSET_X = 110.0f;
                SIZE_CORRECTION = 160.0f;
            }
            
            Rect zombieRect = this->getBoundingBox();

            zombieRect.origin.x += COLLISION_OFFSET_X;
            zombieRect.size.width -= SIZE_CORRECTION;

            if (zombieRect.intersectsRect(plant->getBoundingBox()))
            {
                CCLOG("!!!should jump!!!");
                if (has_jumped)
                    startEating(plant);
                else
                    startJumping();
                return;
            }
        }
    }
}

void PoleVaulter::startJumping()
{
    // Return if already jumping
    if (is_jumping) return;

    is_jumping = true;
    current_speed = 0;
    setState(static_cast<int>(ZombieState::JUMPING));
    CCLOG("Zombie start jumping!");
    cocos2d::AudioEngine::play2d("polevault.mp3", false);
}
