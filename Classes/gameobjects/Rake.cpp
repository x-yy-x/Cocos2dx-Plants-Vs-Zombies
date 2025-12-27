#include "Rake.h"
#include "Zombie.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

// ---------------------------------------------------------
// Factory Method
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// Initialization
// ---------------------------------------------------------
bool Rake::init()
{
    // Load the visual asset for the rake
    if (!Sprite::initWithFile("rake.png"))
    {
        return false;
    }

    used = false;

    // Apply visual adjustments for the "lying on ground" look
    this->setScale(1.2f);
    this->setRotation(-45.0f);

    return true;
}

// ---------------------------------------------------------
// Logic Implementation
// ---------------------------------------------------------
void Rake::trigger(Zombie* z)
{
    // Ensure the trap only triggers once
    if (used) return;
    used = true;

    // Play feedback sound (the "bonk" when the handle hits the zombie)
    cocos2d::AudioEngine::play2d("bonk.mp3", false, 1.0f);

    // Visual sequence: The rake flips up and then removes itself
    auto rotateUp = RotateBy::create(0.12f, 49.0f);
    auto removeAction = RemoveSelf::create();

    this->runAction(Sequence::create(rotateUp, removeAction, nullptr));

    CCLOG("Rake trap triggered by zombie.");
}

cocos2d::Rect Rake::getBoundingBox() const
{
    // Calculate a custom hit-area based on scale and content size
    // Note: The positioning offset depends on the anchor point (default 0.5, 0.5)
    float width = this->getContentSize().width * this->getScale();
    float height = this->getContentSize().height * this->getScale();

    return Rect(
        this->getPositionX() - width,
        this->getPositionY() - height,
        width * 0.5f,
        height * 0.5f
    );
}