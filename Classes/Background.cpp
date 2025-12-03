#include "BackGround.h"
#include "GameDefs.h"

USING_NS_CC;

// ----------------------------------------------------
// BackGround class static constant definitions
// ----------------------------------------------------
const std::string BackGround::IMAGE_FILENAME = "background.png";
const Rect BackGround::INITIAL_PIC_RECT = Rect::ZERO;
const Size BackGround::OBJECT_SIZE = Size::ZERO;

// Initialization function implementation
bool BackGround::init()
{
    // 1. Initialize Sprite with BackGround's IMAGE_FILENAME
    if (!Sprite::initWithFile(IMAGE_FILENAME))
    {
        CCLOG("Failed to load background image: %s", IMAGE_FILENAME.c_str());
        return false;
    }

    // 2. Get visible area and origin
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 3. Set background position to screen center
    this->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    // 4. Get background image size and set scaling to fill screen
    auto bgSize = this->getContentSize();
    float scaleX = visibleSize.width / bgSize.width;
    float scaleY = visibleSize.height / bgSize.height;
    this->setScale(scaleX, scaleY);

    CCLOG("BackGround initialized successfully.");
    return true;
}
