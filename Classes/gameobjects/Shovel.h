#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"

/**
 * @brief Shovel class for removing plants from the grid.
 * Inherits from GameObject.
 * Can be dragged to plant positions to remove them.
 */
class Shovel : public GameObject
{
public:
    /**
     * @brief Shovel initialization function
     */
    virtual bool init() override;

    // Implement the "static create()" method
    CREATE_FUNC(Shovel);

    /**
     * @brief Check if a point is inside the shovel's bounding box
     * @param point Point to check
     * @return true if point is inside, false otherwise
     */
    bool containsPoint(const cocos2d::Vec2& point);

    /**
     * @brief Set shovel to dragging state and follow touch position
     * @param enabled true to enable dragging, false to disable
     */
    void setDragging(bool enabled);

    /**
     * @brief Check if shovel is currently being dragged
     * @return true if dragging, false otherwise
     */
    bool isDragging() const;

    /**
     * @brief Update shovel position to follow touch
     * @param touchPos Touch position in world coordinates
     */
    void updatePosition(const cocos2d::Vec2& touchPos);

    /**
     * @brief Reset shovel to its original position (center of shovel back)
     */
    void resetPosition();

    /**
     * @brief Set the original position (center of shovel back)
     * @param pos Original position
     */
    void setOriginalPosition(const cocos2d::Vec2& pos);

protected:
    // Protected constructor
    Shovel();

    // Virtual destructor
    virtual ~Shovel();

    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const cocos2d::Rect INITIAL_PIC_RECT;
    static const cocos2d::Size OBJECT_SIZE;

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    bool _isDragging;                    // Is currently being dragged
    cocos2d::Vec2 _originalPosition;     // Original position (center of shovel back)
};

