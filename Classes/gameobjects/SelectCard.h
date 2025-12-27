#ifndef __SELECT_CARD_H__
#define __SELECT_CARD_H__

#include "cocos2d.h"
#include "GameDefs.h"
#include "SeedPacket.h"

/**
 * @brief Represents a plant card in the "Choose Your Seeds" menu.
 * This class handles the visual state (selected vs unselected) during
 * the card selection phase before the game starts.
 */
class SelectCard : public cocos2d::Sprite
{
public:
    /**
     * @brief Static factory method to create a SelectCard instance.
     * @param imageFile Path to the card texture.
     * @param plantName The associated plant type identifier.
     * @param seedPacket Pointer to the corresponding SeedPacket object.
     * @return Pointer to the created SelectCard object.
     */
    static SelectCard* create(const std::string& imageFile, PlantName plantName, SeedPacket* seedPacket);

    /** @brief Returns true if the card is already picked for the current level. */
    bool isSelected() const { return is_selected; }

    /** * @brief Updates the selection state and visual appearance.
     * @param selected True to gray out the card, False to return to normal.
     */
    void setSelected(bool selected);

    /** @brief Gets the plant name identifier associated with this card. */
    PlantName getPlantName() const { return plant_name; }

    /** @brief Gets the pointer to the seed packet data. */
    SeedPacket* getSeedPacket() const { return seed_packet; }

    /** @brief Gets the texture filename of the card. */
    const std::string& getImageFile() const { return image_file; }

private:
    SelectCard();
    virtual ~SelectCard();

    bool is_selected;           // Selection state flag
    PlantName plant_name;       // Type of plant this card represents
    SeedPacket* seed_packet;    // Reference to the functional seed packet
    std::string image_file;     // Texture path storage
};

#endif // __SELECT_CARD_H__