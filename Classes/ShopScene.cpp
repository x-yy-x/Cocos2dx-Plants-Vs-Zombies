#include "ShopScene.h"
#include "GameMenu.h"
#include "PlayerProfile.h"
#include "ui/CocosGUI.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ================= 参数（可调） =================
static const float SHELF_1_Y_RATIO = 0.80f;
static const float SHELF_2_Y_RATIO = 0.40f;
static const float ITEM_START_X_RATIO = 0.40f;
static const float ITEM_SPACING_X_RATIO = 0.20f;

static const int PRICE_MOWER = 200;
static const int PRICE_RAKE = 200;
static const int PRICE_TWIN_SUNFLOWER = 2000;
static const int PRICE_GATLING_PEA = 5000;
static const int PRICE_SPIKE_ROCK = 3000;

static const float DAVE_FRAME_DELAY = 0.10f;
static int DAVE_IDLE_LOOPS = 5;

Scene* ShopScene::createScene()
{
    return ShopScene::create();
}

void ShopScene::onBackToMenu(cocos2d::Ref* sender)
{
    cocos2d::AudioEngine::play2d("buttonclick.mp3");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameMenu::createScene()));
}

bool ShopScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    setupUI();
    setupDaveAnimation();
    setupShopItems();

    return true;
}

void ShopScene::setupUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 背景
    auto background = Sprite::create("shop_background.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        float scaleX = visibleSize.width / background->getContentSize().width;
        float scaleY = visibleSize.height / background->getContentSize().height;
        background->setScale(MAX(scaleX, scaleY));
        this->addChild(background, 0);
    }

    // 返回按钮
    auto backItem = MenuItemImage::create("btn_Menu.png", "btn_Menu2.png", CC_CALLBACK_1(ShopScene::onBackToMenu, this));
    if (backItem) {
        backItem->setScale(0.8f);
        backItem->setPosition(Vec2(visibleSize.width - 100.0f + origin.x, visibleSize.height - 40.0f + origin.y));
        auto backMenu = Menu::create(backItem, nullptr);
        backMenu->setPosition(Vec2::ZERO);
        this->addChild(backMenu, 100);
    }

    // 金币显示
    Sprite* coinBank = Sprite::create("CoinBank.png");
    Vec2 coinPos(100 + origin.x, visibleSize.height - 40.0f + origin.y);
    if (coinBank)
    {
        coinBank->setPosition(coinPos);
        this->addChild(coinBank, 99);
    }

    _coinLabel = Label::createWithSystemFont("", "Arial", 20);
    if (_coinLabel) {
        _coinLabel->setColor(Color3B::BLACK);
        _coinLabel->setPosition(coinPos);
        this->addChild(_coinLabel, 100);
        updateCoinLabel();
    }
}

void ShopScene::setupDaveAnimation()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _daveSprite = Sprite::create("dave/idling/1 (1).png");
    if (!_daveSprite) return;

    _daveSprite->setPosition(Vec2(150 + origin.x, 150 + origin.y));
    this->addChild(_daveSprite, 1);

    auto idleAnimation = Animation::create();
    for (int i = 1; i <= 17; ++i)
    {
        idleAnimation->addSpriteFrameWithFile(StringUtils::format("dave/idling/1 (%d).png", i));
    }
    idleAnimation->setDelayPerUnit(DAVE_FRAME_DELAY);

    auto speakingAnimation = Animation::create();
    for (int i = 1; i <= 13; ++i)
    {
        speakingAnimation->addSpriteFrameWithFile(StringUtils::format("dave/speaking/1 (%d).png", i));
    }
    speakingAnimation->setDelayPerUnit(DAVE_FRAME_DELAY);

    auto idleOnce = Animate::create(idleAnimation);
    auto idleRepeat = Repeat::create(idleOnce, DAVE_IDLE_LOOPS);
    auto speakOnce = Animate::create(speakingAnimation);
    auto cycle = Sequence::create(idleRepeat, speakOnce, nullptr);
    _daveSprite->runAction(RepeatForever::create(cycle));
}

void ShopScene::setupShopItems()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    float shelf1_y = visibleSize.height * SHELF_1_Y_RATIO;
    float shelf2_y = visibleSize.height * SHELF_2_Y_RATIO;
    float start_x = visibleSize.width * ITEM_START_X_RATIO;
    float spacing_x = visibleSize.width * ITEM_SPACING_X_RATIO;

    createShopItem("mower.png", "Mower", PRICE_MOWER, Vec2(start_x, shelf1_y));
    createShopItem("rake.png", "Rake", PRICE_RAKE, Vec2(start_x + spacing_x, shelf1_y));

    createShopItem("seedpacket_twinsunflower.png", "Twin Sunflower", PRICE_TWIN_SUNFLOWER, Vec2(start_x, shelf2_y), PlantName::TWINSUNFLOWER);
    createShopItem("seedpacket_gatlingpea.png", "Gatling Pea", PRICE_GATLING_PEA, Vec2(start_x + spacing_x, shelf2_y), PlantName::GATLINGPEA);
    createShopItem("seedpacket_spikerock.png", "Spike Rock", PRICE_SPIKE_ROCK, Vec2(start_x + spacing_x * 2, shelf2_y), PlantName::SPIKEROCK);
}

void ShopScene::createShopItem(const std::string& image, const std::string& name, int price, const Vec2& position, PlantName plantName)
{
    auto itemSprite = Sprite::create(image);
    if (!itemSprite) return;

    itemSprite->setPosition(position);
    this->addChild(itemSprite, 10);

    // 价格标签
    auto priceLabel = Label::createWithSystemFont("$ " + std::to_string(price), "Arial", 24);
    priceLabel->setPosition(Vec2(position.x, position.y - itemSprite->getContentSize().height / 2 - 20));
    this->addChild(priceLabel, 11);

    // SOLD OUT 标签
    auto soldOutLabel = Label::createWithSystemFont("SOLD OUT", "Arial", 28);
    soldOutLabel->setColor(Color3B::RED);
    soldOutLabel->setPosition(itemSprite->getPosition());
    soldOutLabel->setVisible(false);
    this->addChild(soldOutLabel, 12);

    // 购买逻辑
    bool isPlant = (plantName != PlantName::UNKNOWN);
    bool isRakeItem = (name == std::string("Rake"));
    bool isMowerItem = (name == std::string("Mower"));
    bool isSold = false;
    if (isPlant) {
        isSold = PlayerProfile::getInstance()->isPlantUnlocked(plantName);
    } else if (isRakeItem) {
        isSold = PlayerProfile::getInstance()->isRakeEnabled();
    } else if (isMowerItem) {
        isSold = PlayerProfile::getInstance()->isMowerEnabled();
    }

    if (isSold)
    {
        soldOutLabel->setVisible(true);
    }
    else
    {
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [=](Touch* touch, Event* event) mutable {
            auto target = static_cast<Node*>(event->getCurrentTarget());
            // 命中区域：商品图本身 + 价格文字区域（提高易点性）
            Rect hitRect = itemSprite->getBoundingBox();
            if (priceLabel) {
                Rect priceRect = priceLabel->getBoundingBox();
                hitRect.merge(priceRect);
            }
            Vec2 pInParent = target->getParent()->convertToNodeSpace(touch->getLocation());
            if (!hitRect.containsPoint(pInParent)) return false;

            if (isPlant && PlayerProfile::getInstance()->isPlantUnlocked(plantName))
                return false;

            if (PlayerProfile::getInstance()->spendCoins(price))
            {
                cocos2d::AudioEngine::play2d("buttonclick.mp3");
                if (isPlant) {
                    PlayerProfile::getInstance()->unlockPlant(plantName);
                } else if (isRakeItem) {
                    PlayerProfile::getInstance()->enableRake(true);
                } else if (isMowerItem) {
                    PlayerProfile::getInstance()->enableMower(true);
                }
                soldOutLabel->setVisible(true);
                updateCoinLabel();
                listener->setEnabled(false);
            }
            else
            {
                cocos2d::AudioEngine::play2d("buzzer.mp3");
            }
            return true;
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, itemSprite);
    }
}

void ShopScene::updateCoinLabel()
{
    if (_coinLabel)
    {
        _coinLabel->setString(std::to_string(PlayerProfile::getInstance()->getCoins()));
    }
}