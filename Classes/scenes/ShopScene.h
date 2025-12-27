#pragma once

#include "cocos2d.h"
#include "GameDefs.h"

/**
 * @brief Scene representing Crazy Dave's shop.
 * Players can purchase permanent upgrades (Mowers/Rakes) and advanced plant cards.
 */
class ShopScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;

    CREATE_FUNC(ShopScene);

private:
    /** @brief Unique identifiers for shop items to facilitate purchase logic. */
    enum class ShopItemID {
        MOWER,
        RAKE,
        TWIN_SUNFLOWER,
        GATLING_PEA,
        SPIKE_ROCK,
        UNKNOWN
    };

    void setupUI();             // Initialize background, buttons, and coin display
    void setupDaveAnimation();  // Create and play Dave's idling/speaking animations
    void setupShopItems();      // Define and place item shelves

    /** * @brief Factory method to create an interactable shop item.
     * @param plantName Optional parameter; used only for seed packet items.
     */
    void createShopItem(const std::string& image, ShopItemID itemId, int price, const cocos2d::Vec2& position, PlantName plantName = PlantName::UNKNOWN);

    void updateCoinLabel();     // Refresh the displayed coin balance from PlayerProfile

    // --- UI Callbacks ---
    void onBackToMenu(cocos2d::Ref* sender);

    cocos2d::Label* coin_label{ nullptr };
    cocos2d::Sprite* dave_sprite{ nullptr };
};