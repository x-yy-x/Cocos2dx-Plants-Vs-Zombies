#ifndef __SUN_H__
#define __SUN_H__

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"

/**
 * @brief Represents the primary resource in the game.
 * Suns can fall from the sky or be produced by specific plants like Sunflowers.
 */
class Sun : public GameObject
{
public:
    /**
     * @brief Standard Cocos2d-x initialization.
     * @return true if initialization was successful.
     */
    virtual bool init() override;

    // Standard factory macro
    CREATE_FUNC(Sun);

    /**
     * @brief Spawns a sun produced by a Sunflower.
     * @param pos The global position where the sun originates (Sunflower's location).
     * @return A pointer to the newly created Sun instance.
     */
    static Sun* createFromSunflower(const cocos2d::Vec2& pos);

    /**
     * @brief Spawns a custom sun with specific scaling and resource value.
     * Useful for large suns or variations like Sun-shroom production.
     * @param pos Origin position.
     * @param scale Visual scale factor.
     * @param value Resource points awarded upon collection.
     */
    static Sun* createCustomSun(const cocos2d::Vec2& pos, float scale, int value);

    /**
     * @brief Spawns a sun that falls vertically from the sky.
     * @param targetGridCol The grid column index where the sun will land.
     * @param startY The starting vertical coordinate (usually above the visible screen).
     */
    static Sun* createFromSky(int targetGridCol, float startY);

    /**
     * @brief Per-frame logic for falling movement and lifetime tracking.
     */
    virtual void update(float delta) override;

    /**
     * @brief Checks if the sun is currently interactive.
     * @return true if it can be clicked, false if already being collected.
     */
    bool isCollectible() const;

    /**
     * @brief Triggers the collection sequence (movement toward UI and sound).
     * @param onfinish Callback to update the player's total sun count.
     */
    void collect(const std::function<void(int)>& onfinish);

    /**
     * @brief Returns whether the sun should be removed by the GameWorld.
     * @return true if collected or expired.
     */
    bool shouldRemove() const;

private:
    Sun();
    virtual ~Sun();

    /**
     * @brief Configures and starts the rotating sprite animation.
     */
    void setAnimation();

    // ----------------------------------------------------
    // Static configuration constants
    // ----------------------------------------------------
    static const std::string IMAGE_FILENAME;
    static const int SUN_VALUE;           // Default resource value (25)
    static const float FALL_SPEED;        // Speed for sky-drop suns
    static const float LIFETIME;          // Seconds before the sun expires

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    bool is_collected;        // Flag for finished collection sequence
    bool is_collecting;       // Flag for active collection animation
    bool is_falling;          // Flag for sky-to-ground movement
    float life_time;          // Timer tracking duration on field
    cocos2d::Vec2 target_pos; // Landing coordinates for sky suns
    float sun_scale;          // Custom size multiplier
    int sun_value;            // Resource points awarded for this instance
};

#endif // __SUN_H__