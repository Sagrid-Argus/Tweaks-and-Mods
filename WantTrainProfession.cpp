// Bot calculates what profession he wants // TrainerValues.h l.45

class WantTrainProfessionValue : public CalculatedValue<bool>
{
public:
    WantTrainProfessionValue(PlayerbotAI* ai, std::string qualifier = "") 
        : CalculatedValue<bool>(ai, qualifier) {}

    bool Calculate() override
    {
        uint32 skillId = atoi(getQualifier().c_str());

        uint32 first = AI_VALUE2(uint32, "desired train profession", "first");
        uint32 second = AI_VALUE2(uint32, "desired train profession", "second");

        return (skillId == first || skillId == second);
    }
};

// Hook in TrainerValues.cpp l.140
// Bot wants to train

uint32 skillId = sSpellMgr.GetSpellSkill(spellId);

if (sSpellMgr.IsProfessionSpell(spellId))
{
    if (!AI_VALUE2(bool, "want train profession", std::to_string(skillId)))
        continue;
}

// Exemple with Warrior Bot

class DesiredTrainProfessionValue : public CalculatedValue<uint32>
{
public:
    DesiredTrainProfessionValue(PlayerbotAI* ai, std::string qualifier = "") 
        : CalculatedValue<uint32>(ai, qualifier) {}

    uint32 Calculate() override
{
    Player* bot = ai->GetBot();
    std::string q = getQualifier();

    bool first = (q == "first"); // always return current first profession

    // verification of prob first learnt professions
    std::vector<uint32> allProfessions = {164, 171, 202, 197, 165, 333}; // BS, Alch, Eng, Tailor, Leather, Enchant
    for (uint32 skillId : allProfessions)
    {
        if (bot->HasSkill(skillId))
        {
            return skillId;
        }
    }
// 1st profession skill
        if (q == "first")
        {
            uint32 stored = AI_VALUE(uint32, "stored first profession");
            if (stored)
                return stored;

            uint32 chosen = 0;

            if (bot->getClass() == CLASS_WARRIOR)
            {
                uint32 roll = urand(0, 99);

                if (roll < 45)
                    chosen = 164; // Blacksmithing
                else if (roll < 80)
                    chosen = 202; // Engineering
                else
                    chosen = 171; // Alchemy
            }

            SET_AI_VALUE(uint32, "stored first profession", chosen);
            return chosen;
        }

// 2nd profession skill
        if (q == "second")
        {
            uint32 stored = AI_VALUE(uint32, "stored second profession");
            if (stored)
                return stored;

            uint32 first = AI_VALUE2(uint32, "desired train profession", "first");
            uint32 chosen = 0;

            if (first == 164 || first == 202)
                chosen = 186; // Mining

            if (first == 171)
                chosen = 182; // Herbalism

            SET_AI_VALUE(uint32, "stored second profession", chosen);
            return chosen;
        }

        return 0;
    }
};

// Cdebug Values Trainable Spells

std::string TrainableSpellsValue::Format()
{
    std::ostringstream out;

    uint32 trainerType = 0;
    if (!getQualifier().empty())
        trainerType = atoi(getQualifier().c_str());

    auto spells = Calculate();

    out << "Trainable spells";
    if (trainerType)
        out << " [TrainerType " << trainerType << "]";
    out << " : " << spells.size() << "\n";

    for (auto& trainerSpell : spells)
    {
#ifdef MANGOSBOT_ZERO
        uint32 spellId = trainerSpell->learnedSpell;
#else
        uint32 spellId = trainerSpell->learnedSpell[0];
#endif

        const SpellEntry* proto = sSpellStore.LookupEntry(spellId);

        if (!proto)
            continue;

        uint32 skillLine = sSpellMgr.GetSpellSkill(spellId);

        out << "- " 
            << proto->SpellName[0]
            << " (SpellId: " << spellId
            << ", Skill: " << skillLine
            << ")\n";
    }

    return out.str();
}
