#ifndef __SHOVEL_H__
#define __SHOVEL_H__

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"

/**
 * @brief Shovel tool used to remove existing plants from the grid.
 * Inherits from GameObject. It can be picked up from the UI bank and
 * dragged over plants to trigger their removal.
 */
class Shovel : public GameObject
{
public:
    /**
     * @brief Initializes the shovel sprite and its interaction properties.
     * @return true if initialization was successful.
     */
    virtual bool init() override;

    // Standard Cocos2d-x macro for creating the instance
    CREATE_FUNC(Shovel);

    /**
     * @brief Checks if a world-space point falls within the shovel's current hit box.
     * @param point The world position to check.
     * @return true if the point is inside the bounding box.
     */
    bool containsPoint(const cocos2d::Vec2& point);

    /**
     * @brief Toggles the dragging visual state (scaling/opacity).
     * @param enabled True to enter dragging mode, false to return to idle.
     */
    void setDragging(bool enabled);

    /**
     * @brief Returns whether the shovel is currently active in the user's hand.
     */
    bool isDragging() const;

    /**
     * @brief Updates the shovel's position to follow the user's finger or cursor.
     * @param touchPos The target world coordinates.
     */
    void updatePosition(const cocos2d::Vec2& touchPos);

    /**
     * @brief Returns the shovel to its home slot in the UI.
     */
    void resetPosition();

    /**
     * @brief Defines the home position for the shovel (typically the center of the shovel UI slot).
     * @param pos The global coordinates of the UI slot.
     */
    void setOriginalPosition(const cocos2d::Vec2& pos);

private:
    Shovel();
    virtual ~Shovel();

    // ----------------------------------------------------
    // Static configuration constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    bool is_dragging;                // Tracking flag for movement state
    cocos2d::Vec2 original_position; // Cached home position for the UI bank
};

#endif // __SHOVEL_H__