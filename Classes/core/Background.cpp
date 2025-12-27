#include "BackGround.h"
#include "GameDefs.h"
#include "cocos2d.h"

USING_NS_CC;

// ----------------------------------------------------
// Static constant definitions
// ----------------------------------------------------
const std::string BackGround::DAY_IMAGE = "background.png";
const std::string BackGround::NIGHT_IMAGE = "background2.png";

/**
 * @brief Anonymous namespace for internal UI layout constants.
 */
namespace
{
    const float SEED_BANK_POS_X_RATIO = 0.245f;
    const float SEED_BANK_POS_Y_RATIO = 0.96f;
    const float SEED_BANK_SCALE_FACTOR = 1.04f;
    const std::string SEED_BANK_IMAGE = "seedBank.png";
}

/**
 * @brief Constructor implementation using member initializer list.
 */
BackGround::BackGround()
    : is_night_mode(false)
{
    // Constructor body
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
        bg->is_night_mode = isNightMode;
        if (bg->init())
        {
            bg->autorelease();
            return bg;
        }
        delete bg;
    }
    return nullptr;
}

/**
 * @brief Initialization logic.
 * Configures the background texture, scaling, and the seed bank UI component.
 */
bool BackGround::init()
{
    // 1. Select and initialize the main background texture
    const std::string& imageFile = is_night_mode ? NIGHT_IMAGE : DAY_IMAGE;
    if (!Sprite::initWithFile(imageFile))
    {
        CCLOG("Failed to load background image: %s", imageFile.c_str());
        return false;
    }

    // 2. Get visible area and origin for layout calculations
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 3. Center the background on the screen
    this->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    // 4. Calculate scaling factors to ensure the background fills the entire screen
    auto bgSize = this->getContentSize();
    float scaleX = visibleSize.width / bgSize.width;
    float scaleY = visibleSize.height / bgSize.height;
    this->setScale(scaleX, scaleY);

    // 5. Initialize and position the SeedBank Sprite
    auto seedBankSprite = Sprite::create(SEED_BANK_IMAGE);

    if (seedBankSprite)
    {
        // Calculate position based on screen ratios
        float finalX = visibleSize.width * SEED_BANK_POS_X_RATIO + origin.x;
        float finalY = visibleSize.height * SEED_BANK_POS_Y_RATIO + origin.y;

        seedBankSprite->setPosition(Vec2(finalX, finalY));
        seedBankSprite->setScale(SEED_BANK_SCALE_FACTOR);

        // Add as child with higher Z-order to stay on top of the background
        this->addChild(seedBankSprite, 1);

        CCLOG("SeedBank Sprite initialized successfully.");
    }
    else
    {
        CCLOG("Error: Failed to load seed bank image: %s", SEED_BANK_IMAGE.c_str());
    }

    CCLOG("BackGround initialized successfully.");
    return true;
}