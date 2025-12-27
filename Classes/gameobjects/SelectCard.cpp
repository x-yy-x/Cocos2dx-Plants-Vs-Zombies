#include "SelectCard.h"
#include "SeedPacket.h"

USING_NS_CC;

// ---------------------------------------------------------
// Constructor & Destructor
// ---------------------------------------------------------

SelectCard::SelectCard()
    : is_selected(false)
    , plant_name(PlantName::SUNFLOWER)
    , seed_packet(nullptr)
{
    // Member variables are initialized in the list above
}

SelectCard::~SelectCard()
{
    // Standard cleanup
}

// ---------------------------------------------------------
// Factory Method
// ---------------------------------------------------------

SelectCard* SelectCard::create(const std::string& imageFile, PlantName plantName, SeedPacket* seedPacket)
{
    SelectCard* ret = new (std::nothrow) SelectCard();

    // Initialize the sprite with the provided image file
    if (ret && ret->initWithFile(imageFile))
    {
        ret->image_file = imageFile;
        ret->plant_name = plantName;
        ret->seed_packet = seedPacket;

        // Add to the autorelease pool for automatic memory management
        ret->autorelease();
        return ret;
    }

    // Cleanup if initialization fails
    delete ret;
    return nullptr;
}

// ---------------------------------------------------------
// Logic Implementation
// ---------------------------------------------------------

void SelectCard::setSelected(bool selected)
{
    is_selected = selected;

    if (is_selected)
    {
        /** * Apply a gray tint (50% brightness) to indicate that this card
         * has already been chosen and is sitting in the player's slots.
         */
        this->setColor(Color3B(128, 128, 128));
    }
    else
    {
        /** * Restore full brightness (white color filter) to indicate
         * the card is available to be picked.
         */
        this->setColor(Color3B::WHITE);
    }
}