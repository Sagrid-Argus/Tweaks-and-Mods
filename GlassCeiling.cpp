// =============================================================
// Patch: Glass Ceiling / Percentile-Based PvP Rank Adjustment
// Enforcing top-percentage rank limits and RP ceilings per week
// =============================================================

// From l. 833 ObjectMgr.h

void ApplyGlassCeiling(uint32 team);

// From l. 3766 ObjectMgr.cpp

void ObjectMgr::LoadStandingList()
{
    uint32 LastWeekBegin = sWorld.GetDateLastMaintenanceDay() - 7;
    LoadStandingList(LastWeekBegin);

    // Distribution of RP earning without flushing table
    DistributeRankPoints(ALLIANCE, LastWeekBegin);
    ApplyGlassCeiling(ALLIANCE); // Apply glass ceiling immediately

    DistributeRankPoints(HORDE, LastWeekBegin);
    ApplyGlassCeiling(HORDE);    // Apply glass ceiling immediately

    sLog.outString();
    sLog.outString(">> Loaded %u Horde and %u Ally honor standing definitions",
                   static_cast<uint32>(HordeHonorStandingList.size()),
                   static_cast<uint32>(AllyHonorStandingList.size()));
}

// From l.3826 ObjectMgr.cpp

void ObjectMgr::FlushRankPoints(uint32 dateTop)
{
    // FLUSH CP
    auto queryResult = CharacterDatabase.PQuery(
        "SELECT date FROM character_honor_cp WHERE TYPE = %u AND date <= %u "
        "GROUP BY date ORDER BY date DESC", HONORABLE, dateTop);

    if (queryResult)
    {
        uint32 date;
        bool flush;
        uint32 WeekBegin = dateTop - 7;

        // search latest non-processed date if the server has been offline for multiple weeks
        do
        {
            date = queryResult->Fetch()->GetUInt32();
            while (WeekBegin && date < WeekBegin)
                WeekBegin -= 7;
        }
        while (queryResult->NextRow());

        // start to flush from latest non-processed date to current
        while (WeekBegin <= dateTop)
        {
            LoadStandingList(WeekBegin);

            flush = WeekBegin < dateTop - 7; // flush only older weeks

            DistributeRankPoints(ALLIANCE, WeekBegin, flush);
            ApplyGlassCeiling(ALLIANCE); // Apply immediately after

            DistributeRankPoints(HORDE, WeekBegin, flush);
            ApplyGlassCeiling(HORDE);    // Apply immediately after

            WeekBegin += 7;
        }
    }

// From l. 3983 ObjectMgr.cpp

void ObjectMgr::ApplyGlassCeiling(uint32 team)
{
    HonorStandingList list = sObjectMgr.GetStandingListBySide(team);

    if (list.empty())
        return;

    std::vector<HonorStanding> sorted;
    sorted.reserve(list.size());
    for (auto const& entry : list)
        sorted.push_back(entry);

    // Sort players by descending RP = rpEarning + honorPoints
    
    std::sort(sorted.begin(), sorted.end(), [](const HonorStanding& a, const HonorStanding& b){
        return (a.rpEarning + a.honorPoints) > (b.rpEarning + b.honorPoints);
    });

    uint32 totalPlayers = sorted.size();

    // Percentile based max positions
    
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

    for (uint32 pos = 0; pos < totalPlayers; ++pos)
    {
        HonorStanding& player = sorted[pos];
        float RP = player.rpEarning + player.honorPoints;

    // Apply RP ceilings
    
        if (pos >= maxR14)
            RP = std::min(RP, 59999.0f);
        else if (pos >= maxR13)
            RP = std::min(RP, 54999.0f);
        else if (pos >= maxR12)
            RP = std::min(RP, 49999.0f);
        else if (pos >= maxR11)
            RP = std::min(RP, 44999.0f);
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

    // Re compute weekly RP contribution
        
        player.rpEarning = RP - player.honorPoints;

    // Update DB
        
        CharacterDatabase.PExecute(
            "UPDATE characters SET stored_honor_rating = %f WHERE guid = %u",
            RP, player.guid
        );
    
        // Server Log
        
        sLog.outString("[GlassCeiling] Player %u forced to %.0f RP", player.guid, RP);
    }
}
