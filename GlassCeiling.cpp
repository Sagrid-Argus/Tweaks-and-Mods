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

    // Step 1: Sort players by RP descending
  
    std::sort(list.begin(), list.end(), [](const HonorStanding& a, const HonorStanding& b){
    return a.rpEarning + a.honorPoints > b.rpEarning + b.honorPoints;
    });

    uint32 totalPlayers = list.size();

    // Step 2: Define maximum positions for each rank based on percentiles
  
    uint32 maxR14 = std::max(1U, uint32(totalPlayers * 0.003f)); 
    uint32 maxR13 = std::max(1U, uint32(totalPlayers * 0.008f)); 
    uint32 maxR12 = std::max(1U, uint32(totalPlayers * 0.02f));
    uint32 maxR11 = std::max(1U, uint32(totalPlayers * 0.035f));
    uint32 maxR10 = std::max(1U, uint32(totalPlayers * 0.060f));
    uint32 maxR09 = std::max(1U, uint32(totalPlayers * 0.10f));
    uint32 maxR08 = std::max(1U, uint32(totalPlayers * 0.159f));
    uint32 maxR07 = std::max(1U, uint32(totalPlayers * 0.228f));
    uint32 maxR06 = std::max(1U, uint32(totalPlayers * 0.327f));
    uint32 maxR05 = std::max(1U, uint32(totalPlayers * 0.436f));
    uint32 maxR04 = std::max(1U, uint32(totalPlayers * 0.566f));
    uint32 maxR03 = std::max(1U, uint32(totalPlayers * 0.697f));
    uint32 maxR02 = std::max(1U, uint32(totalPlayers * 0.845f));
    
    // Step 3: Iterate and force RP to respect bracket limits
  
    for (uint32 pos = 0; pos < totalPlayers; ++pos)
    {
        HonorStanding& player = list[pos];

        float RP = player.rpEarning + player.honorPoints;

        if (pos >= maxR14) 
            RP = std::min(RP, 59999.0f); // Here everyone out of Top 0,3% is forced out of R14
        if (pos >= maxR13) // etc., for R12
            RP = std::min(RP, 54999.0f);
        if (pos >= maxR12)
            RP = std::min(RP, 49999.0f);
        if (pos >= maxR11)
            RP = std::min(RP, 44999.0f);
        if (pos >= maxR10)
            RP = std::min(RP, 39999.0f);
        if (pos >= maxR09)
            RP = std::min(RP, 34999.0f);
        if (pos >= maxR08)
            RP = std::min(RP, 29999.0f);
        if (pos >= maxR07)
            RP = std::min(RP, 24999.0f);
        if (pos >= maxR06)
            RP = std::min(RP, 19999.0f);
        if (pos >= maxR05)
            RP = std::min(RP, 14999.0f);
        if (pos >= maxR04)
            RP = std::min(RP, 9999.0f);
        if (pos >= maxR03)
            RP = std::min(RP, 4999.0f);
        if (pos >= maxR02)
            RP = std::min(RP, 1999.0f);

        // Save adjusted RP back
        player.rpEarning = RP - player.honorPoints;

        // Update DB
        CharacterDatabase.PExecute(
            "UPDATE characters SET stored_honor_rating = %f WHERE guid = %u",
            RP, player.guid
        );
    }
}


