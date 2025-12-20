#include "Rake.h"
#include "Zombie.h"

USING_NS_CC;

Rake* Rake::create()
{
    Rake* r = new (std::nothrow) Rake();
    if (r && r->init())
    {
        r->autorelease();
        return r;
    }
    CC_SAFE_DELETE(r);
    return nullptr;
}

bool Rake::init()
{
    if (!Sprite::initWithFile("rake.png"))
        return false;
    _used = false;
    // 可根据美术需要调整缩放
    this->setScale(0.8f);
    this->setRotation(-45);
    return true;
}

void Rake::trigger(Zombie* z)
{
    if (_used) return;
    _used = true;
    // 旋转 90 度并消失
    auto rot = RotateBy::create(0.12f, 49.0f);
    auto rm = RemoveSelf::create();
    this->runAction(Sequence::create(rot, rm, nullptr));
}

cocos2d::Rect Rake::getBoundingBox() const  {
return Rect(this->getPositionX() - this->getContentSize().width * this->getScale() / 2,
	this->getPositionY() - this->getContentSize().height * this->getScale() / 2,
	this->getContentSize().width * this->getScale() * 0.5,
	this->getContentSize().height * this->getScale() * 0.5);
}
