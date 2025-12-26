#include "SeedPacket.h"
#include "Plant.h"
#include "GameWorld.h"
#include "Sunflower.h"
#include "Sunshroom.h"
#include "PeaShooter.h"
#include "Repeater.h"
#include "ThreePeater.h"
#include "Puffshroom.h"
#include "Wallnut.h"
#include "CherryBomb.h"
#include "SpikeWeed.h"
#include "Jalapeno.h"
#include "TwinSunflower.h"
#include "GatlingPea.h"
#include "PotatoMine.h"
#include "SpikeRock.h"

USING_NS_CC;

// 初始化静态配置表
const std::map<PlantName, PlantConfig> SeedPacket::CONFIG_TABLE = {
    {PlantName::SUNFLOWER,    {"seedpacket_sunflower.png", 7.5f, 50,    [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Sunflower>(i,c,s,n); }}},
    {PlantName::SUNSHROOM,    {"seedpacket_sunshroom.png", 7.5f, 25,    [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Sunshroom>(i,c,s,n); }}},
    {PlantName::PEASHOOTER,   {"seedpacket_peashooter.png", 7.5f, 100,  [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<PeaShooter>(i,c,s,n); }}},
    {PlantName::REPEATER,     {"seedpacket_repeater.png", 7.5f, 200,   [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Repeater>(i,c,s,n); }}},
    {PlantName::THREEPEATER,  {"Threepeater_Seed_Packet_PC.png", 7.5f, 325, [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<ThreePeater>(i,c,s,n); }}},
    {PlantName::PUFFSHROOM,   {"seedpacket_puffshroom.png", 7.5f, 0,    [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Puffshroom>(i,c,s,n); }}},
    {PlantName::WALLNUT,      {"seedpacket_wallnut.png", 30.0f, 50,   [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Wallnut>(i,c,s,n); }}},
    {PlantName::CHERRYBOMB,   {"seedpacket_cherry_bomb.png", 50.0f, 150, [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<CherryBomb>(i,c,s,n); }}},
    {PlantName::SPIKEWEED,    {"seedpacket_spikeweed.png", 7.5f, 100,  [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<SpikeWeed>(i,c,s,n); }}},
    {PlantName::JALAPENO,     {"seedpacket_jalapeno.png", 50.0f, 125,   [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Jalapeno>(i,c,s,n); }}},
    {PlantName::TWINSUNFLOWER,{"seedpacket_twinsunflower.png", 50.0f, 150, [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<TwinSunflower>(i,c,s,n); }}},
    {PlantName::GATLINGPEA,   {"seedpacket_gatlingpea.png", 50.0f, 250,  [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<GatlingPea>(i,c,s,n); }}},
    {PlantName::POTATOMINE,   {"seedpacket_potatoBomb.png", 30.0f, 25,  [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<PotatoMine>(i,c,s,n); }}},
    {PlantName::SPIKEROCK,    {"seedpacket_spikerock.png", 50.0f, 125,  [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<SpikeRock>(i,c,s,n); }}}
};

SeedPacket* SeedPacket::createFromConfig(PlantName name) {
    auto it = CONFIG_TABLE.find(name);
    if (it != CONFIG_TABLE.end()) {
        const PlantConfig& cfg = it->second;
        return cfg.factory(cfg.packetImage, cfg.cooldown, cfg.sunCost, name);
    }
    return nullptr;
}

// Protected constructor
SeedPacket::SeedPacket()
    : _cooldownTime(7.5f)
    , _accumulatedTime(0.0f)
    , _sunCost(100)
    , _isOnCooldown(false)
{
    CCLOG("SeedPacket created.");
}

// Destructor
SeedPacket::~SeedPacket()
{
    CCLOG("SeedPacket destroyed.");
}

// Initialization
bool SeedPacket::init()
{
    // Don't call GameObject::init() because it tries to load empty IMAGE_FILENAME
    // Call Sprite::init() directly
    if (!Sprite::init())
    {
        return false;
    }

    // Load image (subclass should set _seedPacketImage before calling this)
    if (!_seedPacketImage.empty())
    {
        if (!Sprite::initWithFile(_seedPacketImage))
        {
            CCLOG("Failed to load seed packet image: %s", _seedPacketImage.c_str());
            return false;
        }
    }
    else
    {
        CCLOG("Warning: SeedPacket _seedPacketImage is empty!");
    }

    _isOnCooldown = false;
    _accumulatedTime = 0.0f;

    // Don't schedule update here - it will be scheduled in onEnter()
    // scheduleUpdate() requires the node to be in the scene tree (_running = true)

    CCLOG("SeedPacket initialized with image: %s", _seedPacketImage.c_str());
    return true;
}

// onEnter - schedule update when node enters the scene
void SeedPacket::onEnter()
{
    Sprite::onEnter();
    // Enable update for cooldown animation (now safe because node is in scene tree)
    this->scheduleUpdate();
}

// Update function
void SeedPacket::update(float delta)
{
    auto currentScene = Director::getInstance()->getRunningScene();
    auto gameWorld = dynamic_cast<GameWorld*>(currentScene);
	if (!gameWorld)
	{
		CCLOG("SeedPacket update: GameWorld not found!");
		return;
	}
    if (_isOnCooldown)
    {
        _accumulatedTime += delta;

        if (_accumulatedTime >= _cooldownTime)
        {
            // 冷却结束，恢复到 50% 灰度
            _isOnCooldown = false;
            _accumulatedTime = 0.0f;

        }
        else
        {
            // Update cooldown visual effect
            updateCooldownEffect();
        }
    }
    else
    {
        if (gameWorld->getSunCount() < _sunCost) {
            this->setColor(Color3B(128, 128, 128));
            CCLOG("Seed packet cooldown finished!");
        }
        else {
            this->setColor(Color3B::WHITE);
            CCLOG("Seed packet cooldown finished and ready to use!");
        }
    }
}

// Check if ready
bool SeedPacket::isReady() const
{
    return !_isOnCooldown;
}

// Start cooldown
void SeedPacket::startCooldown()
{
    _isOnCooldown = true;
    _accumulatedTime = 0.0f;
}

// Get sun cost
int SeedPacket::getSunCost() const
{
    return _sunCost;
}

// Update cooldown visual effect
void SeedPacket::updateCooldownEffect()
{
    // Calculate progress: 0.0 (just started) to 1.0 (finished)
    float progress = _accumulatedTime / _cooldownTime;

    // Interpolate from very dark (30, 30, 30) to 50% brightness (128, 128, 128)
    // This makes cooldown very obvious - card stays dim until fully ready
    const GLubyte MIN_BRIGHTNESS = 30;   // Very dark when cooling
    const GLubyte MAX_BRIGHTNESS = 128;  // Only 50% of 255 during cooldown
    
    GLubyte value = static_cast<GLubyte>(MIN_BRIGHTNESS + (MAX_BRIGHTNESS - MIN_BRIGHTNESS) * progress);

    this->setColor(Color3B(value, value, value));
}

PlantName SeedPacket::getPlantName()
{
    return this->_plantName;
}