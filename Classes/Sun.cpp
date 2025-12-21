#include "Sun.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Sun::IMAGE_FILENAME = "sun_spritesheet.png";
const int Sun::SUN_VALUE = 25;
const float Sun::FALL_SPEED = 50.0f;  // pixels per second
const float Sun::LIFETIME = 15.0f;    // 15 seconds

// Constructor
Sun::Sun()
    : _isCollected(false)
    , _isFalling(false)
    , _lifeTime(0.0f)
    , _targetPos(Vec2::ZERO)
    , _sunScale(1.0f)
    , _sunValue(SUN_VALUE)
{
    CCLOG("Sun created.");
}

// Destructor
Sun::~Sun()
{
    CCLOG("Sun destroyed.");
}

// Initialization
bool Sun::init()
{
    if (!GameObject::init())
    {
        return false;
    }

    // Load sun image
    if (!Sprite::initWithFile(IMAGE_FILENAME, Rect(0, 0, 100, 100)))
    {
        CCLOG("Failed to load sun image: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    // Apply custom scale
    this->setScale(_sunScale);

    _isCollected = false;
    _lifeTime = 0.0f;

    // Set up animation (placeholder for now)
    setAnimation();

    // Enable update
    this->scheduleUpdate();

    return true;
}

// Create sun from sunflower
Sun* Sun::createFromSunflower(const Vec2& pos)
{
    Sun* sun = Sun::create();
    if (sun)
    {
        sun->setPosition(pos);
        sun->_isFalling = false;
    }
    return sun;
}

// Create custom sun with specific size and value
Sun* Sun::createCustomSun(const Vec2& pos, float scale, int value)
{
    Sun* sun = Sun::create();
    if (sun)
    {
        sun->setPosition(pos);
        sun->_isFalling = false;
        sun->_sunScale = scale;
        sun->_sunValue = value;
        sun->setScale(scale);  // Apply scale immediately
    }
    return sun;
}

// Create sun from sky
Sun* Sun::createFromSky(int targetGridCol, float startY)
{
    Sun* sun = Sun::create();
    if (sun)
    {
        // Calculate target position (grid center)
        float targetX = GRID_ORIGIN.x + targetGridCol * CELLSIZE.width + CELLSIZE.width * 0.5f;
        int randomRow = rand() % MAX_ROW;
        float targetY = GRID_ORIGIN.y + randomRow * CELLSIZE.height + CELLSIZE.height * 0.5f;

        sun->_targetPos = Vec2(targetX, targetY);
        sun->setPosition(Vec2(targetX, startY)); // Start from top
        sun->_isFalling = true;
    }
    return sun;
}

// Update function
void Sun::update(float delta)
{
    if (_isCollected)
    {
        return;
    }

    // Handle falling from sky
    if (_isFalling)
    {
        Vec2 currentPos = this->getPosition();
        
        // Fall until reaching target Y
        if (currentPos.y > _targetPos.y)
        {
            float newY = currentPos.y - FALL_SPEED * delta;
            if (newY < _targetPos.y)
            {
                newY = _targetPos.y;
                _isFalling = false; // Stop falling
            }
            this->setPositionY(newY);
        }
        return;
    }

    _lifeTime += delta;
}

// Check if collectible
bool Sun::isCollectible() const
{
    return !_isCollected;
}

// Collect sun
int Sun::collect()
{
    if (_isCollected)
    {
        return 0;
    }

    _isCollected = true;
    
    // Fade out and remove
    auto fadeOut = FadeOut::create(0.2f);
    auto remove = RemoveSelf::create();
    auto sequence = Sequence::create(fadeOut, remove, nullptr);
    this->runAction(sequence);

    CCLOG("Sun collected! Value: %d", _sunValue);
    return _sunValue;
}

// Check if should be removed
bool Sun::shouldRemove() const
{
    return _isCollected || (_lifeTime >= LIFETIME);
}

// Set animation (placeholder)
void Sun::setAnimation()
{
    // TODO: Implement sun animation here
    // For now, just use static image
    const float frameWidth = 100;
    const float frameHeight = 100;

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                IMAGE_FILENAME,
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.07f);
    auto animate = Animate::create(animation);

    this->runAction(RepeatForever::create(animate));


    CCLOG("Sun::setAnimation() - not implemented yet");
}
