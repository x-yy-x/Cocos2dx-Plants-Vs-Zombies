#pragma once

#include "Plant.h"
#include "GameWorld.h"
#include "cocos2d.h"

// 必须使用 virtual public Plant 以解决菱形继承问题
class Mushroom : virtual public Plant
{
public:
    // 蘑菇通用的活动状态
    enum class ActivityState
    {
        ACTIVE,     // 活跃（晚上）
        SLEEPING    // 睡觉（白天）
    };

protected:
    Mushroom();
    virtual ~Mushroom();

    // ----------------------------------------------------------------
    // 状态变量
    // ----------------------------------------------------------------
    bool _isNightMode;      // 当前是否为夜晚
    bool _isInitialized;    // 是否已完成首次初始化
    ActivityState _activityState; // 当前活动状态
    cocos2d::Action* _currentAnimation; // 当前播放的动画动作

    // ----------------------------------------------------------------
    // 核心复用功能
    // ----------------------------------------------------------------

    /**
     * @brief 检查并更新昼夜模式
     * @return 如果模式发生了改变（从昼变夜或反之）返回 true，否则返回 false
     */
    bool checkDayNightChange();

    /**
     * @brief 判断当前是否是白天
     */
    bool isDaytime() const;

    /**
     * @brief 通用动画加载辅助函数
     * @param folderPath 图片文件夹路径
     * @param frameCount 帧数
     * @param scale 缩放比例
     * @param offsetX 裁剪区域X偏移 (默认0)
     * @param offsetY 裁剪区域Y偏移 (默认0)
     * @param cropWidth 裁剪宽度 (小于0则使用默认对象宽度)
     * @param cropHeight 裁剪高度 (小于0则使用默认对象高度)
     */
    cocos2d::Animation* loadAnimation(const std::string& folderPath,
        int frameCount,
        float scale,
        float objectW, float objectH, // 传入对象原始尺寸
        float offsetX = 0.0f,
        float offsetY = 0.0f,
        float cropWidth = -1.0f,
        float cropHeight = -1.0f);

    // 子类需要实现的纯虚函数或钩子，用于处理唤醒和睡眠的具体表现
    virtual void wakeUp() = 0;
    virtual void sleep() = 0;
};