#ifndef __PLAYER_PROFILE_H__
#define __PLAYER_PROFILE_H__

#include "GameDefs.h"
#include <unordered_set>

/**
 * @brief Singleton class managing persistent player data.
 * Handles currency (coins), unlocked plant types, and purchased power-ups
 * like the garden rake and lawn mowers.
 */
class PlayerProfile
{
public:
    /** @brief Access the global instance of the player profile. */
    static PlayerProfile* getInstance();

    // Delete copy constructor and assignment operator to enforce Singleton pattern
    PlayerProfile(const PlayerProfile&) = delete;
    PlayerProfile& operator=(const PlayerProfile&) = delete;

    // --- Currency Management ---
    long long getCoins() const;
    void addCoins(long long amount);
    bool spendCoins(long long amount);

    // --- Plant Progression ---
    void unlockPlant(PlantName plantName);
    bool isPlantUnlocked(PlantName plantName) const;

    // --- Shop Items & Power-ups ---
    void enableRake(bool enable = true);
    bool isRakeEnabled() const;
    void enableMower(bool enable = true);
    bool isMowerEnabled() const;

private:
    PlayerProfile();
    static PlayerProfile* instance;

    long long coins;
    std::unordered_set<PlantName> unlocked_plants;
    bool rake_enabled{ false };
    bool mower_enabled{ false };
};

#endif // __PLAYER_PROFILE_H__