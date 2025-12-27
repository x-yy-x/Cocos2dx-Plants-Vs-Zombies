#ifndef __GAMEMENU_H__
#define __GAMEMEMU_H__

#include "cocos2d.h"

/**
 * @brief The main menu scene of the game.
 * Handles navigation to Day mode, Night mode, and the Shop.
 */
class GameMenu : public cocos2d::Scene
{
public:
    GameMenu();
    virtual ~GameMenu();

    /** @brief Initializes the menu, background, and buttons. */
    virtual bool init() override;

    /** @brief Static helper to create the scene. */
    static cocos2d::Scene* createScene();

    /** @brief Callback for the exit button to close the game. */
    void menuCloseCallback(Ref* sender);

    // Standard Cocos2d-x macro to implement the static create() method
    CREATE_FUNC(GameMenu);

private:
    int background_music_id{ -1 }; // Handle for title background music
};

#endif