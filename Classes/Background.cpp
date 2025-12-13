#include "BackGround.h"
#include "GameDefs.h"
#include "cocos2d.h" // ȷ������ cocos2d ͷ�ļ�

USING_NS_CC;

// =================================================================
// BackGround class static constant definitions
// =================================================================
const std::string BackGround::DAY_IMAGE = "background.png";
const std::string BackGround::NIGHT_IMAGE = "background2.png";

// =================================================================
// SeedBank Sprite ���ò����������޸ģ�
// =================================================================
namespace
{
    // X ����ƫ�ƣ������ԭ�㿪ʼ�ľ��룬ռ��Ļ���ȵı�����
    const float SEED_BANK_POS_X_RATIO = 0.245f;

    // Y ����ƫ�ƣ��Ӷ�����ʼ�ľ��룬ռ��Ļ�߶ȵı�����1.0f �Ƕ�����0.0f �ǵײ���
    const float SEED_BANK_POS_Y_RATIO = 0.96f;

    // �Ŵ���
    const float SEED_BANK_SCALE_FACTOR = 1.04f;

    // ͼƬ�ļ���
    const std::string SEED_BANK_IMAGE = "seedBank.png";
}


BackGround* BackGround::create()
{
    return create(false);
}

BackGround* BackGround::create(bool isNightMode)
{
    BackGround* bg = new (std::nothrow) BackGround();
    if (bg)
    {
        bg->_isNightMode = isNightMode;
        if (bg->init())
        {
            bg->autorelease();
            return bg;
        }
        delete bg;
    }
    return nullptr;
}

bool BackGround::init()
{
    const std::string& imageFile = _isNightMode ? NIGHT_IMAGE : DAY_IMAGE;
    if (!Sprite::initWithFile(imageFile))
    {
        CCLOG("Failed to load background image: %s", imageFile.c_str());
        return false;
    }

    // 2. Get visible area and origin
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 3. Set background position to screen center
    this->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    // 4. Get background image size and set scaling to fill screen
    auto bgSize = this->getContentSize();
    float scaleX = visibleSize.width / bgSize.width;
    float scaleY = visibleSize.height / bgSize.height;
    this->setScale(scaleX, scaleY);

    // =================================================================
    // 5. ���� SeedBank Sprite ��������
    // =================================================================
    auto seedBankSprite = Sprite::create(SEED_BANK_IMAGE);

    if (seedBankSprite)
    {
        // ���� SeedBank ������λ��
        // X: ��Ļ���� * ���� + ԭ��X
        float finalX = visibleSize.width * SEED_BANK_POS_X_RATIO + origin.x;

        // Y: ��Ļ�߶� * ���� + ԭ��Y
        float finalY = visibleSize.height * SEED_BANK_POS_Y_RATIO + origin.y;

        seedBankSprite->setPosition(Vec2(finalX, finalY));

        // ���÷Ŵ���
        seedBankSprite->setScale(SEED_BANK_SCALE_FACTOR);

        // Z-order: ȷ�����ڱ���֮��
        // ע�⣺�� seedBankSprite ����Ϊ BackGround �������ӽڵ� (this)
        this->addChild(seedBankSprite, 1);

        CCLOG("SeedBank Sprite initialized successfully.");
    }
    else
    {
        CCLOG("Error: Failed to load seed bank image: %s", SEED_BANK_IMAGE.c_str());
    }
    // =================================================================

    CCLOG("BackGround initialized successfully.");
    return true;
}