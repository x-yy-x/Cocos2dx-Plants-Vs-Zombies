#ifndef __GAMEMENU_H__
#define __GAMEMEMU_H__

#include "cocos2d.h"

class GameMenu : public cocos2d::Scene
{
public:
    GameMenu();
    virtual ~GameMenu();

    virtual bool init() override;

    static cocos2d::Scene* createScene();

    // a selector callback
    void menuCloseCallback(Ref* sender);

    // implement the "static create()" method manually
    CREATE_FUNC(GameMenu);

private:
    int _backgroundMusicId{-1};
};

#endif
