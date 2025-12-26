#include "ShopScene.h"
#include "GameMenu.h"
#include "PlayerProfile.h"
#include "ui/CocosGUI.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ================= 参数 =================
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
static int DAVE_IDLE_LOOPS = 3;

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

    auto background = Sprite::create("shop_background.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        float scaleX = visibleSize.width / background->getContentSize().width;
        float scaleY = visibleSize.height / background->getContentSize().height;
        background->setScale(MAX(scaleX, scaleY));
        this->addChild(background, 0);
    }

    auto backItem = MenuItemImage::create("btn_Menu.png", "btn_Menu2.png", CC_CALLBACK_1(ShopScene::onBackToMenu, this));
    if (backItem) {
        backItem->setScale(0.8f);
        backItem->setPosition(Vec2(visibleSize.width - 100.0f + origin.x, visibleSize.height - 40.0f + origin.y));
        auto backMenu = Menu::create(backItem, nullptr);
        backMenu->setPosition(Vec2::ZERO);
        this->addChild(backMenu, 100);
    }

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
        idleAnimation->addSpriteFrameWithFile(StringUtils::format("dave/idling/1 (%d).png", i));
    idleAnimation->setDelayPerUnit(DAVE_FRAME_DELAY);

    auto speakingAnimation = Animation::create();
    for (int i = 1; i <= 13; ++i)
        speakingAnimation->addSpriteFrameWithFile(StringUtils::format("dave/speaking/1 (%d).png", i));
    speakingAnimation->setDelayPerUnit(DAVE_FRAME_DELAY);

    auto playRandomDaveSound = CallFunc::create([]() {
        int randNum = cocos2d::random(1, 4);
        AudioEngine::play2d(StringUtils::format("dave%d.mp3", randNum));
        });

    auto idleRepeat = Repeat::create(Animate::create(idleAnimation), DAVE_IDLE_LOOPS);
    auto cycle = Sequence::create(idleRepeat, playRandomDaveSound, Animate::create(speakingAnimation), nullptr);
    _daveSprite->runAction(RepeatForever::create(cycle));
}

void ShopScene::setupShopItems()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    float shelf1_y = visibleSize.height * SHELF_1_Y_RATIO;
    float shelf2_y = visibleSize.height * SHELF_2_Y_RATIO;
    float start_x = visibleSize.width * ITEM_START_X_RATIO;
    float spacing_x = visibleSize.width * ITEM_SPACING_X_RATIO;

    // 第一排：道具 (不传 PlantName，传 ShopItemID)
    createShopItem("mower.png", ShopItemID::MOWER, PRICE_MOWER, Vec2(start_x, shelf1_y));
    createShopItem("rake.png", ShopItemID::RAKE, PRICE_RAKE, Vec2(start_x + spacing_x, shelf1_y));

    // 第二排：植物 (传入 PlantName 且传入对应的 ShopItemID)
    createShopItem("seedpacket_twinsunflower.png", ShopItemID::TWIN_SUNFLOWER, PRICE_TWIN_SUNFLOWER, Vec2(start_x, shelf2_y), PlantName::TWINSUNFLOWER);
    createShopItem("seedpacket_gatlingpea.png", ShopItemID::GATLING_PEA, PRICE_GATLING_PEA, Vec2(start_x + spacing_x, shelf2_y), PlantName::GATLINGPEA);
    createShopItem("seedpacket_spikerock.png", ShopItemID::SPIKE_ROCK, PRICE_SPIKE_ROCK, Vec2(start_x + spacing_x * 2, shelf2_y), PlantName::SPIKEROCK);
}
// ShopScene.cpp

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

    // 获取玩家数据实例
    auto profile = PlayerProfile::getInstance();

    // 判定逻辑
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
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [=](Touch* touch, Event* event) mutable {
            auto target = static_cast<Node*>(event->getCurrentTarget());
            Rect hitRect = target->getBoundingBox();

            // 扩大点击判定范围
            if (priceLabel) {
                Rect priceRect = priceLabel->getBoundingBox();
                hitRect.merge(priceRect);
            }

            Vec2 pInParent = target->getParent()->convertToNodeSpace(touch->getLocation());
            if (!hitRect.containsPoint(pInParent)) return false;

            // 再次检查是否已拥有
            if (checkOwned()) {
                AudioEngine::play2d("buzzer.mp3");
                return false;
            }

            // 购买处理
            if (profile->spendCoins(price)) {
                AudioEngine::play2d("buttonclick.mp3");

                switch (itemId) {
                    case ShopItemID::MOWER:          profile->enableMower(true); break;
                    case ShopItemID::RAKE:           profile->enableRake(true); break;
                    case ShopItemID::TWIN_SUNFLOWER:
                    case ShopItemID::GATLING_PEA:
                    case ShopItemID::SPIKE_ROCK:     profile->unlockPlant(plantName); break;
                    default: break;
                }

                soldOutLabel->setVisible(true);
                this->updateCoinLabel(); // 注意这里要用 this-> 访问成员函数
                event->getCurrentTarget()->getEventDispatcher()->removeEventListener(listener);
            }
            else {
                AudioEngine::play2d("buzzer.mp3");
            }
            return true;
            };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, itemSprite);
    }
}

void ShopScene::updateCoinLabel()
{
    if (_coinLabel) {
        _coinLabel->setString(std::to_string(PlayerProfile::getInstance()->getCoins()));
    }
}