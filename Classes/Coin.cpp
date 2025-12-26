#include "Coin.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Coin::IMAGE_FILENAME[3] = { "coin_silver_dollar.png","coin_gold_dollar.png","Diamond.png" };
const int Coin::COIN_VALUE[3] = { 10,50,1000 };
const float Coin::LIFETIME = 10.0f;

// Constructor
Coin::Coin()
    : _isCollected(false)
    , _isCollecting(false)
    , _lifeTime(0.0f)
    , _targetPos(Vec2::ZERO)
    , _coinScale(1.0f)
    , _coinType(CoinType::SILVER)
{
    CCLOG("Coin created.");
}

// Destructor
Coin::~Coin()
{
    CCLOG("Coin destroyed.");
}

// Initialization
bool Coin::init(CoinType coinType)
{
    this->_coinType = coinType;
    if (!GameObject::init())
    {
        return false;
    }

    if (!Sprite::initWithFile(IMAGE_FILENAME[_coinType]))
    {
        CCLOG("Failed to load coin image: %s", IMAGE_FILENAME[_coinType].c_str());
        return false;
    }

    // Apply custom scale
    this->setScale(_coinScale);

    _isCollected = false;
    _lifeTime = 0.0f;

    // Set up animation (placeholder for now)
    setAnimation();

    // Enable update
    this->scheduleUpdate();

    return true;
}

// Update function
void Coin::update(float delta)
{
    if (_isCollected || _isCollecting)
    {
        return;
    }

    _lifeTime += delta;
}

// Check if collectible
bool Coin::isCollectible() const
{
    return !_isCollected;
}

// Collect sun
void Coin::collect(const std::function<void(int)>& onfinished)
{
    if (_isCollected)
    {
        return;
    }
    this->_isCollecting = true;
	if (_coinType == CoinType::DIAMOND)
	{
		cocos2d::AudioEngine::play2d("diamond.mp3", false);
	}
	else
	{
		cocos2d::AudioEngine::play2d("coin.mp3", false);
	}
    // Fade out and remove
    auto move = MoveTo::create(0.8f, Vec2(50, 20));
    auto fadeOut = FadeOut::create(0.2f);
    auto addCoinValue = CallFunc::create([this, onfinished]() {
        onfinished(COIN_VALUE[_coinType]);
        });
    auto markCollected = CallFunc::create([this]() {
        this->_isCollected = true;
        this->_isCollecting = false;
        });
    auto sequence = Sequence::create(move, fadeOut, addCoinValue, markCollected, nullptr);
    this->runAction(sequence);

    CCLOG("Coin collected! Value: %d", COIN_VALUE[_coinType]);
    return;
}

// Check if should be removed
bool Coin::shouldRemove() const
{
    return _isCollected || (_lifeTime >= LIFETIME);
}

// Set animation (placeholder)
void Coin::setAnimation()
{
    if (_coinType == CoinType::DIAMOND)
        return;
    float time = 0.5f;

    auto spin = RepeatForever::create(
        Sequence::create(
            EaseInOut::create(ScaleTo::create(time, -1.0f, 1.0f), 2.0f),
            EaseInOut::create(ScaleTo::create(time, 1.0f, 1.0f), 2.0f),
            nullptr
        )
    );

    this->runAction(spin);
}

Coin* Coin::create(CoinType CoinType)
{
    Coin* coin = new (std::nothrow) Coin();
    if (coin && coin->init(CoinType))
    {
        coin->autorelease();
        return coin;
    }
    CC_SAFE_DELETE(coin);
    return nullptr;
}