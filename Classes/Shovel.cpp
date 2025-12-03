#include "Shovel.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Shovel::IMAGE_FILENAME = "Shovel.png";
const cocos2d::Rect Shovel::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Shovel::OBJECT_SIZE = Size(80, 80);

// Protected constructor
Shovel::Shovel()
    : _isDragging(false)
    , _originalPosition(Vec2::ZERO)
{
    CCLOG("Shovel created.");
}

// Destructor
Shovel::~Shovel()
{
    CCLOG("Shovel destroyed.");
}

// Initialization function
bool Shovel::init()
{
    // Call parent class initialization
    if (!GameObject::init())
    {
        return false;
    }

    // Initialize sprite with shovel image
    if (!Sprite::initWithFile(IMAGE_FILENAME))
    {
        CCLOG("Failed to load shovel image: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    // Set anchor point to left side (tip of the shovel)
    // This makes the tip of the shovel the reference point for removal
    this->setAnchorPoint(Vec2(0.0f, 0.5f));

    _isDragging = false;

    CCLOG("Shovel initialized successfully.");
    return true;
}

// Check if point is inside shovel
bool Shovel::containsPoint(const Vec2& point)
{
    return this->getBoundingBox().containsPoint(point);
}

// Set dragging state
void Shovel::setDragging(bool enabled)
{
    _isDragging = enabled;
    
    if (enabled)
    {
        // Make shovel larger/highlighted when dragging
        this->setScale(1.2f);
        this->setOpacity(200);
    }
    else
    {
        // Reset to normal size
        this->setScale(1.0f);
        this->setOpacity(255);
    }
}

// Check if dragging
bool Shovel::isDragging() const
{
    return _isDragging;
}

// Update position to follow touch
void Shovel::updatePosition(const Vec2& touchPos)
{
    if (_isDragging)
    {
        this->setPosition(touchPos);
    }
}

// Reset to original position
void Shovel::resetPosition()
{
    this->setPosition(_originalPosition);
    setDragging(false);
}

// Set original position
void Shovel::setOriginalPosition(const Vec2& pos)
{
    // Store the center position of shovel back
    // Since anchor point is at left (0, 0.5), we need to adjust
    // to make the whole shovel visually centered in the shovel back
    float shovelWidth = this->getContentSize().width;
    Vec2 adjustedPos = Vec2(pos.x - shovelWidth * 0.5f, pos.y);
    
    _originalPosition = adjustedPos;
    this->setPosition(adjustedPos);
}

