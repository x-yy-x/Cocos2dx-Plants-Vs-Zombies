#ifndef __MOWER_H__
#define __MOWER_H__

#include "cocos2d.h"
#include "GameObject.h"

/**
 * @brief The Lawnmower class represents the last line of defense in a row.
 * When triggered by a zombie, it moves across the screen, destroying all enemies in its path.
 */
class Mower : public GameObject
{
public:
    /**
     * @brief Static factory method to create a Mower instance.
     * @return Pointer to the created Mower object.
     */
    static Mower* create();

    /**
     * @brief Initializes the Mower sprite and state.
     * @return true if initialization was successful.
     */
    virtual bool init() override;

    /**
     * @brief Activates the mower, playing audio and moving it across the screen.
     */
    void start();

    /**
     * @brief Checks if the mower is currently active and moving.
     * @return true if moving, false otherwise.
     */
    bool isMoving() const { return moving; }

private:
    bool moving{ false }; // Movement state flag
};

#endif // __MOWER_H__