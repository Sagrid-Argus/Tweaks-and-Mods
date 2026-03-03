#pragma once
#include "Strategy.h"
#include "PlayerbotAI.h"
#include <vector>
#include <map>
#include <string>
#include <set>

class MinMaxStrategy : public Strategy
{
public:
    MinMaxStrategy(PlayerbotAI* ai);

    // Strategy interface
    std::string getName() override { return "minmax"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;

    // ===========================
    // Evaluation Functions
    // ===========================

    // Zone evaluation
    float EvaluateZone(uint32 zoneId);                  // Score the efficiency of a zone
    bool ShouldMoveZone(uint32 currentZoneId);         // Should bot move to a better zone?

    // Quest evaluation
    float EvaluateQuest(uint32 questId);               // Score quests for efficiency
    bool ShouldAbandonQuest(uint32 questId);          // Decide if a quest should be abandoned
    void UpdateQuestScores();                          // Update scores for all active quests

    // Activity evaluation
    float EvaluateActivity();                          // Score bot productivity (questing, grinding, etc.)
    void CorrectIdleBehavior();                        // Trigger backup actions if bot is idle

    // Economy / vendor evaluation
    float EvaluateEconomy(uint32 itemId, uint32 price); // Score buying/selling an item
    bool ShouldBuyItem(uint32 itemId, uint32 price);   // Decide if bot should purchase
    void UpdateGoldTargets();                          // Update reserved gold for mounts, spells, AH

    // Gear / equipment evaluation
    float EvaluateGear(uint32 itemId);                // Score a gear item for upgrade potential
    bool ShouldEquipItem(uint32 itemId);              // Decide if item should be equipped

    // Grinding / mob evaluation
    float EvaluateMob(uint32 creatureId);             // Score a mob for XP efficiency
    void SelectBestGrindingSpot();                     // Select the most efficient grind spot

    // Professions / crafting evaluation
    float EvaluateCrafting(uint32 itemId);            // Score crafting usage or materials
    void OptimizeProfessions();                        // Adjust profession actions

    // Miscellaneous helpers
    void Update();                                     // Called each AI tick to update scores and targets
    void DebugPrintStatus();                           // Print bot state for testing

private:
    PlayerbotAI* ai;                                   // Reference to bot AI

    // ===========================
    // Knowledge Storage
    // ===========================

    // Zone scoring
    std::map<uint32, float> zoneScores;               // ZoneID → efficiency score

    // Quest scoring
    std::map<uint32, float> questScores;             // QuestID → score
    std::set<uint32> abandonedQuests;                // Quests marked for abandonment

    // Gear scoring
    std::map<uint32, float> gearScores;              // ItemID → upgrade score

    // Economy / gold management
    float goldReserve;                                // Gold reserved for essential milestones
    float goldAvailable;                              // Gold free to spend
    std::map<uint32, float> itemScores;              // ItemID → economic score

    // Activity tracking
    float lastActivityScore;                          // Last computed activity score
    uint32 idleTicks;                                 // Consecutive ticks of inactivity

    // Grinding
    std::map<uint32, float> mobScores;               // CreatureID → grind efficiency

    // Professions / crafting
    std::map<uint32, float> craftScores;             // ItemID → crafting value

    // Configurable thresholds
    float minQuestScoreThreshold;                     // Minimum score to accept a quest
    float minZoneScoreThreshold;                      // Minimum zone efficiency to stay
    float minActivityScore;                           // Minimum activity score before backup action
    float minGearScoreThreshold;                      // Minimum upgrade score to equip
    float minEconomyScoreThreshold;                   // Minimum score to purchase

    // Internal helpers
    float CalculateTravelPenalty(uint32 zoneId);      // Estimate travel cost / time
    float CalculateElitePenalty(uint32 questOrMobId); // Penalize elite mobs/quests if solo
};
