// ==================================================================
// Add in PlayerbotFactory.cpp - Custom Level 20 Mount Initialization
// ==================================================================

#ifdef MANGOSBOT_ZERO

std::vector<uint32> custom20;
switch (bot->getRace())
{
    case RACE_HUMAN:      custom20 = {34000, 34001}; break; // Honor Steed, Harvest Steed
    case RACE_ORC:        custom20 = {34012, 34013}; break; // Frostfang Wolf, Embermane Wolf
    case RACE_DWARF:      custom20 = {34008, 34009}; break; // Coal Ram, Glacier Ram
    case RACE_NIGHTELF:   custom20 = {34004, 34005}; break; // White Sabertooth, Black Sabertooth  
    case RACE_UNDEAD:     custom20 = {34002, 34003}; break; // White Skeleton Horse, Scarlet Skeleton Horse
    case RACE_TAUREN:     custom20 = {34014, 34015}; break; // Mosshide Kodo, Skyhide Kodo
    case RACE_GNOME:      custom20 = {34006, 34007}; break; // Emerlight Mechanostrider, Whiteflare Mechanostrider
    case RACE_TROLL:      custom20 = {34010, 34011}; break; // Bwon'Scara Raptor, Jin'Roka Raptor
}

if (bot->GetLevel() >= 20 && bot->GetLevel() < 40 && !custom20.empty())
{
    uint32 index = urand(0, custom20.size() - 1); 
    uint32 spell = custom20[index];
    bot->learnSpell(spell, false); 
    sLog.outDetail("Bot %d (level %d) learned custom level 20 mount spell %d",
    bot->GetGUIDLow(), bot->GetLevel(), spell);
}
#endif

// ==============================================================================
// Add in AutoLearnSpellAction.cpp - Custom Level 20 Mount Auto-Learn on Level-Up
// ==============================================================================

#ifdef MANGOSBOT_ZERO

std::vector<uint32> custom20;
switch (bot->getRace())
{
    case RACE_HUMAN:      custom20 = {34000, 34001}; break; // Honor Steed, Harvest Steed
    case RACE_ORC:        custom20 = {34012, 34013}; break; // Frostfang Wolf, Embermane Wolf
    case RACE_DWARF:      custom20 = {34008, 34009}; break; // Coal Ram, Glacier Ram
    case RACE_NIGHTELF:   custom20 = {34004, 34005}; break; // White Sabertooth, Black Sabertooth  
    case RACE_UNDEAD:     custom20 = {34002, 34003}; break; // White Skeleton Horse, Scarlet Skeleton Horse
    case RACE_TAUREN:     custom20 = {34014, 34015}; break; // Mosshide Kodo, Skyhide Kodo
    case RACE_GNOME:      custom20 = {34006, 34007}; break; // Emerlight Mechanostrider, Whiteflare Mechanostrider
    case RACE_TROLL:      custom20 = {34010, 34011}; break; // Bwon'Scara Raptor, Jin'Roka Raptor
}

uint32 firstmount = 40;

if (bot->GetLevel() >= 20 && bot->GetLevel() < firstmount && !custom20.empty())
{
    uint32 index = urand(0, custom20.size() - 1);
    uint32 spell = custom20[index];
    bot->learnSpell(spell, false);
    sLog.outDetail("Bot %d reached level %d and learned custom lvl20 mount spell %d",
    bot->GetGUIDLow(), bot->GetLevel(), spell);
}
#endif

 // =======================================================================================================
 // Add in CheckMountStateAction.cpp - Allow Early Mounting - Not needed anymore after druid / shaman patch
 // =======================================================================================================

#ifdef MANGOSBOT_ZERO
// Allow early mounting if bot has class/custom quest movement spells
if (!firstmount)
{
    if (bot->HasSpell(34000) || // Honor Steed
        bot->HasSpell(34001) || // Harvest Steed
        bot->HasSpell(34002) || // White Skeleton Horse
        bot->HasSpell(34003) || // Scarlet Skeleton Horse
        bot->HasSpell(34004) || // White Sabertooth
        bot->HasSpell(34005) || // Black Sabertooth
        bot->HasSpell(34006) || // Emerlight Mechanostrider
        bot->HasSpell(34007) || // Whiteflare Mechanostrider
        bot->HasSpell(34008) || // Coal Ram
        bot->HasSpell(34009) || // Glacier Ram
        bot->HasSpell(34010) || // Bwon'Scara Raptor
        bot->HasSpell(34011) || // Jin'Roka Raptor
        bot->HasSpell(34012) || // Frostfang Wolf
        bot->HasSpell(34013) || // Embermane Wolf
        bot->HasSpell(34014) || // Mosshide Kodo
        bot->HasSpell(34015) || // Skyhide Kodo
        bot->HasSpell(783) ||   // Travel Form (druid)
        bot->HasSpell(2645))    // Ghost Wolf (shaman)
    {
        firstmount = true;
        sLog.outDetail("Bot %d allowed early mount usage (custom20/travel/ghost form)", bot->GetGUIDLow());
    }
}
#endif
