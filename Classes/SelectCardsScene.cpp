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
{
}

SelectCardsScene::~SelectCardsScene()
{
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
            if (_selectBG && _selectBG->isVisible()) {
                _selectBG->runAction(Sequence::create(
                    FadeOut::create(0.25f),
                    CallFunc::create([this](){ _selectBG->setVisible(false); }),
                    nullptr
                ));
            }
            if (_worldLayer) {
                auto moveBack = MoveBy::create(_moveDuration, Vec2(_moveOffsetX, 0));
                auto stayBack = MoveBy::create(_moveDuration, Vec2(_moveOffsetX, 0));
                auto wait = DelayTime::create(2.0f);
                auto goGame = CallFunc::create([this]() {
                    Director::getInstance()->replaceScene(GameWorld::createScene(_isNightMode));
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
