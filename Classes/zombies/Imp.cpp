
#include "Imp.h"
#include "Plant.h"

USING_NS_CC;

// Protected constructor
Imp::Imp()
    : walk_action(nullptr)
    , eat_action(nullptr)
    , fly_action(nullptr)
    , is_flying(false)
{
    CCLOG("Zombie created.");
}

// Destructor
Imp::~Imp()
{
    CC_SAFE_RELEASE(walk_action);
    CC_SAFE_RELEASE(eat_action);
    CC_SAFE_RELEASE(fly_action);
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
        z->MOVE_SPEED = 40.0f;
        z->current_speed = z->MOVE_SPEED;
        z->current_health = 100;
        z->current_state = static_cast<int>(ZombieState::FLYING);
        z->setAnimationForState();
        return z;
    }
    delete z;
    return nullptr;
}

// Initialize walking animation
void Imp::initWalkAnimation()
{
    auto animation = initAnimate("imp_walk_spritesheet.png", 100.0f, 138.0f, 3, 5, 12, 0.08f);
    auto animate = Animate::create(animation);
    this->walk_action = RepeatForever::create(animate);
    walk_action->retain();
}

// Initialize eating animation
void Imp::initEatAnimation()
{
    auto animation = initAnimate("imp_eat_spritesheet.png", 100.0f, 138.0f, 2, 5, 7, 0.15f);
    auto animate = Animate::create(animation);
    this->eat_action = RepeatForever::create(animate);
    eat_action->retain();
}

void Imp::initFlyAnimation()
{
    auto animation = initAnimate("imp_fly_spritesheet.png", 145.0f, 200.0f, 4, 5, 23, 0.06f);
    this->fly_action = Animate::create(animation);
    fly_action->retain();
}


// Set animation corresponding to state
void Imp::setAnimationForState()
{
    switch (static_cast<ZombieState>(current_state))
    {
    case ZombieState::WALKING:
        CCLOG("Setting imp WALKING animation.");
        this->stopAllActions();
        this->runAction(walk_action);
        break;
    case ZombieState::EATING:
        CCLOG("Setting EATING animation.");
        this->stopAllActions();
        this->is_eating = false;
        this->current_speed = MOVE_SPEED;
        // ----------------
        this->runAction(eat_action);
        break;
    case ZombieState::FLYING:
    {
        this->is_flying = true;
        this->stopAllActions();
        this->runAction(Sequence::create(
            CallFunc::create([this]() {
                this->current_speed = 120.0f;
                }),
            fly_action,
            CallFunc::create([this]() {
                this->is_flying = false;
                this->current_speed = MOVE_SPEED;
                this->setState(static_cast<int>(ZombieState::WALKING));
                }),
            nullptr));
        break;
    }
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
    default:
        break;
    }
}

void Imp::updateMoving(float delta)
{
    // Only move when not eating
    float newX = this->getPositionX() - current_speed * delta;
    this->setPositionX(newX);

    // Only calculate vertical displacement when in flying state
    if (is_flying) {
        float newY = this->getPositionY() - 50.0f * delta; // Adjust value as needed
        this->setPositionY(newY);
    }
}

void Imp::encounterPlant(const std::vector<Plant*>& plants)
{
    // Return if flying, dead, or eating
    if (is_flying || is_eating || is_dead || is_dying)
    {
        return;
    }

    // Normal collision detection logic
    for (auto plant : plants)
    {
        if (plant && !plant->isDead())
        {
            // Adjust collision detection based on Imp's size
            Rect zombieRect = this->getBoundingBox();
            if (zombieRect.intersectsRect(plant->getBoundingBox()))
            {
                startEating(plant);
                return;
            }
        }
    }
}
