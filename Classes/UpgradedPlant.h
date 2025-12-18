#pragma once

#include "Plant.h"


class UpgradedPlant : virtual public Plant
{
public:
    // 能否升级指定植物
    virtual bool canUpgrade(Plant* basePlant) const = 0;

    // 执行升级（删除旧植物，生成新植物）
    virtual UpgradedPlant* upgrade(Plant* basePlant) = 0;
};

