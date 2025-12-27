#include "Sun.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Sun::IMAGE_FILENAME = "sun_spritesheet.png";
const int Sun::SUN_VALUE = 25;
const float Sun::FALL_SPEED = 50.0f;  // Pixels per second
const float Sun::LIFETIME = 15.0f;    // 15 seconds before fading

// ----------------------------------------------------
// Lifecycle Methods
// ----------------------------------------------------

Sun::Sun()
    : is_collected(false)
    , is_collecting(false)
    , is_falling(false)
    , life_time(0.0f)
    , target_pos(Vec2::ZERO)
    , sun_scale(1.0f)
    , sun_value(SUN_VALUE)
{
    CCLOG("Sun instance created.");
}

Sun::~Sun()
{
    CCLOG("Sun instance destroyed.");
}

bool Sun::init()
{
    if (!GameObject::init())
    {
        return false;
    }

    // Initialize with first frame of the spritesheet (100x100 pixels)
    if (!Sprite::initWithFile(IMAGE_FILENAME, Rect(0, 0, 100, 100)))
    {
        CCLOG("Error: Failed to load sun spritesheet: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    this->setScale(sun_scale);
    is_collected = false;
    life_time = 0.0f;

    // Start rotation animation and enable per-frame updates
    setAnimation();
    this->scheduleUpdate();

    return true;
}

// ----------------------------------------------------
// Factory Implementations
// ----------------------------------------------------

Sun* Sun::createFromSunflower(const Vec2& pos)
{
    Sun* sun = Sun::create();
    if (sun)
    {
        sun->setPosition(pos);
        sun->is_falling = false;
    }
    return sun;
}

Sun* Sun::createCustomSun(const Vec2& pos, float scale, int value)
{
    Sun* sun = Sun::create();
    if (sun)
    {
        sun->setPosition(pos);
        sun->is_falling = false;
        sun->sun_scale = scale;
        sun->sun_value = value;
        sun->setScale(scale);
    }
    return sun;
}

Sun* Sun::createFromSky(int targetGridCol, float startY)
{
    Sun* sun = Sun::create();
    if (sun)
    {
        // Randomly determine which row the sun will stop falling at
        float targetX = GRID_ORIGIN.x + targetGridCol * CELLSIZE.width + CELLSIZE.width * 0.5f;
        int randomRow = rand() % MAX_ROW;
        float targetY = GRID_ORIGIN.y + randomRow * CELLSIZE.height + CELLSIZE.height * 0.5f;

        sun->target_pos = Vec2(targetX, targetY);
        sun->setPosition(Vec2(targetX, startY));
        sun->is_falling = true;
    }
    return sun;
}

// ----------------------------------------------------
// Logic & Movement
// ----------------------------------------------------

void Sun::update(float delta)
{
    if (is_collected || is_collecting)
    {
        return;
    }

    if (is_falling)
    {
        Vec2 currentPos = this->getPosition();

        // Linear descent until reaching the target landing position
        if (currentPos.y > target_pos.y)
        {
            float newY = currentPos.y - (FALL_SPEED * delta);
            if (newY <= target_pos.y)
            {
                newY = target_pos.y;
                is_falling = false;
            }
            this->setPositionY(newY);
        }
        return;
    }

    // Expire if not collected within the lifetime window
    life_time += delta;
}

bool Sun::isCollectible() const
{
    return !is_collected && !is_collecting;
}

void Sun::collect(const std::function<void(int)>& onfinish)
{
    if (is_collected || is_collecting)
    {
        return;
    }

    this->is_falling = false;
    this->is_collecting = true;

    // Animation sequence: Fly to UI sun counter (95, 675) then disappear
    auto moveToBank = MoveTo::create(0.8f, Vec2(95, 675));
    auto fadeOut = FadeOut::create(0.2f);

    // Callback to update game resource logic
    auto addResource = CallFunc::create([this, onfinish]() {
        if (onfinish) onfinish(sun_value);
        });

    // Cleanup state after animation
    auto markDone = CallFunc::create([this]() {
        this->is_collected = true;
        this->is_collecting = false;
        });

    auto sequence = Sequence::create(moveToBank, fadeOut, addResource, markDone, nullptr);
    this->runAction(sequence);

    CCLOG("Sun collection triggered. Value: %d", sun_value);
}

bool Sun::shouldRemove() const
{
    return is_collected || (life_time >= LIFETIME);
}

void Sun::setAnimation()
{
    // Configure spritesheet animation (12 frames across 2 rows)
    const float frameWidth = 100;
    const float frameHeight = 100;
    const int rowCount = 2;
    const int colCount = 6;
    const int totalFrames = 12;
    const float frameDelay = 0.07f;

    auto animation = initAnimate(IMAGE_FILENAME, frameWidth, frameHeight, rowCount, colCount, totalFrames, frameDelay);
    if (animation) {
        auto animate = Animate::create(animation);
        this->runAction(RepeatForever::create(animate));
    }
}