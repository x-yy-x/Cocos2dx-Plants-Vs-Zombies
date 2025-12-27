#ifndef __RAKE_H__
#define __RAKE_H__

#include "cocos2d.h"
#include "GameObject.h"

class Zombie;

/**
 * @brief Represents a single-use trap that can be placed on the field.
 * When a zombie steps on the rake, it triggers a "bonk" animation and deals fatal damage
 * (handled in GameWorld logic) before removing itself.
 */
class Rake : public GameObject
{
public:
    /**
     * @brief Static factory method to create a Rake instance.
     * @return Pointer to the created Rake object.
     */
    static Rake* create();

    /**
     * @brief Initializes the Rake sprite and initial state.
     * @return true if initialization was successful.
     */
    virtual bool init() override;

    /**
     * @brief Activates the trap, playing the bonk animation and sound.
     * @param z Pointer to the zombie that triggered the rake.
     */
    void trigger(Zombie* z);

    /**
     * @brief Checks if the rake has already been triggered.
     * @return true if used, false otherwise.
     */
    bool isUsed() const { return used; }

    /**
     * @brief Custom bounding box calculation for precise collision detection.
     * @return The calculated Rect in world space.
     */
    virtual cocos2d::Rect getBoundingBox() const override;

private:
    bool used{ false }; // Activation state flag
};

#endif // __RAKE_H__