#pragma once
#include "cocos2d.h"
#include "GameDefs.h"
#include "SeedPacket.h"

class SelectCard : public cocos2d::Sprite
{
public:
    static SelectCard* create(const std::string& imageFile, PlantName plantName, SeedPacket* seedPacket);
    
    bool isSelected() const { return _isSelected; }
    void setSelected(bool selected);
    
    PlantName getPlantName() const { return _plantName; }
    SeedPacket* getSeedPacket() const { return _seedPacket; }
    const std::string& getImageFile() const { return _imageFile; }
    
protected:
    SelectCard();
    virtual ~SelectCard();
    
private:
    bool _isSelected;
    PlantName _plantName;
    SeedPacket* _seedPacket;
    std::string _imageFile;
};

