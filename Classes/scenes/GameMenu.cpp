#include "GameMenu.h"
#include "GameWorld.h"
#include "SelectCardsScene.h"
#include "ShopScene.h"
#include "cocos2d.h"
#include "ui/UIButton.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;
using namespace ui;

GameMenu::GameMenu()
    : background_music_id(-1)
{
}

GameMenu::~GameMenu()
{
    // Stop background music when leaving the scene
    if (background_music_id != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(background_music_id);
    }
}

Scene* GameMenu::createScene()
{
    return GameMenu::create();
}

bool GameMenu::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // Stop all previous audio and reset music handle
    cocos2d::AudioEngine::stopAll();
    background_music_id = cocos2d::AudioEngine::INVALID_AUDIO_ID;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Configuration constants for layout
    const float FONT_SIZE = 40;
    const float VERTICAL_PADDING = 120.0f;
    const float START_Y_OFFSET = 250.0f;

    // --- Background Setup ---
    auto background = Sprite::create("MenuBackground.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        float scaleX = visibleSize.width / background->getContentSize().width;
        float scaleY = visibleSize.height / background->getContentSize().height;
        background->setScale(MAX(scaleX, scaleY));
        this->addChild(background, 0);
    }

    // --- Menu Buttons Logic ---
    // Note: Reverted to your original manual button creation logic
    float currentY = visibleSize.height / 2 + origin.y + START_Y_OFFSET;
    float centerX = visibleSize.width / 2 + origin.x;

    // 1. Day Mode Button
    auto dayBtn = Button::create();
    dayBtn->setTitleText("1. day mode");
    dayBtn->setTitleFontSize(FONT_SIZE);
    dayBtn->setPosition(Vec2(centerX + (visibleSize.width * 0.2f), currentY));
    dayBtn->setRotation(5.0f);
    dayBtn->setScale(1.2f);
    dayBtn->addTouchEventListener([](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectCardsScene::createScene(false)));
        }
        });
    this->addChild(dayBtn, 2);

    // 2. Night Mode Button
    currentY -= VERTICAL_PADDING;
    auto nightBtn = Button::create();
    nightBtn->setTitleText("2. night mode");
    nightBtn->setTitleFontSize(FONT_SIZE);
    nightBtn->setPosition(Vec2(centerX + (visibleSize.width * 0.2f), currentY));
    nightBtn->setRotation(5.0f);
    nightBtn->setScale(1.2f);
    nightBtn->addTouchEventListener([](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectCardsScene::createScene(true)));
        }
        });
    this->addChild(nightBtn, 2);

    // 3. Shop Button
    currentY -= VERTICAL_PADDING;
    auto shopBtn = Button::create();
    shopBtn->setTitleText("3. shop");
    shopBtn->setTitleFontSize(FONT_SIZE);
    shopBtn->setPosition(Vec2(centerX + (visibleSize.width * 0.2f), currentY));
    shopBtn->setRotation(5.0f);
    shopBtn->setScale(1.2f);
    shopBtn->addTouchEventListener([](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, ShopScene::createScene()));
        }
        });
    this->addChild(shopBtn, 2);

    // 4. Exit Button
    currentY -= VERTICAL_PADDING;
    auto exitBtn = Button::create();
    exitBtn->setTitleText("4. exit");
    exitBtn->setTitleFontSize(FONT_SIZE);
    exitBtn->setPosition(Vec2(centerX + (visibleSize.width * 0.2f), currentY));
    exitBtn->setRotation(5.0f);
    exitBtn->setScale(1.2f);
    exitBtn->addTouchEventListener([this](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            this->menuCloseCallback(sender);
        }
        });
    this->addChild(exitBtn, 2);

    // --- Audio Playback ---
    background_music_id = cocos2d::AudioEngine::play2d("title.mp3", true);

    return true;
}

void GameMenu::menuCloseCallback(Ref* sender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}