/**
 * @file GameMenu.cpp
 * @brief 实现游戏主菜单场景，包含背景设置和菜单功能。
 */

#include "GameMenu.h"
#include "GameWorld.h" // 引入占位 GameWorld 场景头文件
#include "cocos2d.h"

 // 使用命名空间简化代码
USING_NS_CC;

// =========================================================================
// 1. 场景创建 (createScene)
// =========================================================================

/**
 * @brief 创建并返回 GameMenu 场景。
 * * @return cocos2d::Scene* */
Scene* GameMenu::createScene()
{
    // 使用 Cocos2d-x 的 CREATE_FUNC 宏创建场景实例
    return GameMenu::create();
}

// =========================================================================
// 2. 场景初始化 (init)
// =========================================================================

/**
 * @brief 初始化 GameMenu 场景的内容。
 * * @return true 初始化成功，false 初始化失败
 */
bool GameMenu::init()
{
    // 调用父类 Scene 的 init()
    if (!Scene::init())
    {
        return false;
    }

    // 获取可见区域大小和原点，用于适配不同设备
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // --- A. 全局背景 (LoadingPage.png) ---
    // 使用 LoadingPage.png 作为全局背景 Sprite
    auto background = Sprite::create("LoadingPage.png");
    if (background)
    {
        // 居中显示
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        // 缩放以适配屏幕宽度和高度中较大的比例，以确保画面填满屏幕 (类似 Zoom 效果)
        float scaleX = visibleSize.width / background->getContentSize().width;
        float scaleY = visibleSize.height / background->getContentSize().height;
        background->setScale(MAX(scaleX, scaleY));

        this->addChild(background, 0); // Z-order 0 (最底层)
    }
    else
    {
        log("Error: Failed to load background image: LoadingPage.png");
    }

    // --- B. 菜单背景图 (Menu.png) ---

    auto menuBackground = Sprite::create("Menu.png");
    if (menuBackground)
    {
        menuBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(menuBackground, 1); // Z-order 1 (在全局背景之上)
    }
    else
    {
        log("Error: Failed to load menu background image: Menu.png");
    }

    // --- C. 菜单项创建 (MenuItemLabel) ---

    // 定义菜单项的公共属性
    // 进一步增大字体大小，提高可见性和点击区域
    const float FONT_SIZE = 100;
    // 正常颜色：白色
    const Color3B NORMAL_COLOR = Color3B::WHITE;
    // 选中颜色：黄色 (提供视觉反馈)
    const Color3B SELECTED_COLOR = Color3B::YELLOW;
    // 增加垂直间距，进一步确保点击区域分离
    const float VERTICAL_PADDING = 40.0f;

    // 菜单项文本内容
    const std::string DAY_MODE_TEXT = "1. day mode";
    const std::string NIGHT_MODE_TEXT = "2. nigth mode";
    const std::string SHOP_TEXT = "3.shop";
    const std::string EXIT_TEXT = "4. exit";


    /**
     * @brief 辅助函数：创建 MenuItemLabel，使用两个 Label 实现正常/选中状态的颜色切换。
     * @param text 菜单项显示的文本
     * @param callback 菜单项的回调函数
     * @return MenuItem* 创建的菜单项
     */
    auto createTextButton = [&](const std::string& text, const ccMenuCallback& callback) -> MenuItem* {
        // 正常 Label (白色)
        auto normalLabel = Label::create();
        normalLabel->setString(text);
		normalLabel->setScale(1.5); // 统一字体大小

        return MenuItemLabel::create(normalLabel, callback);
        };


    // 1. 白天模式 (功能按钮) -> 进入 GameWorld 场景
    auto dayModeItem = createTextButton(DAY_MODE_TEXT,
        // 匿名函数 Lambda 表达式作为回调
        [](Ref* sender) {
            // 使用 TransitionFade 过渡效果，切换到 GameWorld 场景
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameWorld::createScene()));
        });

    // 2. 黑夜模式 (占位符)
    auto nightModeItem = createTextButton(NIGHT_MODE_TEXT, nullptr);

    // 3. 商店 (占位符)
    auto shopItem = createTextButton(SHOP_TEXT, nullptr);

    // 4. 退出游戏 (功能按钮) -> 退出程序
    auto exitItem = createTextButton(EXIT_TEXT, CC_CALLBACK_1(GameMenu::menuCloseCallback, this));


    // 创建 Menu
    auto menu = Menu::create(dayModeItem, nightModeItem, shopItem, exitItem, nullptr);
    menu->alignItemsVerticallyWithPadding(VERTICAL_PADDING); // 垂直排列，设置间距

    // 将 Menu 放置在屏幕中央
    Vec2 menuPosition = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    // 适当向上偏移，让菜单更靠近屏幕中心上方，视觉上更舒适
    menuPosition.y += 50.0f;

    menu->setPosition(menuPosition);

    this->addChild(menu, 2); // Z-order 2 (在所有背景之上)

    return true;
}

// =========================================================================
// 3. 退出游戏回调 (menuCloseCallback)
// =========================================================================

/**
 * @brief 退出游戏的回调函数。
 * * @param sender 触发回调的对象
 */
void GameMenu::menuCloseCallback(Ref* sender)
{
    // 停止 Cocos2d-x 引擎的主循环
    Director::getInstance()->end();

    // 针对 iOS 平台，需要显式调用 exit(0)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}