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

// Initialize the static configuration table with plant-specific metadata and factory lambdas
const std::map<PlantName, PlantConfig> SeedPacket::CONFIG_TABLE = {
    {PlantName::SUNFLOWER,    {"seedpacket_sunflower.png", 7.5f, 50,    [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Sunflower>(i,c,s,n); }}},
    {PlantName::SUNSHROOM,    {"seedpacket_sunshroom.png", 7.5f, 25,    [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Sunshroom>(i,c,s,n); }}},
    {PlantName::PEASHOOTER,   {"seedpacket_peashooter.png", 7.5f, 100,  [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<PeaShooter>(i,c,s,n); }}},
    {PlantName::REPEATER,     {"seedpacket_repeater.png", 7.5f, 200,    [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Repeater>(i,c,s,n); }}},
    {PlantName::THREEPEATER,  {"Threepeater_Seed_Packet_PC.png", 7.5f, 325, [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<ThreePeater>(i,c,s,n); }}},
    {PlantName::PUFFSHROOM,   {"seedpacket_puffshroom.png", 7.5f, 0,     [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Puffshroom>(i,c,s,n); }}},
    {PlantName::WALLNUT,      {"seedpacket_wallnut.png", 30.0f, 50,    [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Wallnut>(i,c,s,n); }}},
    {PlantName::CHERRYBOMB,   {"seedpacket_cherry_bomb.png", 50.0f, 150, [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<CherryBomb>(i,c,s,n); }}},
    {PlantName::SPIKEWEED,    {"seedpacket_spikeweed.png", 7.5f, 100,   [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<SpikeWeed>(i,c,s,n); }}},
    {PlantName::JALAPENO,     {"seedpacket_jalapeno.png", 50.0f, 125,   [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<Jalapeno>(i,c,s,n); }}},
    {PlantName::TWINSUNFLOWER,{"seedpacket_twinsunflower.png", 50.0f, 150, [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<TwinSunflower>(i,c,s,n); }}},
    {PlantName::GATLINGPEA,   {"seedpacket_gatlingpea.png", 50.0f, 250,  [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<GatlingPea>(i,c,s,n); }}},
    {PlantName::POTATOMINE,   {"seedpacket_potatoBomb.png", 30.0f, 25,   [](const std::string& i, float c, int s, PlantName n) {return SeedPacket::create<PotatoMine>(i,c,s,n); }}},
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

SeedPacket::SeedPacket()
    : cooldown_time(7.5f)
    , accumulated_time(0.0f)
    , sun_cost(100)
    , is_on_cooldown(false)
{
    CCLOG("SeedPacket instance created.");
}

SeedPacket::~SeedPacket()
{
    CCLOG("SeedPacket instance destroyed.");
}

bool SeedPacket::init()
{
    // Skip GameObject::init to avoid incorrect default texture loading
    if (!Sprite::init())
    {
        return false;
    }

    if (!seed_packet_image.empty())
    {
        if (!Sprite::initWithFile(seed_packet_image))
        {
            CCLOG("Failed to load seed packet image: %s", seed_packet_image.c_str());
            return false;
        }
    }
    else
    {
        CCLOG("Warning: seed_packet_image path is undefined.");
    }

    is_on_cooldown = false;
    accumulated_time = 0.0f;

    CCLOG("SeedPacket successfully initialized with asset: %s", seed_packet_image.c_str());
    return true;
}

void SeedPacket::onEnter()
{
    Sprite::onEnter();
    // Schedule the update loop once the node is attached to the scene
    this->scheduleUpdate();
}

void SeedPacket::update(float delta)
{
    auto currentScene = Director::getInstance()->getRunningScene();
    auto gameWorld = dynamic_cast<GameWorld*>(currentScene);

    if (!gameWorld)
    {
        return;
    }

    if (is_on_cooldown)
    {
        accumulated_time += delta;

        if (accumulated_time >= cooldown_time)
        {
            // Cooldown complete: reset state
            is_on_cooldown = false;
            accumulated_time = 0.0f;
        }
        else
        {
            updateCooldownEffect();
        }
    }
    else
    {
        // Visual feedback based on affordability
        if (gameWorld->getSunCount() < sun_cost) {
            // Affordability check: Gray out if not enough sun
            this->setColor(Color3B(128, 128, 128));
        }
        else {
            // Ready to be clicked/dragged
            this->setColor(Color3B::WHITE);
        }
    }
}

bool SeedPacket::isReady() const
{
    return !is_on_cooldown;
}

void SeedPacket::startCooldown()
{
    is_on_cooldown = true;
    accumulated_time = 0.0f;
}

int SeedPacket::getSunCost() const
{
    return sun_cost;
}

void SeedPacket::updateCooldownEffect()
{
    // Linear progress from 0.0 to 1.0
    float progress = accumulated_time / cooldown_time;

    // Transition color from very dark (30, 30, 30) to half-brightness (128, 128, 128)
    // The card stays dimmed during the recovery phase
    const GLubyte MIN_BRIGHTNESS = 30;
    const GLubyte MAX_BRIGHTNESS = 128;

    GLubyte value = static_cast<GLubyte>(MIN_BRIGHTNESS + (MAX_BRIGHTNESS - MIN_BRIGHTNESS) * progress);
    this->setColor(Color3B(value, value, value));
}

PlantName SeedPacket::getPlantName()
{
    return this->plant_name;
}