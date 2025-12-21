#pragma once
#include "cocos2d.h"
#include "GameDefs.h"
#include <vector>

class SeedPacket;
class SelectCard;

class SelectCardsScene : public cocos2d::Scene
{
private:
    bool _isNightMode;
    cocos2d::Node* _worldLayer;
    cocos2d::Node* _uiLayer;
    cocos2d::Sprite* _background;
    cocos2d::Menu* _backMenu;
    cocos2d::Sprite* _selectBG;
    cocos2d::Node* _zombieShowLayer;
    float _moveDuration;
    float _moveOffsetX;
    float _selectBGPosXRatio;
    float _selectBGPosYRatio;
    float _selectBGScale;
    float _zShowStartX;
    float _zShowStartY;
    float _zShowGapX;
    float _zShowGapY;
    
    // Card selection
    std::vector<SelectCard*> _allSelectCards;
    std::vector<SelectCard*> _selectedCards;  // Store SelectCard* instead of SeedPacket*
    std::vector<PlantName> _selectedPlantNames;  // Store PlantName instead of SeedPacket* for safety
    cocos2d::Node* _selectedCardsContainer;
    std::vector<cocos2d::Sprite*> _displaySprites;  // Store display sprites in seedBank for click handling

    // Audio & transition state (no gameplay changes)
    int _selectBgmId = -1;
    int _readySfxId = -1;
    bool _isTransitioning = false;

public:
    static SelectCardsScene* createScene(bool isNightMode);
    static SelectCardsScene* create(bool isNightMode);
    virtual bool init() override;
    void setMoveDuration(float d);
    void setMoveOffsetX(float x);
    void setSelectBGPosRatio(float xr, float yr);
    void setSelectBGScale(float s);

protected:
    SelectCardsScene();
    virtual ~SelectCardsScene();
    void buildWorld();
    void buildUI();
    void runIntroMove();
    void showSelectBG();
    void spawnZombieShowcase();
    void createSelectCards();
    void onCardSelected(SelectCard* card);
    void updateSelectedCardsDisplay();

    // Ready-Set-Plant sequence
    void playReadySetPlantSequence();
    void maybeGoGame();
};
