#include "ShopScene.h"
#include "GameMenu.h"
#include "PlayerProfile.h"
#include "ui/CocosGUI.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// --- Layout and Pricing Parameters ---
static const float SHELF_1_Y_RATIO = 0.80f;     // Vertical position for tools
static const float SHELF_2_Y_RATIO = 0.40f;     // Vertical position for plants
static const float ITEM_START_X_RATIO = 0.40f;  // Starting horizontal offset
static const float ITEM_SPACING_X_RATIO = 0.20f; // Gap between items

static const int PRICE_MOWER = 200;
static const int PRICE_RAKE = 200;
static const int PRICE_TWIN_SUNFLOWER = 2000;
static const int PRICE_GATLING_PEA = 5000;
static const int PRICE_SPIKE_ROCK = 3000;

static const float DAVE_FRAME_DELAY = 0.10f;    // Animation speed
static int DAVE_IDLE_LOOPS = 3;                 // Number of idle cycles before speaking

Scene* ShopScene::createScene()
{
    return ShopScene::create();
}

bool ShopScene::init()
{
    if (!Scene::init()) return false;

    AudioEngine::play2d("title.mp3", true);
    setupUI();
    setupDaveAnimation();
    setupShopItems();

    return true;
}

void ShopScene::onBackToMenu(cocos2d::Ref* sender)
{
    AudioEngine::play2d("buttonclick.mp3");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameMenu::createScene()));
}

void ShopScene::setupUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. Scene Background
    auto background = Sprite::create("shop_background.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        float scaleX = visibleSize.width / background->getContentSize().width;
        float scaleY = visibleSize.height / background->getContentSize().height;
        background->setScale(MAX(scaleX, scaleY));
        this->addChild(background, 0);
    }

    // 2. Navigation Menu
    auto backItem = MenuItemImage::create("btn_Menu.png", "btn_Menu2.png", CC_CALLBACK_1(ShopScene::onBackToMenu, this));
    if (backItem) {
        backItem->setScale(0.8f);
        backItem->setPosition(Vec2(visibleSize.width - 100.0f + origin.x, visibleSize.height - 40.0f + origin.y));
        auto backMenu = Menu::create(backItem, nullptr);
        backMenu->setPosition(Vec2::ZERO);
        this->addChild(backMenu, 100);
    }

    // 3. Currency Display (Coin Bank)
    Sprite* coinBank = Sprite::create("CoinBank.png");
    Vec2 coinPos(100 + origin.x, visibleSize.height - 40.0f + origin.y);
    if (coinBank)
    {
        coinBank->setPosition(coinPos);
        this->addChild(coinBank, 99);
    }

    coin_label = Label::createWithSystemFont("", "Arial", 20);
    if (coin_label) {
        coin_label->setColor(Color3B::BLACK);
        coin_label->setPosition(coinPos);
        this->addChild(coin_label, 100);
        updateCoinLabel();
    }
}

void ShopScene::setupDaveAnimation()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    dave_sprite = Sprite::create("dave/idling/1 (1).png");
    if (!dave_sprite) return;

    dave_sprite->setPosition(Vec2(150 + origin.x, 150 + origin.y));
    this->addChild(dave_sprite, 1);

    // Create Idle Animation (Breathing/Looking around)
    auto idleAnimation = Animation::create();
    for (int i = 1; i <= 17; ++i)
        idleAnimation->addSpriteFrameWithFile(StringUtils::format("dave/idling/1 (%d).png", i));
    idleAnimation->setDelayPerUnit(DAVE_FRAME_DELAY);

    // Create Speaking Animation (Mouth movements)
    auto speakingAnimation = Animation::create();
    for (int i = 1; i <= 13; ++i)
        speakingAnimation->addSpriteFrameWithFile(StringUtils::format("dave/speaking/1 (%d).png", i));
    speakingAnimation->setDelayPerUnit(DAVE_FRAME_DELAY);

    // Randomized Dave "gibberish" voice logic
    auto playRandomDaveSound = CallFunc::create([]() {
        int randNum = cocos2d::random(1, 4);
        AudioEngine::play2d(StringUtils::format("dave%d.mp3", randNum));
        });

    // Dave behavior loop: Idle -> Speak -> Play Audio -> Repeat
    auto idleRepeat = Repeat::create(Animate::create(idleAnimation), DAVE_IDLE_LOOPS);
    auto cycle = Sequence::create(idleRepeat, playRandomDaveSound, Animate::create(speakingAnimation), nullptr);
    dave_sprite->runAction(RepeatForever::create(cycle));
}

void ShopScene::setupShopItems()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    float shelf1_y = visibleSize.height * SHELF_1_Y_RATIO;
    float shelf2_y = visibleSize.height * SHELF_2_Y_RATIO;
    float start_x = visibleSize.width * ITEM_START_X_RATIO;
    float spacing_x = visibleSize.width * ITEM_SPACING_X_RATIO;

    // Shelf 1: Utility items/Tools
    createShopItem("mower.png", ShopItemID::MOWER, PRICE_MOWER, Vec2(start_x, shelf1_y));
    createShopItem("rake.png", ShopItemID::RAKE, PRICE_RAKE, Vec2(start_x + spacing_x, shelf1_y));

    // Shelf 2: Advanced Plant Seed Packets
    createShopItem("seedpacket_twinsunflower.png", ShopItemID::TWIN_SUNFLOWER, PRICE_TWIN_SUNFLOWER, Vec2(start_x, shelf2_y), PlantName::TWINSUNFLOWER);
    createShopItem("seedpacket_gatlingpea.png", ShopItemID::GATLING_PEA, PRICE_GATLING_PEA, Vec2(start_x + spacing_x, shelf2_y), PlantName::GATLINGPEA);
    createShopItem("seedpacket_spikerock.png", ShopItemID::SPIKE_ROCK, PRICE_SPIKE_ROCK, Vec2(start_x + spacing_x * 2, shelf2_y), PlantName::SPIKEROCK);
}

void ShopScene::createShopItem(const std::string& image, ShopItemID itemId, int price, const Vec2& position, PlantName plantName)
{
    auto itemSprite = Sprite::create(image);
    if (!itemSprite) return;

    itemSprite->setPosition(position);
    this->addChild(itemSprite, 10);

    auto priceLabel = Label::createWithSystemFont("$ " + std::to_string(price), "Arial", 24);
    priceLabel->setPosition(Vec2(position.x, position.y - itemSprite->getContentSize().height / 2 - 20));
    this->addChild(priceLabel, 11);

    auto soldOutLabel = Label::createWithSystemFont("SOLD OUT", "Arial", 28);
    soldOutLabel->setColor(Color3B::RED);
    soldOutLabel->setPosition(itemSprite->getPosition());
    soldOutLabel->setVisible(false);
    this->addChild(soldOutLabel, 12);

    auto profile = PlayerProfile::getInstance();

    // Purchase validation logic: check if the item is already permanently unlocked
    auto checkOwned = [=]() -> bool {
        switch (itemId) {
            case ShopItemID::MOWER:          return profile->isMowerEnabled();
            case ShopItemID::RAKE:           return profile->isRakeEnabled();
            case ShopItemID::TWIN_SUNFLOWER:
            case ShopItemID::GATLING_PEA:
            case ShopItemID::SPIKE_ROCK:     return profile->isPlantUnlocked(plantName);
            default: return false;
        }
        };

    if (checkOwned()) {
        soldOutLabel->setVisible(true);
    }
    else {
        // Handle touch interactions for purchasing
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [=](Touch* touch, Event* event) mutable {
            auto target = static_cast<Node*>(event->getCurrentTarget());
            Rect hitRect = target->getBoundingBox();

            // Expand hit area to include the price tag
            if (priceLabel) {
                Rect priceRect = priceLabel->getBoundingBox();
                hitRect.merge(priceRect);
            }

            Vec2 pInParent = target->getParent()->convertToNodeSpace(touch->getLocation());
            if (!hitRect.containsPoint(pInParent)) return false;

            // Prevent double-buying
            if (checkOwned()) {
                AudioEngine::play2d("buzzer.mp3");
                return false;
            }

            // Transaction handling
            if (profile->spendCoins(price)) {
                AudioEngine::play2d("buttonclick.mp3");

                // Update persistent player data based on item type
                switch (itemId) {
                    case ShopItemID::MOWER:          profile->enableMower(true); break;
                    case ShopItemID::RAKE:           profile->enableRake(true); break;
                    case ShopItemID::TWIN_SUNFLOWER:
                    case ShopItemID::GATLING_PEA:
                    case ShopItemID::SPIKE_ROCK:     profile->unlockPlant(plantName); break;
                    default: break;
                }

                soldOutLabel->setVisible(true);
                this->updateCoinLabel();
                // Disable touch listener after item is sold out
                event->getCurrentTarget()->getEventDispatcher()->removeEventListener(listener);
            }
            else {
                // Insufficient funds
                AudioEngine::play2d("buzzer.mp3");
            }
            return true;
            };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, itemSprite);
    }
}

void ShopScene::updateCoinLabel()
{
    if (coin_label) {
        coin_label->setString(std::to_string(PlayerProfile::getInstance()->getCoins()));
    }
}