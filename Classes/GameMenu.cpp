#include "GameMenu.h"
#include "GameWorld.h"
#include "SelectCardsScene.h"
#include "ShopScene.h"
#include "cocos2d.h"
#include "ui/UIButton.h"
#include "audio/include/AudioEngine.h"
#include <vector>
#include <string>
#include <functional>

USING_NS_CC;
using namespace ui;

GameMenu::GameMenu()
    : _backgroundMusicId(-1)
{}

GameMenu::~GameMenu()
{
    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
    }
}

struct ButtonConfig {
    std::string text;
    std::function<void(Ref*)> callback;
};

struct ButtonVisualTemplate {
    float x_offset_ratio = 0.0f;
    float rotation_angle = 0.0f;
    float scale_factor = 1.0f;
};

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

    cocos2d::AudioEngine::stopAll();
    _backgroundMusicId = cocos2d::AudioEngine::INVALID_AUDIO_ID;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    const float FONT_SIZE = 40;
    const float VERTICAL_PADDING = 120.0f;
    const float START_Y_OFFSET = 250.0f;

    ButtonVisualTemplate DAY_MODE_TEMPLATE = {
        0.2f,
        5.0f,
        1.2f
    };

    auto dayModeCallback = [](Ref* sender) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectCardsScene::createScene(false)));
    };

    auto nightModeCallback = [](Ref* sender) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectCardsScene::createScene(true)));
    };

    auto shopCallback = [](Ref* sender) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, ShopScene::createScene()));
    };

    auto exitCallback = [this](Ref* sender) {
        this->menuCloseCallback(sender);
    };

    std::vector<ButtonConfig> configs = {
        { "1. day mode",         dayModeCallback },
        { "2. night mode",       nightModeCallback },
        { "3. shop",             shopCallback },
        { "4. exit",             exitCallback }
    };

    auto background = Sprite::create("MenuBackground.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        float scaleX = visibleSize.width / background->getContentSize().width;
        float scaleY = visibleSize.height / background->getContentSize().height;
        background->setScale(MAX(scaleX, scaleY));
        this->addChild(background, 0);
    }
    else
    {
        log("Error: Failed to load background image: MenuBackground.png");
    }

    float currentY = visibleSize.height / 2 + origin.y + START_Y_OFFSET;
    float centerX = visibleSize.width / 2 + origin.x;

    for (size_t i = 0; i < configs.size(); ++i)
    {
        const auto& config = configs[i];

        auto button = Button::create();
        button->setTitleText(config.text);
        button->setTitleFontSize(FONT_SIZE);
        button->setTitleColor(Color3B::WHITE);

        float finalX = centerX + (visibleSize.width * DAY_MODE_TEMPLATE.x_offset_ratio);
        button->setPosition(Vec2(finalX, currentY));
        button->setRotation(DAY_MODE_TEMPLATE.rotation_angle);
        button->setScale(DAY_MODE_TEMPLATE.scale_factor);

        if (config.callback)
        {
            button->addTouchEventListener([callback = config.callback](Ref* sender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    callback(sender);
                }
            });
        }

        this->addChild(button, 2);

        currentY -= VERTICAL_PADDING;
    }

    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
    }
    _backgroundMusicId = cocos2d::AudioEngine::play2d("title.mp3", true);

    return true;
}

void GameMenu::menuCloseCallback(Ref* sender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}