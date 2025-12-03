/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "Zombie.h"
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    CCLOG("Init started!");
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();

    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //初始写死，后续需要修改
    _cellSize.setSize(130, 120);
    _gridOrigin = origin + Vec2(10, 10);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("background.png");
    auto sprite_seedpacket_peashooter = Sprite::create("seedpacket_peashooter.png");
    if (sprite == nullptr)
    {
        problemLoading("'background.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));


        auto bgSize = sprite->getContentSize();


        float scaleX = visibleSize.width / bgSize.width;
        float scaleY = visibleSize.height / bgSize.height;

        sprite->setScale(scaleX, scaleY);

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

    if (sprite_seedpacket_peashooter == nullptr)
    {
        problemLoading("'seedpacket_peashooter.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite_seedpacket_peashooter->setPosition(Vec2(187,667));


        auto bgSize = sprite_seedpacket_peashooter->getContentSize();




        // add the sprite as a child to this layer
        this->addChild(sprite_seedpacket_peashooter, 0);
    }

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this, sprite_seedpacket_peashooter](Touch* touch, Event* event) {
        Vec2 pos = this->convertToNodeSpace(touch->getLocation());

        CCLOG("touch at: %f, %f", pos.x, pos.y);

        if (sprite_seedpacket_peashooter->getBoundingBox().containsPoint(pos)) {
            this->_plantSelected = true;
            CCLOG("peashooter card selected");
            return true;
        }
        return false; // 返回 false：不拦截事件
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto touchListener = EventListenerTouchOneByOne::create();

    // 点击开始
    touchListener->onTouchBegan = [](Touch* touch, Event* event) {
        return true;   // 必须 return true 才能接收到 touch ended
        };

    // 点击结束
    touchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        Vec2 pos = touch->getLocation();

        // 如果当前没有选植物 → 什么都不做
        if (!this->_plantSelected)
            return;

        CCLOG("Touch at: %f, %f", pos.x, pos.y);

        // 种植物
        this->plantAtPosition(pos);

        // 重置选择状态
        this->_plantSelected = false;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // 1. 创建一个僵尸
    auto zombie = Zombie::createZombie();

    // 2. 设置僵尸初始位置（从右边走进来）
    zombie->setPosition(Vec2(1300, 120));  // 根据你的草坪高度调整

    // 3. 把植物列表告诉僵尸（非常重要）
    zombie->setPlantList(&_plant);

    // 4. 把僵尸加入场景
    this->addChild(zombie, 5);

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::plantAtPosition(const Vec2& pos)
{
    int col = (pos.x - _gridOrigin.x) / _cellSize.width;
    int row = (pos.y - _gridOrigin.y) / _cellSize.height;

    CCLOG("row=%d col=%d", row, col);

    // 防止越界（可以根据你的草地排布调整）
    if (col < 0 || col >= 9 || row < 0 || row >= 5) {
        CCLOG("invalid grid cell");
        return;
    }

    // 3. 计算网格中心点的像素位置
    float centerX = _gridOrigin.x + col * _cellSize.width + _cellSize.width * 0.5f;
    float centerY = _gridOrigin.y + row * _cellSize.height + _cellSize.height * 0.5f;

    int dx = 30, dy = 8;
    Vec2 plantPos(centerX + dx, centerY + dy);

    // 4. 创建一个豌豆射手 sprite
    auto plant = Sprite::create("peashooter_spritesheet.png", Rect(0, 0, 100, 100));
    _plant.push_back(plant);
    plant->setPosition(plantPos);
    this->addChild(plant);

    // 加动画
    addPeashooterAnimation(plant);
    CCLOG("planted peashooter at row=%d, col=%d", row, col);
}

void HelloWorld::addPeashooterAnimation(Sprite* sprite)
{
    const float sheetWidth = 512.0f;
    const float sheetHeight = 512.0f;

    const int cols = 6;
    const int rows = 4;

    const float frameWidth = 100;   // ≈ 85.333
    const float frameHeight = 100;// = 128

    Vector<SpriteFrame*> frames;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = SpriteFrame::create(
                "Peashooter_spritesheet.png",
                Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames,0.07f);
    auto animate = Animate::create(animation);

    sprite->runAction(RepeatForever::create(animate));
}






