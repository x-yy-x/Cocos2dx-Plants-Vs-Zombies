#pragma once
#include "cocos2d.h"
#include "GameObject.h"

/**
 * @class BackGround
 * @brief Handles the game world's background layer and static UI elements like the Seed Bank.
 * Inherits from GameObject and supports switching between day and night visual modes.
 */
class BackGround : public GameObject
{
public:
    /**
     * @brief Creates a default background (Day Mode).
     * @return A pointer to the created BackGround instance.
     */
    static BackGround* create();

    /**
     * @brief Creates a background with a specified time mode.
     * @param isNightMode True for Night Mode, false for Day Mode.
     * @return A pointer to the created BackGround instance.
     */
    static BackGround* create(bool isNightMode);

    /**
     * @brief Initializes the background sprite and adds the Seed Bank overlay.
     * @return true if initialization was successful.
     */
    virtual bool init() override;

    /**
     * @brief Manually sets the night mode flag.
     * @param isNightMode The time mode to be applied.
     */
    void setNightMode(bool isNightMode)
    {
        is_night_mode = isNightMode;
    }

private:
    /**
     * @brief Private constructor using member initializer list.
     */
    BackGround();

    // ----------------------------------------------------
    // Static Configuration Constants
    // ----------------------------------------------------
    static const std::string DAY_IMAGE;   // Filename for day background
    static const std::string NIGHT_IMAGE; // Filename for night background

    // ----------------------------------------------------
    // Member Variables
    // ----------------------------------------------------
    bool is_night_mode; // Flag to determine which background texture to load
};