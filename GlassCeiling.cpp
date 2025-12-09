// ================================================================
// Patch: Glass Ceiling / Percentile-Based Rank Adjustment
// Apply AFTER DistributeRankPoints() - ObjectMgr.cpp l.3982
// i.e.
// DistributeRankPoints(ALLIANCE, WeekBegin, flush);
// ApplyGlassCeiling(ALLIANCE);
// DistributeRankPoints(HORDE, WeekBegin, flush);
// ApplyGlassCeiling(HORDE);
// Must check position and formulas, but this is the concept
// ================================================================

void ObjectMgr::ApplyGlassCeiling(uint32 team)
{
    HonorStandingList list = GetStandingListBySide(team);

    if (list.empty())
        return;

    // Step 1: Sort players by total RP descending
    std::sort(list.begin(), list.end(), [](const HonorStanding& a, const HonorStanding& b){
        return (a.rpEarning + a.honorPoints) > (b.rpEarning + b.honorPoints);
    });

    uint32 totalPlayers = list.size();

    // Step 2: Define rank brackets: cumulative top percent and max RP for each rank
    struct RankBracket {
        float percentile; // cumulative top %
        float RPmax;
    };

    RankBracket brackets[] = {
        {0.003f, 59999.0f},  // R14 top 0.3%
        {0.008f, 54999.0f},  // R13 top 0.8%
        {0.02f,   49999.0f}, // R12 top 2%
        {0.035f,  44999.0f}, // R11 top 3.5%
        {0.06f,   39999.0f}, // R10 top 6%
        {0.10f,   34999.0f}, // R09 top 10%
        {0.159f,  29999.0f}, // R08 top 15.9%
        {0.228f,  24999.0f}, // R07 top 22.8%
        {0.327f,  19999.0f}, // R06 top 32.7%
        {0.436f,  14999.0f}, // R05 top 43.6%
        {0.566f,   9999.0f}, // R04 top 56.6%
        {0.697f,   4999.0f}, // R03 top 69.7%
        {0.845f,   1999.0f}  // R02 top 84.5%
        // Remaining players will naturally fall into R01/R00 based on RP
    };

    // Step 3: Iterate and adjust RP according to percentile brackets
    for (uint32 pos = 0; pos < totalPlayers; ++pos)
    {
        HonorStanding& player = list[pos];
        float RP = player.rpEarning + player.honorPoints;

        // Apply the first bracket that the position exceeds
        for (auto& b : brackets)
        {
            uint32 cutoff = std::max(1U, uint32(totalPlayers * b.percentile));
            if (pos >= cutoff)
            {
                RP = std::min(RP, b.RPmax);
            }
            else break;
        }

        // Update player's rpEarning (weekly contribution)
        player.rpEarning = RP - player.honorPoints;

        // Update DB
        CharacterDatabase.PExecute(
            "UPDATE characters SET stored_honor_rating = %f WHERE guid = %u",
            RP, player.guid
        );

        // Debug log
        sLog.outString("[GlassCeiling] Player GUID %u -> RP forced to %.0f", player.guid, RP);
    }
}
