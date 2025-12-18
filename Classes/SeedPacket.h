#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"

// Forward declaration
class Plant;

/**
 * @brief SeedPacket base class for plant seed cards
 * Manages cooldown, sun cost, and plant creation
 */
class SeedPacket : public GameObject
{
public:
    /**
     * @brief Template-based factory method to create seed packet for any plant type
     * Eliminates the need for separate SeedPacket subclasses
     * 
     * Usage:
     *   auto packet = SeedPacket::create<Sunflower>("seedpacket_sunflower.png", 3.0f, 50);
     * 
     * @param imageFile Seed packet image filename
     * @param cooldownTime Cooldown time in seconds
     * @param sunCost Sun cost to plant
     * @return Pointer to created seed packet, or nullptr if failed
     */
    template<typename PlantType>
    static SeedPacket* create(const std::string& imageFile, float cooldownTime, int sunCost,PlantName plantName)
    {
        // Internal template class for specific plant type
        class SeedPacketImpl : public SeedPacket
        {
        public:
            SeedPacketImpl(const std::string& imageFile, float cooldownTime, int sunCost, PlantName plantName)
            {
                _seedPacketImage = imageFile;
                _cooldownTime = cooldownTime;
                _sunCost = sunCost;
                _plantName = plantName;
                _isOnCooldown = false;
                _accumulatedTime = 0.0f;
            }

            virtual Plant* plantAt(const cocos2d::Vec2& globalPos) override
            {
                return PlantType::plantAtPosition(globalPos);
            }

            virtual Plant* createPreviewPlant() override
            {
                auto preview = PlantType::create();
                if (preview)
                {
                    preview->setOpacity(128); // 50% transparent
                }
                return preview;
            }
        };

        auto packet = new (std::nothrow) SeedPacketImpl(imageFile, cooldownTime, sunCost,plantName);
        if (packet && packet->init())
        {
            packet->autorelease();
            return packet;
        }
        delete packet;
        return nullptr;
    }

    /**
     * @brief Initialization function
     */
    virtual bool init() override;

    /**
     * @brief Update function for cooldown animation
     * @param delta Time delta
     */
    virtual void update(float delta) override;

    /**
     * @brief Check if the seed packet is ready (not on cooldown)
     * @return true if ready, false if cooling down
     */
    bool isReady() const;

    /**
     * @brief Start cooldown
     */
    void startCooldown();

    /**
     * @brief Get sun cost
     * @return Sun cost value
     */
    int getSunCost() const;

    /**
     * @brief Create and return a plant at given position (to be overridden by subclasses)
     * @param globalPos Position to plant
     * @return Pointer to created plant, or nullptr if failed
     */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) = 0;

    /**
     * @brief Create a preview plant (transparent, non-interactive)
     * @return Pointer to preview plant
     */
    virtual Plant* createPreviewPlant() = 0;

    PlantName getPlantName();

protected:
    // Protected constructor
    SeedPacket();

    // Virtual destructor
    virtual ~SeedPacket();

    /**
     * @brief Update cooldown visual effect (grayscale to color)
     */
    void updateCooldownEffect();

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    float _cooldownTime;         // Total cooldown time in seconds
    float _accumulatedTime;      // Time accumulated since cooldown started
    int _sunCost;                // Sun cost to plant
    bool _isOnCooldown;          // Is currently cooling down
    PlantName _plantName;
    // Image filename (to be set by subclasses)
    std::string _seedPacketImage;
};
