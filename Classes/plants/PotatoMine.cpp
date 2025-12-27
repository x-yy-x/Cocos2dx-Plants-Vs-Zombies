#include "PotatoMine.h"
#include "Zombie.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ---------- Static constants ----------
const float       PotatoMine::DEFAULT_ARMING_TIME = 12.0f; // Underground preparation time
const int         PotatoMine::EXPLOSION_DAMAGE   = 1800;   // Explosion damage
const int         PotatoMine::EXPLOSION_RADIUS   = 0;      // Same grid
const std::string PotatoMine::ARMING_IMAGE       = "potato_mine/arming.png";
const std::string PotatoMine::READY_FRAME_DIR    = "potato_mine/ready/"; // Frame directory
const std::string PotatoMine::TRIGGERED_IMAGE    = "potato_mine/triggered.png";

// ---------- Constructor ----------
PotatoMine::PotatoMine()
    : BombPlant()
    , _state(MineState::ARMING)
    , _armingTimer(DEFAULT_ARMING_TIME)
{
    explosion_damage = EXPLOSION_DAMAGE;
    explosion_radius = EXPLOSION_RADIUS;
}

// ---------- init ----------
bool PotatoMine::init()
{
    if (!BombPlant::init())
        return false;

    if (!Sprite::initWithFile(ARMING_IMAGE))
        return false;

    // Set health value (easy to be eaten by zombies)
    max_health      = 80;
    current_health  = 80;

    // Mine doesn't need cooldown
    cooldown_interval = 0.0f;
    accumulated_time  = 0.0f;

    this->scheduleUpdate();
    return true;
}

// ---------- update ----------
void PotatoMine::update(float delta)
{
    Plant::update(delta);

    switch (_state)
    {
    case MineState::ARMING:
        _armingTimer -= delta;
        if (_armingTimer <= 0.0f)
        {
            switchToReadyState();
        }
        break;
    case MineState::READY:
        // nothing special
        break;
    case MineState::TRIGGERED:
        // 等待动画结束即可
        break;
    }
}

// ---------- switchToReadyState ----------
void PotatoMine::switchToReadyState()
{
    _state = MineState::READY;

    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 8; ++i)
    {
        std::string frameFile = READY_FRAME_DIR + "1 (" + std::to_string(i) + ").png";
        auto frame = SpriteFrame::create(frameFile, Rect(0, 0, this->getContentSize().width, this->getContentSize().height));
        if (frame)
        {
            frames.pushBack(frame);
        }
    }

    if (!frames.empty())
    {
        float per = 0.1f;
        auto animation = Animation::createWithSpriteFrames(frames, per);
        auto animate = Animate::create(animation);
        this->runAction(RepeatForever::create(animate));
    }
}

// ---------- explode ----------
void PotatoMine::explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol)
{
    if (_state != MineState::READY || has_exploded)
        return;

    // Detect zombie collision in current row
    auto& zombies = allZombiesInRow[plantRow];
    bool triggered = false;
    for (auto zombie : zombies)
    {
        if (zombie && !zombie->isDead())
        {
            if (this->getBoundingBox().intersectsRect(zombie->getBoundingBox()))
            {
                zombie->takeDamage(static_cast<float>(explosion_damage));
                triggered = true;
            }
        }
    }

    if (triggered)
    {
        _state = MineState::TRIGGERED;
        has_exploded = true;
        playExplosionAnimation();
    }
}

// ---------- playExplosionAnimation ----------
void PotatoMine::playExplosionAnimation()
{
    // Stop all actions and switch to explosion texture
    this->stopAllActions();
    this->setTexture(TRIGGERED_IMAGE);

    // Play sound effect
    cocos2d::AudioEngine::play2d("mine.mp3", false);

    // Explosion blink + fade out then die
    auto blink = Blink::create(0.3f, 4);
    auto fade  = FadeOut::create(0.4f);
    auto die   = CallFunc::create([this]() {
        this->is_dead = true;
    });

    this->runAction(Sequence::create(blink, fade, die, nullptr));
}

// ---------- setAnimation ----------
void PotatoMine::setAnimation()
{
    // No animation needed for initial underground phase (single frame)
}

// ---------- plantAtPosition ----------
PotatoMine* PotatoMine::plantAtPosition(const cocos2d::Vec2& globalPos)
{
    return createPlantAtPosition<PotatoMine>(globalPos);
}

