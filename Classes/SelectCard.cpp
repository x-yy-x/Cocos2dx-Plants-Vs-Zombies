#include "SelectCard.h"
#include "SeedPacket.h"

USING_NS_CC;

SelectCard::SelectCard()
    : _isSelected(false)
    , _plantName(PlantName::SUNFLOWER)
    , _seedPacket(nullptr)
{
}

SelectCard::~SelectCard()
{
}

SelectCard* SelectCard::create(const std::string& imageFile, PlantName plantName, SeedPacket* seedPacket)
{
    SelectCard* ret = new (std::nothrow) SelectCard();
    if (ret && ret->initWithFile(imageFile))
    {
        ret->_imageFile = imageFile;
        ret->_plantName = plantName;
        ret->_seedPacket = seedPacket;
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void SelectCard::setSelected(bool selected)
{
    _isSelected = selected;
    if (_isSelected)
    {
        // Gray out when selected
        this->setColor(Color3B(128, 128, 128));
    }
    else
    {
        // Normal color when not selected
        this->setColor(Color3B::WHITE);
    }
}


