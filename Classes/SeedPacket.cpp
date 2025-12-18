#include "SeedPacket.h"
#include "Plant.h"

USING_NS_CC;

// Protected constructor
SeedPacket::SeedPacket()
    : _cooldownTime(7.5f)
    , _accumulatedTime(0.0f)
    , _sunCost(100)
    , _isOnCooldown(false)
{
    CCLOG("SeedPacket created.");
}

// Destructor
SeedPacket::~SeedPacket()
{
    CCLOG("SeedPacket destroyed.");
}

// Initialization
bool SeedPacket::init()
{
    // Don't call GameObject::init() because it tries to load empty IMAGE_FILENAME
    // Call Sprite::init() directly
    if (!Sprite::init())
    {
        return false;
    }

    // Load image (subclass should set _seedPacketImage before calling this)
    if (!_seedPacketImage.empty())
    {
        if (!Sprite::initWithFile(_seedPacketImage))
        {
            CCLOG("Failed to load seed packet image: %s", _seedPacketImage.c_str());
            return false;
        }
    }
    else
    {
        CCLOG("Warning: SeedPacket _seedPacketImage is empty!");
    }

    _isOnCooldown = false;
    _accumulatedTime = 0.0f;

    // Enable update for cooldown animation
    this->scheduleUpdate();

    CCLOG("SeedPacket initialized with image: %s", _seedPacketImage.c_str());
    return true;
}

// Update function
void SeedPacket::update(float delta)
{
    if (_isOnCooldown)
    {
        _accumulatedTime += delta;

        if (_accumulatedTime >= _cooldownTime)
        {
            // Cooldown finished - restore to full brightness
            _isOnCooldown = false;
            _accumulatedTime = 0.0f;
            this->setColor(Color3B::WHITE);
            CCLOG("Seed packet cooldown finished!");
        }
        else
        {
            // Update cooldown visual effect
            updateCooldownEffect();
        }
    }
    else
    {
        // Ensure full brightness when not on cooldown
        this->setColor(Color3B::WHITE);
    }
}

// Check if ready
bool SeedPacket::isReady() const
{
    return !_isOnCooldown;
}

// Start cooldown
void SeedPacket::startCooldown()
{
    _isOnCooldown = true;
    _accumulatedTime = 0.0f;
}

// Get sun cost
int SeedPacket::getSunCost() const
{
    return _sunCost;
}

// Update cooldown visual effect
void SeedPacket::updateCooldownEffect()
{
    // Calculate progress: 0.0 (just started) to 1.0 (finished)
    float progress = _accumulatedTime / _cooldownTime;

    // Interpolate from very dark (30, 30, 30) to 50% brightness (128, 128, 128)
    // This makes cooldown very obvious - card stays dim until fully ready
    const GLubyte MIN_BRIGHTNESS = 30;   // Very dark when cooling
    const GLubyte MAX_BRIGHTNESS = 128;  // Only 50% of 255 during cooldown
    
    GLubyte value = static_cast<GLubyte>(MIN_BRIGHTNESS + (MAX_BRIGHTNESS - MIN_BRIGHTNESS) * progress);

    this->setColor(Color3B(value, value, value));
}

PlantName SeedPacket::getPlantName()
{
    return this->_plantName;
}