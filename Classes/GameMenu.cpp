/**
 * @file GameMenu.cpp
 * @brief ʵ����Ϸ���˵������������������úͲ˵����ܡ�
 */

#include "GameMenu.h"
#include "GameWorld.h" 
#include "SelectCardsScene.h"
#include "cocos2d.h"
#include "ui/UIButton.h" 
#include "audio/include/AudioEngine.h"
#include <vector> 
#include <string> 
#include <functional> 

// ʹ�������ռ�򻯴���
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

// =========================================================================
// 1. �������� (createScene)
// =========================================================================

Scene* GameMenu::createScene()
{
    // ʹ�� Cocos2d-x �� CREATE_FUNC �괴������ʵ��
    return GameMenu::create();
}

// =========================================================================
// 2. �������ýṹ�� (ģ�廯����)
// =========================================================================

/**
 * @brief ����˵���ť�����ýṹ�壬������ѭ���е����á�
 */
struct ButtonConfig {
    std::string text;
    // �ص�����
    std::function<void(Ref*)> callback;
};

/**
 * @brief �����һ����ť��ģ�壩���Ӿ������ṹ�塣
 */
struct ButtonVisualTemplate {
    float x_offset_ratio = 0.0f; // X �����������Ļ���Ŀ��ȵ�ƫ�Ʊ���
    float rotation_angle = 0.0f; // ��ת�Ƕ�
    float scale_factor = 1.0f;   // �Ŵ���
};

// =========================================================================
// 3. ������ʼ�� (init)
// =========================================================================

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

    // --- A. ���л����ֲ��� ---
    const float FONT_SIZE = 40;
    const float VERTICAL_PADDING = 120.0f; // ��ť֮��Ĵ�ֱ���
    // ���޸ĵ� 1�����������ϵ�������������ƫ����
    const float START_Y_OFFSET = 250.0f; // �˵����������Ļ���ĵ�����ƫ����

    // --- B. ��һ����ť���Ӿ�ģ�� (���а�ť�������մ�ģ��) ---
    ButtonVisualTemplate DAY_MODE_TEMPLATE = {
        // ���޸ĵ� 2�����������ҵ��������� X ��ƫ�Ʊ���
        0.2f,     // X ��ƫ�Ʊ��� (0.1f ������Ļ���ȵ� 10% ƫ��)
        // ���޸ĵ� 3������ת 10 ��
        5.0f,    // ��ת�Ƕ� (10.0f)
        1.2f      // �Ŵ��� (���ֲ���)
    };

    // --- C. ���� Lambda ����ʽ���ڻص� ---
    auto dayModeCallback = [](Ref* sender) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectCardsScene::createScene(false)));
    };

    auto nightModeCallback = [](Ref* sender) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectCardsScene::createScene(true)));
    };

    auto exitCallback = [this](Ref* sender) {
        this->menuCloseCallback(sender);
        };

    // --- D. �򻯰�ť�����б� ---
    std::vector<ButtonConfig> configs = {
        { "1. day mode",         dayModeCallback },
        { "2. night mode",       nightModeCallback },
        { "3. shop",             nullptr },
        { "4. exit",             exitCallback }
    };

    // --- E. ȫ�ֱ������� ---
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

    // --- F. �Զ������Ͷ�λ��ť�������߼��� ---
    // ���޸ĵ㡿��Y ����ʼλ��ʹ���µ� START_Y_OFFSET
    float currentY = visibleSize.height / 2 + origin.y + START_Y_OFFSET;
    float centerX = visibleSize.width / 2 + origin.x;

    for (size_t i = 0; i < configs.size(); ++i)
    {
        const auto& config = configs[i];

        auto button = Button::create();
        button->setTitleText(config.text);
        button->setTitleFontSize(FONT_SIZE);
        button->setTitleColor(Color3B::WHITE);

        // --- Ӧ��ģ����� ---

        // 1. ���� X ���꣨ʹ��ģ��� X ƫ�ƣ�
        float finalX = centerX + (visibleSize.width * DAY_MODE_TEMPLATE.x_offset_ratio);

        // 2. ����λ�ã�Y ������ѭ���м��㣩
        button->setPosition(Vec2(finalX, currentY));

        // 3. ������ת�Ƕȣ�ʹ��ģ�����ת�ǣ�
        button->setRotation(DAY_MODE_TEMPLATE.rotation_angle);

        // 4. ���÷Ŵ�����ʹ��ģ������ű�����
        button->setScale(DAY_MODE_TEMPLATE.scale_factor);

        // ------------------------------------

        // ���ûص�����
        if (config.callback)
        {
            button->addTouchEventListener([callback = config.callback](Ref* sender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    callback(sender);
                }
                });
        }

        this->addChild(button, 2);

        // ������һ����ť��Y����
        currentY -= VERTICAL_PADDING;
    }

    // Stop any existing music and play menu music
    if (_backgroundMusicId != cocos2d::AudioEngine::INVALID_AUDIO_ID)
    {
        cocos2d::AudioEngine::stop(_backgroundMusicId);
    }
    _backgroundMusicId = cocos2d::AudioEngine::play2d("title.mp3", true);

    return true;
}

// =========================================================================
// 4. �˳���Ϸ�ص� (menuCloseCallback)
// =========================================================================

/**
 * @brief �˳���Ϸ�Ļص�������
 * @param sender �����ص��Ķ���
 */
void GameMenu::menuCloseCallback(Ref* sender)
{
    // ֹͣ Cocos2d-x �������ѭ��
    Director::getInstance()->end();

    // ��� iOS ƽ̨����Ҫ��ʽ���� exit(0)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}