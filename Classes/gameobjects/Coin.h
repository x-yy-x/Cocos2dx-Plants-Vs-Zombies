#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include <string>

/**
 * @brief Coin class for currency collection
 * Can be dropped by defeated zombies or generated through special events
 */
class Coin : public GameObject
{
public:
    /**
     * @brief Defines the classification of the coin and its associated value
     */
    enum class CoinType {
        SILVER,
        GOLD,
        DIAMOND
    };

    /**
     * @brief Initialization function
     * @param coinType The type of coin to initialize (Silver, Gold, or Diamond)
     * @return true if initialization was successful
     */
    bool init(CoinType coinType);

    /**
     * @brief Static create method following Cocos2d-x pattern
     * @param coinType The type of coin to create
     * @return A pointer to the created Coin instance
     */
    static Coin* create(CoinType coinType);

    /**
     * @brief Update function for movement and lifetime tracking
     * @param delta Time elapsed since the last frame
     */
    virtual void update(float delta) override;

    /**
     * @brief Check if the coin is currently in a state where the player can click it
     * @return true if collectible, false otherwise
     */
    bool isCollectible() const;

    /**
     * @brief Collect the coin, trigger animation, and award currency
     * @param onFinished Callback function to execute when the collection logic completes
     */
    void collect(const std::function<void(int)>& onFinished);

    /**
     * @brief Check if the coin object should be purged from the game world
     * @return true if the coin has expired or finished its collection animation
     */
    bool shouldRemove() const;

private:
    Coin();
    virtual ~Coin();

    /**
     * @brief Configures visual animations and sprite frame settings
     */
    void setAnimation();

    // ----------------------------------------------------
    // Static configuration constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME[3]; // Texture paths for each coin type
    static const int COIN_VALUE[3];             // Monetary value per coin type
    static const float LIFETIME;                // Duration before the coin fades out automatically

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    bool is_collected;        // Flag indicating if the coin lifecycle is finished
    bool is_collecting;       // Flag indicating if the collection animation is active
    float life_time;          // Counter for tracking time elapsed since creation
    cocos2d::Vec2 target_pos; // Final destination for falling/jumping trajectory
    float coin_scale;         // Visual scale factor
    CoinType coin_type;       // The specific type of this coin instance
};