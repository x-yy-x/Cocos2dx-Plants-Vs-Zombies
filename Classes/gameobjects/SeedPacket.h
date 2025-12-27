#ifndef __SEED_PACKET_H__
#define __SEED_PACKET_H__

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include <map>
#include <string>

// Forward declaration
class Plant;

/**
 * @brief SeedPacket base class for plant seed cards.
 * Manages cooldown timers, sun costs, and provides factory methods for plant creation.
 */
class SeedPacket : public GameObject
{
public:
    /**
     * @brief Template-based factory method to create seed packets for any plant type.
     * Uses an internal implementation class to link the packet to a specific Plant class.
     * * @param imageFile The texture path for the seed packet.
     * @param cooldownTime Duration of the recovery period in seconds.
     * @param sunCost The amount of sun required to use this packet.
     * @param plantName Enum identifier for the plant.
     * @return Pointer to the created SeedPacket instance.
     */
    template<typename PlantType>
    static SeedPacket* create(const std::string& imageFile, float cooldownTime, int sunCost, PlantName plantName)
    {
        // Internal implementation class to handle specific plant instantiation logic
        class SeedPacketImpl : public SeedPacket
        {
        public:
            SeedPacketImpl(const std::string& imageFile, float cooldownTime, int sunCost, PlantName plantName)
            {
                seed_packet_image = imageFile;
                cooldown_time = cooldownTime;
                sun_cost = sunCost;
                plant_name = plantName;
                is_on_cooldown = false;
                accumulated_time = 0.0f;
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
                    // 50% transparency for the placement preview
                    preview->setOpacity(128);
                }
                return preview;
            }
        };

        auto packet = new (std::nothrow) SeedPacketImpl(imageFile, cooldownTime, sunCost, plantName);
        if (packet && packet->init())
        {
            packet->autorelease();
            return packet;
        }
        delete packet;
        return nullptr;
    }

    /** @brief Static configuration table mapping plant names to their packet data */
    static const std::map<PlantName, PlantConfig> CONFIG_TABLE;

    /** @brief Global interface to create a packet using the configuration table */
    static SeedPacket* createFromConfig(PlantName name);

    virtual bool init() override;
    virtual void onEnter() override;
    virtual void update(float delta) override;

    /** @brief Returns true if the packet is ready for use (not on cooldown) */
    bool isReady() const;

    /** @brief Resets the cooldown timer and sets the state to cooling down */
    void startCooldown();

    /** @brief Returns the sun cost of this specific seed packet */
    int getSunCost() const;

    /** @brief Virtual method to place a plant on the grid */
    virtual Plant* plantAt(const cocos2d::Vec2& globalPos) = 0;

    /** @brief Virtual method to create a non-interactive preview sprite */
    virtual Plant* createPreviewPlant() = 0;

    /** @brief Returns the identifier name of the plant */
    PlantName getPlantName();

private:
    SeedPacket();
    virtual ~SeedPacket();

    /** @brief Updates the visual brightness to reflect cooldown progress */
    void updateCooldownEffect();

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    float cooldown_time;      // Total required cooldown duration
    float accumulated_time;   // Current elapsed cooldown time
    int sun_cost;             // Required sun resource
    bool is_on_cooldown;      // Cooldown state flag
    PlantName plant_name;     // Associated plant type
    std::string seed_packet_image; // Path to texture
};

#endif // __SEED_PACKET_H__