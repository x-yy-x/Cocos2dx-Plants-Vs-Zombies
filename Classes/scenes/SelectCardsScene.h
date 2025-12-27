#pragma once
#include "cocos2d.h"
#include "GameDefs.h"
#include <vector>

class SeedPacket;
class SelectCard;

/**
 * @brief Scene for selecting plant cards before starting a level.
 * Handles the camera pan animation, zombie preview, and deck building logic.
 */
class SelectCardsScene : public cocos2d::Scene
{
public:
    /**
     * @brief Creates the SelectCardsScene with specific environment settings.
     * @param isNightMode True for night levels, false for day levels.
     */
    static SelectCardsScene* createScene(bool isNightMode);
    static SelectCardsScene* create(bool isNightMode);

    virtual bool init() override;

private:
    // --- Layout and Animation Configuration ---
    void setMoveDuration(float d);
    void setMoveOffsetX(float x);
    void setSelectBGPosRatio(float xr, float yr);
    void setSelectBGScale(float s);

    SelectCardsScene();
    virtual ~SelectCardsScene();

    // --- Scene Initialization Steps ---
    void buildWorld();          // Setup background and environment layers
    void buildUI();             // Setup buttons and basic UI elements
    void runIntroMove();        // Camera pan animation to preview the lawn
    void showSelectBG();        // Displays the card selection wooden board
    void spawnZombieShowcase(); // Spawns preview zombies on the right side of the lawn

    // --- Card Selection Logic ---
    void createSelectCards();           // Populates the board with available plants
    void onCardSelected(SelectCard* card); // Callback when a plant card is clicked
    void updateSelectedCardsDisplay();  // Refreshes the top "Seed Bank" UI bar

    // --- Level Transition Sequence ---
    void playReadySetPlantSequence();   // Plays the "Ready, Set, Plant!" animation
    void maybeGoGame();                 // Transitions to the GameWorld scene

    // --- Environment and Layers ---
    bool is_night_mode;
    cocos2d::Node* world_layer;         // Layer containing the background and zombies
    cocos2d::Node* ui_layer;            // Layer containing UI and menus
    cocos2d::Sprite* background;
    cocos2d::Menu* back_menu;
    cocos2d::Sprite* selectBG;          // The card selection board sprite
    cocos2d::Node* zombie_show_layer;   // Container for preview zombies

    // --- Visual Configuration Values ---
    float move_duration;                // Duration of the camera pan
    float move_offsetX;                 // Horizontal distance for the camera pan
    float selectBG_posX_ratio;          // Relative X position for the selection board
    float selectBG_posY_ratio;          // Relative Y position for the selection board
    float selectBG_scale;               // Scale factor for the selection board
    float zShow_startX;                 // Starting X for zombie preview
    float zShow_startY;                 // Starting Y for zombie preview
    float zShow_gapX;                   // Horizontal gap between preview zombies
    float zShow_gapY;                   // Vertical gap between preview zombies

    // --- Card Selection State ---
    std::vector<SelectCard*> all_select_cards;       // All cards available on the board
    std::vector<SelectCard*> selected_cards;         // Pointers to currently chosen cards
    std::vector<PlantName> selected_plant_names;     // Names of selected plants passed to gameplay
    cocos2d::Node* selected_cards_container;         // The top bar (Seed Bank) node
    std::vector<cocos2d::Sprite*> display_sprites;   // Visual representations in the Seed Bank

    // --- Audio and Scene State ---
    bool can_select_cards = true;       // Prevents clicking during animations
    int select_BgmId = -1;              // Handle for the background music
    int readySfxId = -1;                // Handle for the transition sound effects
    bool is_transitioning = false;      // Prevents multiple scene transitions
};