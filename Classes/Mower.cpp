#include "Mower.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

Mower* Mower::create()
{
    Mower* m = new (std::nothrow) Mower();
    if (m && m->init())
    {
        m->autorelease();
        return m;
    }
    CC_SAFE_DELETE(m);
    return nullptr;
}

bool Mower::init()
{
    if (!Sprite::initWithFile("mower.png"))
        return false;
    _moving = false;
    return true;
}

void Mower::start()
{
    if (_moving) return;
    _moving = true;
    cocos2d::AudioEngine::play2d("Lawnmower.ogg", false, 1.0f);
    auto vs = Director::getInstance()->getVisibleSize();
    float dx = vs.width + 300.0f; // 充足的距离，越界后由 GameWorld 清理
    float speed = 900.0f;         // 像素/秒
    float dur = dx / speed;
    this->runAction(MoveBy::create(dur, Vec2(dx, 0)));
}

