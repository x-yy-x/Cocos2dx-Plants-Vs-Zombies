#pragma once

#include "cocos2d.h"
#include "Plant.h"
#include "GameDefs.h"
#include "Pea.h"

class Wallnut : public Plant
{
public:
    /**
     * @brief PeaShooter initialization function
     */
    virtual bool init() override;

    enum class WallnutState
    {
        NORMAL,
        CRACKED
    };

    // Implement the auto-generated static PeaShooter* create() function
    CREATE_FUNC(Wallnut);

    /**
     * @brief Static planting function for PeaShooter.
     * @param globalPos Touch position in global coordinates
     * @return PeaShooter* Returns PeaShooter instance on success, nullptr on failure
     */
    static Wallnut* plantAtPosition(const cocos2d::Vec2& globalPos);

    /**
     * @brief Override update function
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    //WallnutState getState() const;

    //void setState(WallnutState newState);

protected:
    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;
    static const float ATTACK_RANGE;  // PeaShooter's attack range

    cocos2d::RepeatForever* normalAnimation;
    cocos2d::RepeatForever* crackedAnimation;

    WallnutState _currentState;

    /**
     * @brief Set up animation frames
     */
    virtual void setAnimation() override;

    void setCrackedAnimation();

    // Protected constructor
    Wallnut();
};
