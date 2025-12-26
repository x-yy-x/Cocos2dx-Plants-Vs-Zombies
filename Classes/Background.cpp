#include "BackGround.h"
#include "GameDefs.h"
#include "cocos2d.h" // Ensure cocos2d header file is included

USING_NS_CC;

// =================================================================
// BackGround class static constant definitions
// =================================================================
const std::string BackGround::DAY_IMAGE = "background.png";
const std::string BackGround::NIGHT_IMAGE = "background2.png";

// =================================================================
// SeedBank Sprite configuration - do not modify
// =================================================================
namespace
{
    // X position offset, distance from origin, ratio of screen width

    const float SEED_BANK_POS_X_RATIO = 0.245f;

    // Y position offset, distance from origin, ratio of screen height (1.0f is top, 0.0f is bottom)

    const float SEED_BANK_POS_Y_RATIO = 0.96f;

    // Scaling factor

    const float SEED_BANK_SCALE_FACTOR = 1.04f;

    // Image filename

    const std::string SEED_BANK_IMAGE = "seedBank.png";
}


BackGround* BackGround::create()
{
    return create(false);
}

BackGround* BackGround::create(bool isNightMode)
{
    BackGround* bg = new (std::nothrow) BackGround();
    if (bg)
    {
        bg->_isNightMode = isNightMode;
        if (bg->init())
        {
            bg->autorelease();
            return bg;
        }
        delete bg;
    }
    return nullptr;
}

bool BackGround::init()
{
    const std::string& imageFile = _isNightMode ? NIGHT_IMAGE : DAY_IMAGE;
    if (!Sprite::initWithFile(imageFile))
    {
        CCLOG("Failed to load background image: %s", imageFile.c_str());
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

    // =================================================================
    // 5. Initialize SeedBank Sprite
    // =================================================================
    auto seedBankSprite = Sprite::create(SEED_BANK_IMAGE);

    if (seedBankSprite)
    {
        // Set SeedBank position
        // X: Screen width * ratio + origin X
        float finalX = visibleSize.width * SEED_BANK_POS_X_RATIO + origin.x;

        // Y: Screen height * ratio + origin Y
        float finalY = visibleSize.height * SEED_BANK_POS_Y_RATIO + origin.y;

        seedBankSprite->setPosition(Vec2(finalX, finalY));

        // Set scaling factor
        seedBankSprite->setScale(SEED_BANK_SCALE_FACTOR);

        // Z-order: Ensure it's above the background
        // Note: Add seedBankSprite as a child of BackGround (this)
        this->addChild(seedBankSprite, 1);

        CCLOG("SeedBank Sprite initialized successfully.");
    }
    else
    {
        CCLOG("Error: Failed to load seed bank image: %s", SEED_BANK_IMAGE.c_str());
    }
    // =================================================================

    CCLOG("BackGround initialized successfully.");
    return true;
}