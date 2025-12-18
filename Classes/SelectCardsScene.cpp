#include "SelectCardsScene.h"
#include "GameMenu.h"
#include "GameWorld.h"
#include "Zombie.h"
#include "FlagZombie.h"
#include "PoleVaulter.h"
#include "Zomboni.h"
#include "Gargantuar.h"
#include "Imp.h"
#include "ui/CocosGUI.h"
#include <algorithm>
#include "SelectCard.h"
#include "SeedPacket.h"
#include "Sunflower.h"
#include "Sunshroom.h"
#include "PeaShooter.h"
#include "Repeater.h"
#include "ThreePeater.h"
#include "Puffshroom.h"
#include "Wallnut.h"
#include "CherryBomb.h"
#include "SpikeWeed.h"
#include "Jalapeno.h"
#include "TwinSunflower.h"

USING_NS_CC;

namespace
{
    const float SEED_BANK_POS_X_RATIO = 0.312f;
    const float SEED_BANK_POS_Y_RATIO = 0.922f;
    const float SEED_BANK_SCALE_FACTOR_X = 1.36f;
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
        ret->_isNightMode = isNightMode;
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
    : _isNightMode(false)
    , _worldLayer(nullptr)
    , _uiLayer(nullptr)
    , _background(nullptr)
    , _backMenu(nullptr)
    , _selectBG(nullptr)
    , _zombieShowLayer(nullptr)
    , _moveDuration(0.9f)
    , _moveOffsetX(520.0f)
    , _selectBGPosXRatio(0.35f)
    , _selectBGPosYRatio(0.4f)
    , _selectBGScale(1.0f)
    , _zShowStartX(880.0f)
    , _zShowStartY(140.0f)
    , _zShowGapY(120.0f)
    , _selectedCardsContainer(nullptr)
{
}

SelectCardsScene::~SelectCardsScene()
{
    // No need to release SeedPackets anymore since we're not passing pointers to GameWorld
    // SeedPackets are managed by Cocos2d-x's autorelease pool
}

bool SelectCardsScene::init()
{
    if (!Scene::init()) return false;

    _worldLayer = Node::create();
    this->addChild(_worldLayer, 0);

    _uiLayer = Node::create();
    this->addChild(_uiLayer, 10);

    _zombieShowLayer = Node::create();
    this->addChild(_zombieShowLayer, 15);

    buildWorld();
    buildUI();
    runIntroMove();

    return true;
}

void SelectCardsScene::setMoveDuration(float d)
{
    _moveDuration = d;
}

void SelectCardsScene::setMoveOffsetX(float x)
{
    _moveOffsetX = x;
}

void SelectCardsScene::setSelectBGPosRatio(float xr, float yr)
{
    _selectBGPosXRatio = xr;
    _selectBGPosYRatio = yr;
}

void SelectCardsScene::setSelectBGScale(float s)
{
    _selectBGScale = s;
}

void SelectCardsScene::buildWorld()
{
    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    const char* bgFile = _isNightMode ? "select_night.png" : "select_day.png";
    _background = Sprite::create(bgFile);
    if (_background)
    {
        _background->setPosition(Vec2(vs.width * 1.48f / 2.0f + origin.x, vs.height * 0.5f + origin.y));
        float scaleX = vs.width / _background->getContentSize().width * 1.48f;
        float scaleY = vs.height / _background->getContentSize().height;
        _background->setScale(scaleX, scaleY);
        _worldLayer->addChild(_background, 0);
    }
      
    auto seedBankSprite = Sprite::create(SEED_BANK_IMAGE);
    if (seedBankSprite)
    {
        float finalX = vs.width * SEED_BANK_POS_X_RATIO + origin.x;
        float finalY = vs.height * SEED_BANK_POS_Y_RATIO + origin.y;
        seedBankSprite->setPosition(Vec2(finalX, finalY));
        seedBankSprite->setScale(SEED_BANK_SCALE_FACTOR_X, SEED_BANK_SCALE_FACTOR_Y);
        this->addChild(seedBankSprite, 1);
        CCLOG("SeedBank Sprite initialized successfully.");
    }
    else
    {
        CCLOG("Error: Failed to load seed bank image: %s", SEED_BANK_IMAGE.c_str());
    }
    
    // Create container for selected cards (above seedBank)
    _selectedCardsContainer = Node::create();
    this->addChild(_selectedCardsContainer, 2);
}

void SelectCardsScene::buildUI()
{
    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto backItem = MenuItemImage::create(
        "btn_Menu.png",
        "btn_Menu2.png",
        [this](Ref*) {
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameMenu::createScene()));
        }
    );
    backItem->setScale(0.8f);
    backItem->setPosition(Vec2(vs.width - 60.0f + origin.x, vs.height - 40.0f + origin.y));

    _backMenu = Menu::create(backItem, nullptr);
    _backMenu->setPosition(Vec2::ZERO);
    _uiLayer->addChild(_backMenu, 100);
}

void SelectCardsScene::runIntroMove()
{
    if (_worldLayer)
    {
        auto move = MoveBy::create(_moveDuration, Vec2(-_moveOffsetX, 0));
        auto after = CallFunc::create([this]() {
            showSelectBG();
            spawnZombieShowcase();
            createSelectCards();
        });
        _worldLayer->runAction(Sequence::create(move, after, nullptr));
    }
}

void SelectCardsScene::showSelectBG()
{
    if (_selectBG) return;

    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    _selectBG = Sprite::create("SelectCard_BG.png");
    if (_selectBG)
    {
        float finalX = vs.width * _selectBGPosXRatio + origin.x;
        float finalY = vs.height * _selectBGPosYRatio + origin.y;
        _selectBG->setPosition(Vec2(finalX, finalY));
        _selectBG->setScale(_selectBGScale);
        _selectBG->setOpacity(0);
        _uiLayer->addChild(_selectBG, 20);
        _selectBG->runAction(FadeIn::create(0.25f));

        auto startLabel = Label::createWithSystemFont("Let's start!", "Arial", 28);
        auto startItem = MenuItemLabel::create(startLabel, [this](Ref*){
            // Hide selectBG and all cards inside it
            if (_selectBG && _selectBG->isVisible()) {
                _selectBG->runAction(Sequence::create(
                    FadeOut::create(0.25f),
                    CallFunc::create([this](){ 
                        _selectBG->setVisible(false);
                        // Hide all select cards
                        for (auto* card : _allSelectCards) {
                            if (card) card->setVisible(false);
                        }
                    }),
                    nullptr
                ));
            }
            if (_worldLayer) {
                auto moveBack = MoveBy::create(_moveDuration, Vec2(_moveOffsetX, 0));
                auto stayBack = MoveBy::create(_moveDuration, Vec2(_moveOffsetX, 0));
                auto wait = DelayTime::create(2.0f);
                auto goGame = CallFunc::create([this]() {
                    // Pass selected plant names to GameWorld
                    Director::getInstance()->replaceScene(GameWorld::createScene(_isNightMode, _selectedPlantNames));
                });
                _worldLayer->runAction(moveBack);
                _zombieShowLayer->runAction(stayBack);
                _worldLayer->runAction(Sequence::create(wait, goGame, nullptr));
            }
        });
        auto startMenu = Menu::create(startItem, nullptr);
        startMenu->setPosition(Vec2(_selectBG->getContentSize().width * 0.5f, _selectBG->getContentSize().height * 0.08f));
        _selectBG->addChild(startMenu, 1);
    }
}

void SelectCardsScene::spawnZombieShowcase()
{
    int idx = 0;
    auto addShow = [&](Sprite* sp){
        if (!sp) return;
        sp->setPosition(Vec2(_zShowStartX, _zShowStartY + _zShowGapY * idx));
        if (_zombieShowLayer) _zombieShowLayer->addChild(sp, 100 - idx * 2);
        idx++;
    };

    if (auto z = Zombie::createZombie())
        addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = FlagZombie::createZombie())
        addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = PoleVaulter::createZombie())
        addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = Zomboni::createZombie())
        addShow(z->createShowcaseSprite(Vec2::ZERO));
    if (auto z = Gargantuar::createZombie())
        addShow(z->createShowcaseSprite(Vec2::ZERO));
}

void SelectCardsScene::createSelectCards()
{
    if (!_selectBG) return;
    
    // Create all plant seed packets (same as GameWorld)
    struct PlantInfo {
        const char* imageFile;
        float cooldown;
        int sunCost;
        PlantName plantName;
    };
    
    std::vector<PlantInfo> plants = {
        {"seedpacket_sunflower.png", 3.0f, 50, PlantName::SUNFLOWER},
        {"seedpacket_sunshroom.png", 3.0f, 25, PlantName::SUNSHROOM},
        {"seedpacket_peashooter.png", 7.5f, 100, PlantName::PEASHOOTER},
        {"seedpacket_repeater.png", 3.0f, 200, PlantName::REPEATER},
        {"Threepeater_Seed_Packet_PC.png", 3.0f, 325, PlantName::THREEPEATER},
        {"seedpacket_puffshroom.png", 3.0f, 0, PlantName::PUFFSHROOM},
        {"seedpacket_wallnut.png", 30.0f, 50, PlantName::WALLNUT},
        {"seedpacket_cherry_bomb.png", 1.0f, 150, PlantName::CHERRYBOMB},
        {"seedpacket_spikeweed.png", 1.0f, 100, PlantName::SPIKEWEED},
        {"seedpacket_jalapeno.png", 1.0f, 100, PlantName::JALAPENO},
        {"seedpacket_twinsunflower.png", 1.0f, 150, PlantName::TWINSUNFLOWER}
    };
    
    // Create seed packets and select cards
    for (const auto& plant : plants) {
        SeedPacket* seedPacket = nullptr;
        switch (plant.plantName) {
            case PlantName::SUNFLOWER:
                seedPacket = SeedPacket::create<Sunflower>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::SUNSHROOM:
                seedPacket = SeedPacket::create<Sunshroom>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::PEASHOOTER:
                seedPacket = SeedPacket::create<PeaShooter>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::REPEATER:
                seedPacket = SeedPacket::create<Repeater>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::THREEPEATER:
                seedPacket = SeedPacket::create<ThreePeater>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::PUFFSHROOM:
                seedPacket = SeedPacket::create<Puffshroom>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::WALLNUT:
                seedPacket = SeedPacket::create<Wallnut>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::CHERRYBOMB:
                seedPacket = SeedPacket::create<CherryBomb>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::SPIKEWEED:
                seedPacket = SeedPacket::create<SpikeWeed>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::JALAPENO:
                seedPacket = SeedPacket::create<Jalapeno>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            case PlantName::TWINSUNFLOWER:
                seedPacket = SeedPacket::create<TwinSunflower>(plant.imageFile, plant.cooldown, plant.sunCost, plant.plantName);
                break;
            default:
                break;
        }
        
        if (seedPacket) {
            // No need to retain - SeedPacket is managed by Cocos2d-x's autorelease pool
            // We only use it to get the plant name, not to pass the pointer to GameWorld
            auto selectCard = SelectCard::create(plant.imageFile, plant.plantName, seedPacket);
            if (selectCard) {
                _allSelectCards.push_back(selectCard);
                _selectBG->addChild(selectCard, 10);
            }
        }
    }
    
    // Layout cards: 8 per row, reference GameWorld spacing
    const float cardSpacingX = 65.0f;
    const float cardSpacingY = 75.0f;  // Increased vertical spacing to avoid overlap
    const float cardStartX = 50.0f;  // Offset from selectBG left edge
    const float cardStartY = _selectBG->getContentSize().height - 100.0f;  // Offset from top
    
    for (size_t i = 0; i < _allSelectCards.size(); ++i) {
        if (_allSelectCards[i]) {
            int row = i / 8;
            int col = i % 8;
            float x = cardStartX + col * cardSpacingX;
            float y = cardStartY - row * cardSpacingY;
            _allSelectCards[i]->setPosition(Vec2(x, y));
            
            // Set callback for card selection
            auto card = _allSelectCards[i];
            auto listener = EventListenerTouchOneByOne::create();
            listener->setSwallowTouches(true);
            listener->onTouchBegan = [this, card](Touch* touch, Event* event) -> bool {
                if (!card || !card->getParent()) return false;
                // Convert touch location to card's parent (selectBG) coordinate space
                Vec2 locationInParent = card->getParent()->convertTouchToNodeSpace(touch);
                // Get card's position in parent space
                Vec2 cardPos = card->getPosition();
                // Get card's content size
                Size cardSize = card->getContentSize();
                // Check if touch is within card bounds (considering anchor point)
                Vec2 anchorOffset = Vec2(cardSize.width * card->getAnchorPoint().x, 
                                         cardSize.height * card->getAnchorPoint().y);
                Rect cardRect = Rect(cardPos.x - anchorOffset.x, 
                                     cardPos.y - anchorOffset.y, 
                                     cardSize.width, cardSize.height);
                if (cardRect.containsPoint(locationInParent)) {
                    this->onCardSelected(card);
                    return true;
                }
                return false;
            };
            Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, card);
        }
    }
}

void SelectCardsScene::onCardSelected(SelectCard* card)
{
    if (!card) return;
    
    bool wasSelected = card->isSelected();
    
    // Maximum 9 plants can be selected
    if (!wasSelected && _selectedCards.size() >= 9) {
        return;  // Cannot select more
    }
    
    card->setSelected(!wasSelected);
    
    auto* seedPacket = card->getSeedPacket();
    if (!seedPacket) return;  // Safety check
    
    if (card->isSelected()) {
        // Add to selected list
        _selectedCards.push_back(card);
        _selectedPlantNames.push_back(card->getPlantName());
    } else {
        // Remove from selected list
        auto it = std::find(_selectedCards.begin(), _selectedCards.end(), card);
        if (it != _selectedCards.end()) {
            _selectedCards.erase(it);
        }
        auto it2 = std::find(_selectedPlantNames.begin(), _selectedPlantNames.end(), card->getPlantName());
        if (it2 != _selectedPlantNames.end()) {
            _selectedPlantNames.erase(it2);
        }
    }
    
    updateSelectedCardsDisplay();
}

void SelectCardsScene::updateSelectedCardsDisplay()
{
    if (!_selectedCardsContainer) return;
    
    // Clear existing display and listeners
    _selectedCardsContainer->removeAllChildren();
    _displaySprites.clear();
    
    // Reference GameWorld positions
    auto vs = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    float baseX = 187.0f;
    float baseY = 667.0f;
    float spacing = 65.0f;
    
    // Display selected seed packets in seedBank area
    // Use SelectCard* instead of SeedPacket* to avoid accessing freed memory
    for (size_t i = 0; i < _selectedCards.size(); ++i) {
        auto* card = _selectedCards[i];
        if (!card) continue;  // Safety check
        
        // Get image filename from SelectCard and create sprite from file
        const std::string& imageFile = card->getImageFile();
        if (imageFile.empty()) continue;
        
        auto displaySprite = Sprite::create(imageFile);
        if (displaySprite) {
            displaySprite->setPosition(Vec2(baseX + i * spacing + origin.x, baseY + origin.y));
            // No scaling - same size as in main scene
            _selectedCardsContainer->addChild(displaySprite);
            _displaySprites.push_back(displaySprite);
            
            // Add touch listener to allow deselecting by clicking on seedBank card
            auto listener = EventListenerTouchOneByOne::create();
            listener->setSwallowTouches(true);
            listener->onTouchBegan = [this, card](Touch* touch, Event* event) -> bool {
                if (!card) return false;
                // Use getBoundingBox for touch detection
                auto* target = static_cast<Sprite*>(event->getCurrentTarget());
                Rect boundingBox = target->getBoundingBox();
                Vec2 touchLocation = touch->getLocation();
                if (boundingBox.containsPoint(touchLocation)) {
                    // Deselect the card
                    this->onCardSelected(card);
                    return true;
                }
                return false;
            };
            Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, displaySprite);
        }
    }
}
