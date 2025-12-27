#include "Shovel.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string Shovel::IMAGE_FILENAME = "Shovel.png";
const cocos2d::Rect Shovel::INITIAL_PIC_RECT = Rect::ZERO;
const cocos2d::Size Shovel::OBJECT_SIZE = Size(80, 80);

// Constructor
Shovel::Shovel()
    : is_dragging(false)
    , original_position(Vec2::ZERO)
{
    CCLOG("Shovel instance created.");
}

// Destructor
Shovel::~Shovel()
{
    CCLOG("Shovel instance destroyed.");
}

// Initialization logic
bool Shovel::init()
{
    if (!GameObject::init())
    {
        return false;
    }

    // Load the shovel texture
    if (!Sprite::initWithFile(IMAGE_FILENAME))
    {
        CCLOG("Critical Error: Failed to load shovel image: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    /** * Setting the anchor point to the left center (0.0, 0.5).
     * This ensures the tip of the shovel acts as the "hotspot" for collision detection
     * and follows the cursor accurately.
     */
    this->setAnchorPoint(Vec2(0.0f, 0.5f));

    is_dragging = false;

    CCLOG("Shovel successfully initialized.");
    return true;
}

// Hit-test check
bool Shovel::containsPoint(const Vec2& point)
{
    return this->getBoundingBox().containsPoint(point);
}

// Dragging state visual feedback
void Shovel::setDragging(bool enabled)
{
    is_dragging = enabled;

    if (enabled)
    {
        // Enlarge and make slightly translucent to signify it is "picked up"
        this->setScale(1.2f);
        this->setOpacity(200);
    }
    else
    {
        // Restore standard scale and full opacity
        this->setScale(1.0f);
        this->setOpacity(255);
    }
}

bool Shovel::isDragging() const
{
    return is_dragging;
}

void Shovel::updatePosition(const Vec2& touchPos)
{
    if (is_dragging)
    {
        this->setPosition(touchPos);
    }
}

void Shovel::resetPosition()
{
    this->setPosition(original_position);
    setDragging(false);
}

void Shovel::setOriginalPosition(const Vec2& pos)
{
    /** * Since the anchor point is at the left (0, 0.5), we adjust the position
     * by half its width to ensure the shovel body is centered within its UI container.
     */
    float shovelWidth = this->getContentSize().width;
    Vec2 adjustedPos = Vec2(pos.x - (shovelWidth * 0.5f), pos.y);

    original_position = adjustedPos;
    this->setPosition(adjustedPos);
}