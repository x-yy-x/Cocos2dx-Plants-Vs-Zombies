#pragma once

#include "cocos2d.h"
#include "GameDefs.h"

/**
 * @brief Base class for all interactive objects displayed on screen.
 * Inherits from cocos2d::Sprite to provide common game logic and rendering.
 * Can be extended with update logic (override update function if needed).
 */
class GameObject : public cocos2d::Sprite
{
public:
    // 1. Use CREATE_FUNC macro to create instances
    CREATE_FUNC(GameObject);

    // 2. Virtual destructor to ensure proper cleanup in derived classes
    virtual ~GameObject();

    /**
     * @brief Initialization function inherited from Sprite.
     * Attempts to initialize using static constants defined by subclasses
     * (IMAGE_FILENAME and INITIAL_PIC_RECT).
     */
    virtual bool init() override;

protected:
    // Protected constructor to prevent direct instantiation (use CREATE_FUNC macro)
    GameObject();

    // ----------------------------------------------------
    // Static constants: subclasses should define these
    // ----------------------------------------------------

    // Default image filename. Subclasses should define and implement in their .cpp file.
    static const std::string IMAGE_FILENAME;

    // Default initial image rectangle for extracting from sprite sheet.
    static const cocos2d::Rect INITIAL_PIC_RECT;

    // Object's standard size. Used for collision detection or layout.
    static const cocos2d::Size OBJECT_SIZE;
    

    // ----------------------------------------------------
    // Protected member data
    // ----------------------------------------------------
    cocos2d::Vec2 pos;
};
