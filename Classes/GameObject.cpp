#include "GameObject.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions (base class defaults)
// Note: Subclasses can redefine these in their own .cpp files
// ----------------------------------------------------
const std::string GameObject::IMAGE_FILENAME = "";
const Rect GameObject::INITIAL_PIC_RECT = Rect::ZERO;
const Size GameObject::OBJECT_SIZE = Size::ZERO;

// Protected constructor implementation
GameObject::GameObject()
{
    CCLOG("GameObject created.");
    // Initialize any GameObject-specific state here
}

// Destructor implementation
GameObject::~GameObject()
{
    CCLOG("GameObject destroyed.");
}

// Initialization function implementation
bool GameObject::init()
{
    // 1. Call parent class initialization
    // If IMAGE_FILENAME is empty, use Sprite::init()
    if (IMAGE_FILENAME.empty())
    {
        if (!Sprite::init())
        {
            return false;
        }
    }
    else
    {
        if (!Sprite::initWithFile(IMAGE_FILENAME, INITIAL_PIC_RECT))
        {
            return false;
        }
    }
    return true;
}
