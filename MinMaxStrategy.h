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

        float EvaluateZone(uint32 zoneId);
        bool ShouldMoveZone(uint32 currentZoneId);
      
        float EvaluateQuest(uint32 questId);
        bool ShouldAbandonQuest(uint32 questId);
        void UpdateQuestScores();

        float EvaluateActivity();
        void CorrectIdleBehavior();

        float EvaluateEconomy(uint32 itemId, uint32 price);
        bool ShouldBuyItem(uint32 itemId, uint32 price);
        void UpdateGoldTargets();

        float EvaluateGear(uint32 itemId);
        bool ShouldEquipItem(uint32 itemId);

        float EvaluateMob(uint32 creatureId);
        void SelectBestGrindingSpot();

        float EvaluateCrafting(uint32 itemId);
        void OptimizeProfessions();

    private:
        PlayerbotAI* ai;

        std::map<uint32, float> zoneScores;
        std::map<uint32, float> questScores;
        std::set<uint32> abandonedQuests;

        std::map<uint32, float> gearScores;

        float goldReserve;
        float goldAvailable;
        std::map<uint32, float> itemScores;

        float lastActivityScore;
        uint32 idleTicks;

        std::map<uint32, float> mobScores;

        std::map<uint32, float> craftScores;

        float minQuestScoreThreshold;
        float minZoneScoreThreshold;
        float minActivityScore;
        float minGearScoreThreshold;
        float minEconomyScoreThreshold;

        float CalculateTravelPenalty(uint32 zoneId);
        float CalculateElitePenalty(uint32 questOrMobId);
    };
}
