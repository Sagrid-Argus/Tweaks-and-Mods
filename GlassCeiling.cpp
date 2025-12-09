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

    // Step 2: Define maximum positions for each rank based on cumulative percentiles
    
    uint32 maxR14 = std::max(1U, uint32(totalPlayers * 0.003f));   // Top 0.3%
    uint32 maxR13 = std::max(1U, uint32(totalPlayers * 0.008f));   // Top 0.8%
    uint32 maxR12 = std::max(1U, uint32(totalPlayers * 0.02f));    // Top 2%
    uint32 maxR11 = std::max(1U, uint32(totalPlayers * 0.035f));   // Top 3.5%
    uint32 maxR10 = std::max(1U, uint32(totalPlayers * 0.060f));   // Top 6%
    uint32 maxR09 = std::max(1U, uint32(totalPlayers * 0.10f));    // Top 10%
    uint32 maxR08 = std::max(1U, uint32(totalPlayers * 0.159f));   // Top 15.9%
    uint32 maxR07 = std::max(1U, uint32(totalPlayers * 0.228f));   // Top 22.8%
    uint32 maxR06 = std::max(1U, uint32(totalPlayers * 0.327f));   // Top 32.7%
    uint32 maxR05 = std::max(1U, uint32(totalPlayers * 0.436f));   // Top 43.6%
    uint32 maxR04 = std::max(1U, uint32(totalPlayers * 0.566f));   // Top 56.6%
    uint32 maxR03 = std::max(1U, uint32(totalPlayers * 0.697f));   // Top 69.7%
    uint32 maxR02 = std::max(1U, uint32(totalPlayers * 0.845f));   // Top 84.5%

    // Step 3: Iterate over players and force RP according to percentile ceiling
    
    for (uint32 pos = 0; pos < totalPlayers; ++pos)
    {
        HonorStanding& player = list[pos];
        float RP = player.rpEarning + player.honorPoints;

        // Apply ceiling according to rank percentile
        
        if (pos >= maxR14)
            RP = std::min(RP, 59999.0f); // Outside top 0.3% → max RP for R14
        else if (pos >= maxR13)
            RP = std::min(RP, 54999.0f); // Outside top 0.8% → max RP for R13
        else if (pos >= maxR12)
            RP = std::min(RP, 49999.0f); // Outside top 2% → max RP for R12
        else if (pos >= maxR11)
            RP = std::min(RP, 44999.0f); // etc.
        else if (pos >= maxR10)
            RP = std::min(RP, 39999.0f);
        else if (pos >= maxR09)
            RP = std::min(RP, 34999.0f);
        else if (pos >= maxR08)
            RP = std::min(RP, 29999.0f);
        else if (pos >= maxR07)
            RP = std::min(RP, 24999.0f);
        else if (pos >= maxR06)
            RP = std::min(RP, 19999.0f);
        else if (pos >= maxR05)
            RP = std::min(RP, 14999.0f);
        else if (pos >= maxR04)
            RP = std::min(RP, 9999.0f);
        else if (pos >= maxR03)
            RP = std::min(RP, 4999.0f);
        else if (pos >= maxR02)
            RP = std::min(RP, 1999.0f);

        // Update player's rpEarning (weekly contribution)
        
        player.rpEarning = RP - player.honorPoints;

        // Update database
        
        CharacterDatabase.PExecute(
            "UPDATE characters SET stored_honor_rating = %f WHERE guid = %u",
            RP, player.guid
        );

        // Debug log
        
        sLog.outString("[GlassCeiling] Player GUID %u -> RP forced to %.0f", player.guid, RP);
    }
}
