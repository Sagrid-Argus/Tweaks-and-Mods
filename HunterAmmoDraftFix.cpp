// in ItemUsageValue::Calculate()
// ItemUsageValue.cpp l393

if (proto->Class == ammoClass && proto->SubClass == subClass)
            {
                uint32 currentAmmoId = bot->GetUInt32Value(PLAYER_AMMO_ID);
                const ItemPrototype* currentAmmoProto = nullptr;
                if (currentAmmoId)
                    currentAmmoProto = sObjectMgr.GetItemPrototype(currentAmmoId);

                float betterAmmoStacks = BetterStacks(proto, "ammo"); // how much better ammo we have
                float needAmmo = (bot->getClass() == CLASS_HUNTER) ? 8 : 2;

                if (ai->HasCheat(BotCheatMask::item))
                    needAmmo = 1;

                                    // fallback: equip any ammo if no ammo equipped
                if (!currentAmmoId)
                {
                                    // check if this proto exists in bags
                if (ai->HasItemInInventory(proto->ItemId))
                return ItemUsage::ITEM_USAGE_EQUIP;
                }

                // If no better ammo exists
                if (betterAmmoStacks <= 0)
                {
                    // Equip this ammo if not already equipped
                    if (currentAmmoId != proto->ItemId)
                        return ItemUsage::ITEM_USAGE_EQUIP;
                }

                // If this ammo is already equipped or after equipping
                float totalStacks = betterAmmoStacks + CurrentStacks(ai, proto);

                if (totalStacks < needAmmo)            // Not enough ammo, buy more
                    return ItemUsage::ITEM_USAGE_AMMO;
                else if (totalStacks < needAmmo + 1)   // Enough ammo, but keep it
                    return ItemUsage::ITEM_USAGE_KEEP;
            } // end proto class/subclass check
        } // end if pItem
    } // end class check
} // end AMMO check

///////////////////////////////////////////////

// in HunterActions.cpp l35

bool HunterEquipAmmoAction::Execute(Event& event)
{
    // Get ranged weapon
    Item* ranged = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
    if (!ranged)
        return false;

    uint32 ammoClass = ITEM_CLASS_PROJECTILE;
    uint32 subClass = 0;

    switch (ranged->GetProto()->SubClass)
    {
        case ITEM_SUBCLASS_WEAPON_GUN:
            subClass = ITEM_SUBCLASS_BULLET;
            break;
        case ITEM_SUBCLASS_WEAPON_BOW:
        case ITEM_SUBCLASS_WEAPON_CROSSBOW:
            subClass = ITEM_SUBCLASS_ARROW;
            break;
        case ITEM_SUBCLASS_WEAPON_THROWN:
            ammoClass = ITEM_CLASS_WEAPON;
            subClass = ITEM_SUBCLASS_WEAPON_THROWN;
            break;
    }

    uint32 currentAmmoId = bot->GetUInt32Value(PLAYER_AMMO_ID);

    // Scan inventory
    for (int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        if (Bag* bag = (Bag*)bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            for (uint32 j = 0; j < bag->GetBagSize(); ++j)
            {
                if (Item* item = bag->GetItemByPos(j))
                {
                    const ItemPrototype* proto = item->GetProto();
                    if (!proto)
                        continue;

                    if (proto->Class == ammoClass && proto->SubClass == subClass)
                    {
                        // Equip if different or none equipped
                        if (currentAmmoId != proto->ItemId)
                        {
                            bot->SetUInt32Value(PLAYER_AMMO_ID, proto->ItemId);
                            bot->UpdateDamagePhysical(RANGED_ATTACK);
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

////////////////////////////////////////

// HunterStrategy.cpp l93

void HunterStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "stealthed nearby",
        NextAction::array(0, new NextAction("flare", ACTION_NORMAL + 3), NULL)));
// equip ammo if no ammo equipped
    triggers.push_back(new TriggerNode(
    "no ammo",
    NextAction::array(0,
        new NextAction("equip ammo", ACTION_EMERGENCY),
        new NextAction("say::no ammo", ACTION_NORMAL),
        NULL)));
// 
    triggers.push_back(new TriggerNode(
        "no pet",
        NextAction::array(0, new NextAction("tame beast", ACTION_EMERGENCY), NULL)));
}

//////////////////////////////

// HunterAiObjectContext.cpp l243

class AiObjectContextInternal : public NamedObjectContext<Action>
        {
        public:
            AiObjectContextInternal()
            {
                creators["auto shot"] = [](PlayerbotAI* ai) { return new CastAutoShotAction(ai); };
                creators["equip ammo"] = [](PlayerbotAI* ai) { return new HunterEquipAmmoAction(ai); }; 

/////////////////////////////////////

// HunterActions.h l18

    class HunterEquipAmmoAction : public Action
{
public:
    HunterEquipAmmoAction(PlayerbotAI* ai) : Action(ai, "equip ammo") {}
    virtual bool Execute(Event& event) override;
};

////////////////////////////////////

// HunterTriggers.h l204

class HunterNoAmmoTrigger : public AmmoCountTrigger
    {
    public:
        HunterNoAmmoTrigger(PlayerbotAI* ai) : AmmoCountTrigger(ai, "ammo", 1, 10) {}
        virtual bool IsActive() override
    {
        uint32 ammoId = bot->GetUInt32Value(PLAYER_AMMO_ID);

        // No ammo equipped at all
        if (ammoId == 0)
        return AI_VALUE2(uint32, "item count", "ammo") > 0;

        // Check if we still have THIS ammo in inventory
        uint32 count = AI_VALUE2(uint32, "item count", std::to_string(ammoId));

        // If equipped ammo is gone, but we have other ammo → trigger
        return count == 0 && AI_VALUE2(uint32, "item count", "ammo") > 0;
    }
    };
