#include "Coin.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Coin::IMAGE_FILENAME[3] = { "coin_silver_dollar.png", "coin_gold_dollar.png", "Diamond.png" };
const int Coin::COIN_VALUE[3] = { 10, 50, 1000 };
const float Coin::LIFETIME = 10.0f;

// Constructor with member initializer list
Coin::Coin()
    : is_collected(false)
    , is_collecting(false)
    , life_time(0.0f)
    , target_pos(Vec2::ZERO)
    , coin_scale(1.0f)
    , coin_type(CoinType::SILVER)
{
    CCLOG("Coin instance created.");
}

// Destructor
Coin::~Coin()
{
    CCLOG("Coin instance destroyed.");
}

// Initialization logic
bool Coin::init(CoinType coinType)
{
    this->coin_type = coinType;

    if (!GameObject::init())
    {
        return false;
    }

    // Initialize sprite with the corresponding texture based on type
    if (!Sprite::initWithFile(IMAGE_FILENAME[static_cast<int>(coin_type)]))
    {
        CCLOG("Failed to load coin image: %s", IMAGE_FILENAME[static_cast<int>(coin_type)].c_str());
        return false;
    }

    // Apply visual properties
    this->setScale(coin_scale);
    is_collected = false;
    life_time = 0.0f;

    // Trigger idle animations (spinning effect)
    setAnimation();

    // Enable per-frame update scheduling
    this->scheduleUpdate();

    return true;
}

// Per-frame logic update
void Coin::update(float delta)
{
    // Do not update lifetime if the coin is already being processed for collection
    if (is_collected || is_collecting)
    {
        return;
    }

    life_time += delta;
}

// Returns true if the coin is interactive
bool Coin::isCollectible() const
{
    return !is_collected && !is_collecting;
}

// Collection sequence: plays sound and moves to UI bank
void Coin::collect(const std::function<void(int)>& onfinished)
{
    if (is_collected || is_collecting)
    {
        return;
    }

    this->is_collecting = true;

    // Play specific audio feedback based on coin value
    if (coin_type == CoinType::DIAMOND)
    {
        cocos2d::AudioEngine::play2d("diamond.mp3", false);
    }
    else
    {
        cocos2d::AudioEngine::play2d("coin.mp3", false);
    }

    // Movement animation towards the UI currency display (50, 20)
    auto move = MoveTo::create(0.8f, Vec2(50, 20));
    auto fadeOut = FadeOut::create(0.2f);

    // Callback to update the player's total currency
    auto addCoinValue = CallFunc::create([this, onfinished]() {
        if (onfinished) {
            onfinished(COIN_VALUE[static_cast<int>(coin_type)]);
        }
        });

    // Cleanup logic to mark object for removal
    auto markCollected = CallFunc::create([this]() {
        this->is_collected = true;
        this->is_collecting = false;
        });

    auto sequence = Sequence::create(move, fadeOut, addCoinValue, markCollected, nullptr);
    this->runAction(sequence);

    CCLOG("Coin collection sequence started. Value: %d", COIN_VALUE[static_cast<int>(coin_type)]);
}

// Condition for GameWorld to remove this object from the container
bool Coin::shouldRemove() const
{
    return is_collected || (life_time >= LIFETIME);
}

// Creates the "3D" spinning effect by scaling the X-axis
void Coin::setAnimation()
{
    // Diamonds do not use the standard spinning animation
    if (coin_type == CoinType::DIAMOND)
        return;

    float duration = 0.5f;

    auto spin = RepeatForever::create(
        Sequence::create(
            EaseInOut::create(ScaleTo::create(duration, -1.0f, 1.0f), 2.0f),
            EaseInOut::create(ScaleTo::create(duration, 1.0f, 1.0f), 2.0f),
            nullptr
        )
    );

    this->runAction(spin);
}

// Static factory method
Coin* Coin::create(CoinType coinType)
{
    Coin* coin = new (std::nothrow) Coin();
    if (coin && coin->init(coinType))
    {
        coin->autorelease();
        return coin;
    }
    CC_SAFE_DELETE(coin);
    return nullptr;
}