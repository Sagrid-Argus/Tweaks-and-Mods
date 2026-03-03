#pragma once
#include "playerbot/strategy/Strategy.h"
#include "playerbot/playerbot.h"
#include <vector>
#include <map>
#include <string>
#include <set>

namespace ai
{
    class MinMaxStrategy : public Strategy
{
public:
    MinMaxStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    std::string getName() override { return "minmax"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    int GetType() override { return STRATEGY_TYPE_GENERIC; }

#ifdef GenerateBotHelp
    std::string GetHelpName() override { return "minmax"; }
    std::string GetHelpDescription() override
    {
        return "Overrides bot decisions to maximize XP, gold efficiency, and quest selection.";
    }
#endif

private:
    void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;

    void OnStrategyAdded(BotState state) override;
    void OnStrategyRemoved(BotState state) override;

public:
    void Update();
    void DebugPrintStatus();
};
    
    float EvaluateZone(uint32 zoneId);                  // Score the efficiency of a zone
    bool ShouldMoveZone(uint32 currentZoneId);         // Should bot move to a better zone?
  
    float EvaluateQuest(uint32 questId);               // Score quests for efficiency
    bool ShouldAbandonQuest(uint32 questId);          // Decide if a quest should be abandoned
    void UpdateQuestScores();                          // Update scores for all active quests

    float EvaluateActivity();                          // Score bot productivity (questing, grinding, etc.)
    void CorrectIdleBehavior();                        // Trigger backup actions if bot is idle

    float EvaluateEconomy(uint32 itemId, uint32 price); // Score buying/selling an item
    bool ShouldBuyItem(uint32 itemId, uint32 price);   // Decide if bot should purchase
    void UpdateGoldTargets();                          // Update reserved gold for mounts, spells, AH

    float EvaluateGear(uint32 itemId);                // Score a gear item for upgrade potential
    bool ShouldEquipItem(uint32 itemId);              // Decide if item should be equipped

    float EvaluateMob(uint32 creatureId);             // Score a mob for XP efficiency
    void SelectBestGrindingSpot();                     // Select the most efficient grind spot

    float EvaluateCrafting(uint32 itemId);            // Score crafting usage or materials
    void OptimizeProfessions();                        // Adjust profession actions

private:
    PlayerbotAI* ai;                                   // Reference to bot AI

    std::map<uint32, float> zoneScores;               // ZoneID → efficiency score
    std::map<uint32, float> questScores;             // QuestID → score
    std::set<uint32> abandonedQuests;                // Quests marked for abandonment

    std::map<uint32, float> gearScores;              // ItemID → upgrade score

    float goldReserve;                                // Gold reserved for essential milestones
    float goldAvailable;                              // Gold free to spend
    std::map<uint32, float> itemScores;              // ItemID → economic score

    float lastActivityScore;                          // Last computed activity score
    uint32 idleTicks;                                 // Consecutive ticks of inactivity

    std::map<uint32, float> mobScores;               // CreatureID → grind efficiency

    std::map<uint32, float> craftScores;             // ItemID → crafting value

    float minQuestScoreThreshold;                     // Minimum score to accept a quest
    float minZoneScoreThreshold;                      // Minimum zone efficiency to stay
    float minActivityScore;                           // Minimum activity score before backup action
    float minGearScoreThreshold;                      // Minimum upgrade score to equip
    float minEconomyScoreThreshold;                   // Minimum score to purchase

    float CalculateTravelPenalty(uint32 zoneId);      // Estimate travel cost / time
    float CalculateElitePenalty(uint32 questOrMobId); // Penalize elite mobs/quests if solo
};
