#include "SelectCardsScene.h"
#include "GameMenu.h"
#include "GameWorld.h"
#include "Zombie.h"
#include "FlagZombie.h"
#include "PoleVaulter.h"
#include "Zomboni.h"
#include "Gargantuar.h"
#include "Imp.h"
#include "BucketHeadZombie.h"
#include "NormalZombie.h"
#include "ui/CocosGUI.h"
#include <algorithm>
#include "SelectCard.h"
#include "SeedPacket.h"
#include "audio/include/AudioEngine.h"
#include "PlayerProfile.h"

USING_NS_CC;

namespace
{
    // Layout constants for the Seed Bank UI (the bar that holds selected cards)
    const float SEED_BANK_POS_X_RATIO = 0.315f;
    const float SEED_BANK_POS_Y_RATIO = 0.927f;
    const float SEED_BANK_SCALE_FACTOR_X = 1.34f;
    const float SEED_BANK_SCALE_FACTOR_Y = 1.0f;
    const std::string SEED_BANK_IMAGE = "seedBank.png";
}

SelectCardsScene* SelectCardsScene::createScene(bool isNightMode)
{
    auto scene = SelectCardsScene::create(isNightMode);
    return scene;
}

SelectCardsScene* SelectCardsScene::create(bool isNightMode)
{
    SelectCardsScene* ret = new (std::nothrow) SelectCardsScene();
    if (ret)
    {
        ret->is_night_mode = isNightMode;
        if (ret->init())
        {
            ret->autorelease();
            return ret;
        }
        delete ret;
    }
    return nullptr;
}

SelectCardsScene::SelectCardsScene()
    : is_night_mode(false)
    , world_layer(nullptr)
    , ui_layer(nullptr)
    , background(nullptr)
    , back_menu(nullptr)
    , selectBG(nullptr)
    , zombie_show_layer(nullptr)
    , move_duration(0.9f)
    , move_offsetX(520.0f)
    , selectBG_posX_ratio(0.35f)
    , selectBG_posY_ratio(0.4f)
    , selectBG_scale(1.0f)
    , zShow_startX(880.0f)
    , zShow_startY(140.0f)
    , zShow_gapX(200.0f)
    , zShow_gapY(150.0f)
    , selected_cards_container(nullptr)
{
}

SelectCardsScene::~SelectCardsScene()
{
    // Destructor: SeedPackets are autoreleased by Cocos2d-x
}

bool SelectCardsScene::init()
{
    if (!Scene::init()) return false;

    // Layer organization: World (Background) -> Zombies -> UI (Cards/Menus)
    world_layer = Node::create();
    this->addChild(world_layer, 0);

    ui_layer = Node::create();
    this->addChild(ui_layer, 10);

    zombie_show_layer = Node::create();
    this->addChild(zombie_show_layer, 8);

    buildWorld();
    buildUI();
    runIntroMove();

    // Initialize background music for selection
    select_BgmId = cocos2d::AudioEngine::play2d("select.mp3", true);

    return true;
}

// --- Configuration Setters ---
void SelectCardsScene::setMoveDuration(float d) { move_duration = d; }
void SelectCardsScene::setMoveOffsetX(float x) { move_offsetX = x; }
void SelectCardsScene::setSelectBGPosRatio(float xr, float yr) { selectBG_posX_ratio = xr; selectBG_posY_ratio = yr; }
void SelectCardsScene::setSelectBGScale(float s) { selectBG_scale = s; }

void SelectCardsScene::buildWorld()
{
    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // Setup environmental background (Day/Night)
    const char* bgFile = is_night_mode ? "select_night.png" : "select_day.png";
    background = Sprite::create(bgFile);
    if (background)
    {
        background->setPosition(Vec2(vs.width * 1.47f / 2.0f + origin.x, vs.height * 0.5f + origin.y));
        float scaleX = vs.width / background->getContentSize().width * 1.47f;
        float scaleY = vs.height / background->getContentSize().height;
        background->setScale(scaleX, scaleY);
        world_layer->addChild(background, 0);
    }

    // Setup the Seed Bank UI bar
    auto seedBankSprite = Sprite::create(SEED_BANK_IMAGE);
    if (seedBankSprite)
    {
        float finalX = vs.width * SEED_BANK_POS_X_RATIO + origin.x;
        float finalY = vs.height * SEED_BANK_POS_Y_RATIO + origin.y;
        seedBankSprite->setPosition(Vec2(finalX, finalY));
        seedBankSprite->setScale(SEED_BANK_SCALE_FACTOR_X, SEED_BANK_SCALE_FACTOR_Y);
        this->addChild(seedBankSprite, 1);
    }

    // Container to hold the actual plant sprites inside the Seed Bank
    selected_cards_container = Node::create();
    this->addChild(selected_cards_container, 2);
}

void SelectCardsScene::buildUI()
{
    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // Create the "Back to Menu" button
    auto backItem = MenuItemImage::create(
        "btn_Menu.png",
        "btn_Menu2.png",
        [this](Ref*) {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
            if (select_BgmId != cocos2d::AudioEngine::INVALID_AUDIO_ID) {
                cocos2d::AudioEngine::stop(select_BgmId);
                select_BgmId = cocos2d::AudioEngine::INVALID_AUDIO_ID;
            }
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameMenu::createScene()));
        }
    );
    backItem->setScale(0.8f);
    backItem->setPosition(Vec2(vs.width - 100.0f + origin.x, vs.height - 40.0f + origin.y));

    back_menu = Menu::create(backItem, nullptr);
    back_menu->setPosition(Vec2::ZERO);
    ui_layer->addChild(back_menu, 100);
}

void SelectCardsScene::runIntroMove()
{
    if (world_layer)
    {
        // Pan the camera to the right to show zombies, then reveal selection UI
        auto move = MoveBy::create(move_duration, Vec2(-move_offsetX, 0));
        auto after = CallFunc::create([this]() {
            showSelectBG();
            spawnZombieShowcase();
            createSelectCards();
            });
        world_layer->runAction(Sequence::create(move, after, nullptr));
    }
}

void SelectCardsScene::showSelectBG()
{
    if (selectBG) return;

    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // Initialize the main wooden board for card selection
    selectBG = Sprite::create("SelectCard_BG.png");
    if (selectBG)
    {
        float finalX = vs.width * selectBG_posX_ratio + origin.x;
        float finalY = vs.height * selectBG_posY_ratio + origin.y;
        selectBG->setPosition(Vec2(finalX, finalY));
        selectBG->setScale(selectBG_scale);
        selectBG->setOpacity(0);
        ui_layer->addChild(selectBG, 20);
        selectBG->runAction(FadeIn::create(0.25f));

        // Create the "Let's Start" button on the board
        auto startLabel = Label::createWithSystemFont("Let's start!", "Arial", 28);
        auto startItem = MenuItemLabel::create(startLabel, [this](Ref*) {
            cocos2d::AudioEngine::play2d("buttonclick.mp3", false);

            // Hide the selection board and pan camera back to start the game
            if (selectBG && selectBG->isVisible()) {
                selectBG->runAction(Sequence::create(
                    FadeOut::create(0.25f),
                    CallFunc::create([this]() {
                        selectBG->setVisible(false);
                        for (auto* card : all_select_cards) {
                            if (card) card->setVisible(false);
                        }
                        }),
                    nullptr
                ));
            }
            if (world_layer) {
                auto moveBack = MoveBy::create(move_duration, Vec2(move_offsetX, 0));
                auto stayBack = MoveBy::create(move_duration, Vec2(move_offsetX, 0));
                world_layer->runAction(Sequence::create(
                    moveBack,
                    CallFunc::create([this]() {
                        if (select_BgmId != cocos2d::AudioEngine::INVALID_AUDIO_ID) {
                            cocos2d::AudioEngine::stop(select_BgmId);
                            select_BgmId = cocos2d::AudioEngine::INVALID_AUDIO_ID;
                        }
                        playReadySetPlantSequence();
                        }),
                    nullptr
                ));
                if (zombie_show_layer) zombie_show_layer->runAction(stayBack);
            }
            });
        auto startMenu = Menu::create(startItem, nullptr);
        startMenu->setPosition(Vec2(selectBG->getContentSize().width * 0.5f, selectBG->getContentSize().height * 0.08f));
        selectBG->addChild(startMenu, 1);
    }
}

void SelectCardsScene::spawnZombieShowcase()
{
    // Utility to arrange preview zombies in a grid on the right side of the lawn
    int idx = 0, idy = 0;
    auto addShow = [&](Sprite* sp) {
        if (!sp) return;
        sp->setPosition(Vec2(zShow_startX + zShow_gapX * idx, zShow_startY + zShow_gapY * idy));
        if (zombie_show_layer) zombie_show_layer->addChild(sp, 7 - idy);
        idy++;
        if (idy >= 4) {
            idy = 0;
            ++idx;
        }
        };

    // Instantiate various zombies for the preview sequence
    if (auto z = NormalZombie::createZombie()) addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = FlagZombie::createZombie()) addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = PoleVaulter::createZombie()) addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = Zomboni::createZombie()) addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = Gargantuar::createZombie()) addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = BucketHeadZombie::createZombie()) addShow(z->createShowcaseSprite(Vec2::ZERO));
}

void SelectCardsScene::createSelectCards()
{
    if (!selectBG) return;

    // 1. Iterate through Global Config to automatically create plant cards
    for (std::map<PlantName, PlantConfig>::const_iterator it = SeedPacket::CONFIG_TABLE.begin();
        it != SeedPacket::CONFIG_TABLE.end(); ++it)
    {
        PlantName name = it->first;
        const PlantConfig& cfg = it->second;

        SeedPacket* seedPacket = SeedPacket::createFromConfig(name);
        if (seedPacket) {
            auto selectCard = SelectCard::create(cfg.packetImage, name, seedPacket);
            if (selectCard) {
                all_select_cards.push_back(selectCard);
                selectBG->addChild(selectCard, 10);
            }
        }
    }

    // 2. Grid Layout Logic (8 columns)
    const float cardSpacingX = 65.0f;
    const float cardSpacingY = 85.0f;
    const float cardStartX = 50.0f;
    const float cardStartY = selectBG->getContentSize().height - 100.0f;

    for (size_t i = 0; i < all_select_cards.size(); ++i) {
        auto card = all_select_cards[i];
        if (!card) continue;

        int row = i / 8;
        int col = i % 8;
        card->setPosition(Vec2(cardStartX + col * cardSpacingX, cardStartY - row * cardSpacingY));

        // Darken cards if plant is not yet unlocked by the player
        if (!PlayerProfile::getInstance()->isPlantUnlocked(card->getPlantName())) {
            card->setColor(Color3B(100, 100, 100));
        }

        // 3. Bind Touch Events for each card
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this, card](Touch* touch, Event* event) -> bool {
            if (!can_select_cards) return false;

            if (!card || !card->getParent()) return false;
            Vec2 locationInParent = card->getParent()->convertTouchToNodeSpace(touch);
            Rect cardRect = card->getBoundingBox();

            if (cardRect.containsPoint(locationInParent)) {
                this->onCardSelected(card);
                return true;
            }
            return false;
            };
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, card);
    }
}


void SelectCardsScene::onCardSelected(SelectCard* card)
{
    if (!card) return;

    // Check unlock status
    if (!PlayerProfile::getInstance()->isPlantUnlocked(card->getPlantName())) {
        cocos2d::AudioEngine::play2d("buzzer.mp3", false);
        return;
    }

    bool wasSelected = card->isSelected();

    // Limit deck to 9 slots
    if (!wasSelected && selected_cards.size() >= 9) {
        cocos2d::AudioEngine::play2d("buzzer.mp3", false);
        return;
    }

    cocos2d::AudioEngine::play2d("buttonclick.mp3", false);
    card->setSelected(!wasSelected);

    auto* seedPacket = card->getSeedPacket();
    if (!seedPacket) return;

    if (card->isSelected()) {
        selected_cards.push_back(card);
        selected_plant_names.push_back(card->getPlantName());
    }
    else {
        auto it = std::find(selected_cards.begin(), selected_cards.end(), card);
        if (it != selected_cards.end()) {
            selected_cards.erase(it);
        }
        auto it2 = std::find(selected_plant_names.begin(), selected_plant_names.end(), card->getPlantName());
        if (it2 != selected_plant_names.end()) {
            selected_plant_names.erase(it2);
        }
    }

    updateSelectedCardsDisplay();
}

void SelectCardsScene::updateSelectedCardsDisplay()
{
    if (!selected_cards_container) return;

    selected_cards_container->removeAllChildren();
    display_sprites.clear();

    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    float baseX = 187.0f;
    float baseY = 667.0f;
    float spacing = 65.0f;

    // Refresh the Seed Bank bar with currently selected plant sprites
    for (size_t i = 0; i < selected_cards.size(); ++i) {
        auto* card = selected_cards[i];
        if (!card) continue;

        const std::string& imageFile = card->getImageFile();
        if (imageFile.empty()) continue;

        auto displaySprite = Sprite::create(imageFile);
        if (displaySprite) {
            displaySprite->setPosition(Vec2(baseX + i * spacing + origin.x, baseY + origin.y));
            selected_cards_container->addChild(displaySprite);
            display_sprites.push_back(displaySprite);

            // Allow deselecting by clicking on the Seed Bank cards
            auto listener = EventListenerTouchOneByOne::create();
            listener->setSwallowTouches(true);
            listener->onTouchBegan = [this, card](Touch* touch, Event* event) -> bool {
                if (!can_select_cards) return false;
                if (!card) return false;

                auto* target = static_cast<Sprite*>(event->getCurrentTarget());
                Rect boundingBox = target->getBoundingBox();
                Vec2 touchLocation = touch->getLocation();
                if (boundingBox.containsPoint(touchLocation)) {
                    this->onCardSelected(card);
                    return true;
                }
                return false;
                };
            Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, displaySprite);
        }
    }
}

void SelectCardsScene::playReadySetPlantSequence()
{
    if (is_transitioning) return;
    is_transitioning = true;
    can_select_cards = false;

    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center(vs.width * 0.5f + origin.x, vs.height * 0.5f + origin.y);

    // Create sprites for "Ready, Set, Plant!" text panels
    auto s1 = Sprite::create("w_0000_Group-1.png");
    auto s2 = Sprite::create("w_0001_Group-2.png");
    auto s3 = Sprite::create("w_0001_Group-3.png");

    float maxScale = 1.0f;
    auto setupSprite = [&](Sprite* sp) {
        if (!sp) return;
        sp->setPosition(center);
        sp->setOpacity(0);
        sp->setScale(maxScale);
        this->addChild(sp, 1000);
        };
    setupSprite(s1); setupSprite(s2); setupSprite(s3);

    // Animation sequence for transition panels
    float dur = 0.6f;
    if (s1) s1->runAction(Sequence::create(FadeIn::create(0.1f), DelayTime::create(dur), FadeOut::create(0.1f), nullptr));
    if (s2) s2->runAction(Sequence::create(DelayTime::create(dur + 0.2f), FadeIn::create(0.1f), DelayTime::create(dur), FadeOut::create(0.1f), nullptr));
    if (s3) s3->runAction(Sequence::create(DelayTime::create(2 * dur + 0.4f), FadeIn::create(0.1f), DelayTime::create(dur), FadeOut::create(0.1f), nullptr));

    readySfxId = cocos2d::AudioEngine::play2d("plants-vs-zombies-ready-set-plant.mp3", false);

    // Final scene swap to the game world after the sequence completes
    float totalWait = 2.6f;
    this->runAction(Sequence::create(
        DelayTime::create(totalWait),
        CallFunc::create([this]() { maybeGoGame(); }),
        nullptr
    ));
}

void SelectCardsScene::maybeGoGame()
{
    cocos2d::AudioEngine::stopAll();
    // Navigate to the main gameplay scene
    Director::getInstance()->replaceScene(GameWorld::createScene(is_night_mode, selected_plant_names));
}