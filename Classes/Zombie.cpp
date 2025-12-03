#include "Zombie.h"

Zombie::~Zombie()
{
    CC_SAFE_RELEASE(_walkAction);
    CC_SAFE_RELEASE(_eatAction);
}

bool Zombie::init()
{
    if (!Sprite::init())
        return false;
    scheduleUpdate();
    return true;
}

Zombie* Zombie::createZombie()
{
    Zombie* z = new Zombie();

    if (z && z->init())
    {
        z->autorelease();

        z->initWalkAnimation();
        z->initEatAnimation();
        z->runAction(z->_walkAction);

        return z;
    }

    delete z;
    return nullptr;
}

void Zombie::initWalkAnimation()
{
    const float frameWidth = 125;
    const float frameHeight = 173.8;

    cocos2d::Vector<cocos2d::SpriteFrame*> frames;

    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == 4 && col == 6)
                break;
            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = cocos2d::SpriteFrame::create(
                "zombie_walk_spritesheet.png",
                cocos2d::Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.05f);
    auto animate = cocos2d::Animate::create(animation);

    this->_walkAction = cocos2d::RepeatForever::create(animate);
    _walkAction->retain();
}

void Zombie::initEatAnimation()
{
    const float frameWidth = 125;
    const float frameHeight = 173.8;

    cocos2d::Vector<cocos2d::SpriteFrame*> frames;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == 3 && col == 9)
                break;
            float x = col * frameWidth;
            float y = row * frameHeight;

            auto frame = cocos2d::SpriteFrame::create(
                "zombie_eat_spritesheet.png",
                cocos2d::Rect(x, y, frameWidth, frameHeight)
            );

            frames.pushBack(frame);
        }
    }

    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.03f);
    auto animate = cocos2d::Animate::create(animation);

    this->_eatAction = cocos2d::RepeatForever::create(animate);
    _eatAction->retain();
}

void Zombie::setPlantList(std::vector<cocos2d::Sprite*>*p)
{
    _plants = p;
}

void Zombie::update(float dt)
{
    // 如果僵尸没有在吃，继续往左走
    if (!_isEating)
    {
        float newX = this->getPositionX() - _speed * dt;
        this->setPositionX(newX);
    }
    CCLOG("plant count = %d", this->_plants->size());
    // 每帧检查是否撞到植物
    checkCollision();
    CCLOG("_eatAction = %p", _eatAction);
}

void Zombie::checkCollision()
{
    if (_isEating) return;

    for (auto plant : *_plants)
    {
        if (this->getBoundingBox().intersectsRect(plant->getBoundingBox()))
        {
            startEating(plant);
            return;
        }
    }
}

void Zombie::startEating(cocos2d::Sprite* plant)
{
    _isEating = true;
    _targetPlant = plant;
    _speed = 0;  // 停止移动

    this->stopAction(_walkAction);
    this->runAction(_eatAction);

    CCLOG("Zombie start eating plant!");
}

void Zombie::onPlantDied()
{
    _isEating = false;
    _speed = _normalSpeed;
    _targetPlant = nullptr;

    this->stopAction(_eatAction);
    this->runAction(_walkAction);

    CCLOG("Zombie resume walking");
}