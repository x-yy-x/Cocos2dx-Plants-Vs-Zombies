#include "PotatoMine.h"
#include "Zombie.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ---------- 静态常量 ----------
const float       PotatoMine::DEFAULT_ARMING_TIME = 12.0f; // 地下准备时间
const int         PotatoMine::EXPLOSION_DAMAGE   = 1800;   // 爆炸伤害
const int         PotatoMine::EXPLOSION_RADIUS   = 0;      // 同格子
const std::string PotatoMine::ARMING_IMAGE       = "potato_mine/arming.png";
const std::string PotatoMine::READY_FRAME_DIR    = "potato_mine/ready/"; // 帧目录
const std::string PotatoMine::TRIGGERED_IMAGE    = "potato_mine/triggered.png";

// ---------- 构造 ----------
PotatoMine::PotatoMine()
    : BombPlant()
    , _state(MineState::ARMING)
    , _armingTimer(DEFAULT_ARMING_TIME)
{
    _explosionDamage = EXPLOSION_DAMAGE;
    _explosionRadius = EXPLOSION_RADIUS;
}

// ---------- init ----------
bool PotatoMine::init()
{
    if (!BombPlant::init())
        return false;

    if (!Sprite::initWithFile(ARMING_IMAGE))
        return false;

    // 生命值设定（易被僵尸吃）
    _maxHealth      = 300;
    _currentHealth  = 300;

    // 地雷不需要冷却
    _cooldownInterval = 0.0f;
    _accumulatedTime  = 0.0f;

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

    // 播放破土动画 + 就绪循环动画
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
    if (_state != MineState::READY || _hasExploded)
        return;

    // 检测当前行僵尸碰撞
    auto& zombies = allZombiesInRow[plantRow];
    bool triggered = false;
    for (auto zombie : zombies)
    {
        if (zombie && !zombie->isDead())
        {
            if (this->getBoundingBox().intersectsRect(zombie->getBoundingBox()))
            {
                zombie->takeDamage(_explosionDamage);
                triggered = true;
            }
        }
    }

    if (triggered)
    {
        _state = MineState::TRIGGERED;
        _hasExploded = true;
        playExplosionAnimation();
    }
}

// ---------- playExplosionAnimation ----------
void PotatoMine::playExplosionAnimation()
{
    // 停止一切动作，并切换为爆炸贴图
    this->stopAllActions();
    this->setTexture(TRIGGERED_IMAGE);

    // 播放音效
    cocos2d::AudioEngine::play2d("mine.mp3", false);

    // 爆炸闪烁 + 渐隐后死亡
    auto blink = Blink::create(0.3f, 4);
    auto fade  = FadeOut::create(0.4f);
    auto die   = CallFunc::create([this]() {
        this->_isDead = true; // 标记死亡，GameWorld 会清理
    });

    this->runAction(Sequence::create(blink, fade, die, nullptr));
}

// ---------- setAnimation ----------
void PotatoMine::setAnimation()
{
    // 初始埋地阶段无需动画（单帧）
}

// ---------- plantAtPosition ----------
PotatoMine* PotatoMine::plantAtPosition(const cocos2d::Vec2& globalPos)
{
    return createPlantAtPosition<PotatoMine>(globalPos);
}

