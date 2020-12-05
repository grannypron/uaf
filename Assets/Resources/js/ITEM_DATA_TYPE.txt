/** TODO - make the ItemData an associative indexed array for speed's sake **/
function ITEM_DATA_TYPE() {
    this.ItemData = new CList();
    this.m_AmmoTypes = new CList();
}

ITEM_DATA_TYPE.prototype.GetWpnType = function(itemID) {
    var pItem;

    pItem = this.PeekItem(itemID);
    if (pItem != null) {
        return pItem.Wpn_Type;
    }
    else
        return weaponClassType.NotWeapon;
}

ITEM_DATA_TYPE.prototype.PeekItem = function (itemID) {
    if (UAFUtil.IsEmpty(itemID)) { return null; }
    return this.ItemData.GetAtPos(this.LocateItem(itemID));
}

ITEM_DATA_TYPE.prototype.GetItemFromID = function (itemID) {
    if (UAFUtil.IsEmpty(itemID)) { return null; }
    return this.ItemData.GetAtPos(this.LocateItem(itemID));
}

ITEM_DATA_TYPE.prototype.GetItemByPos = function (idx) {
    return this.ItemData.GetAtPos(idx);
}

ITEM_DATA_TYPE.prototype.FreeArt = function () {
    var i, n;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.GetItemByPos(i).FreeArt();
    }
}

ITEM_DATA_TYPE.prototype.GetCount = function () {
    return this.ItemData.GetCount();
}

ITEM_DATA_TYPE.prototype.ClearSounds = function() {
    var i, n;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.GetItemByPos(i).ClearSounds();
    };
}

ITEM_DATA_TYPE.prototype.WpnConsumesSelfAsAmmo = function (weaponID) {
    if (this.IsMoneyItem(weaponID))
        return false;
    return (this.WpnConsumesSelfAsAmmo(itemData.GetWpnType(weaponID)));
}


ITEM_DATA_TYPE.prototype.WpnConsumesSelfAsAmmo = function (weaponID) {
    if (this.IsMoneyItem(weaponID))
        return false;
    return (this.WpnConsumesSelfAsAmmoType(itemData.GetWpnType(weaponID)));
}

ITEM_DATA_TYPE.prototype.WpnConsumesSelfAsAmmoType = function(type) {
    switch (type) {
        case weaponClassType.HandBlunt:
        case weaponClassType.HandCutting:
        case weaponClassType.SlingNoAmmo:
        case weaponClassType.Bow:
        case weaponClassType.Crossbow:
            return false;

        case weaponClassType.HandThrow: // dec item qty by 1 when item is thrown
        case weaponClassType.Throw:
        case weaponClassType.SpellCaster:
        case weaponClassType.SpellLikeAbility:
            return true;
    }
    return false;
}

/**PARAMCHECK: WeaponID should be a string **/
ITEM_DATA_TYPE.prototype.WpnConsumesAmmoAtRange = function(weaponID, Range)
{
    if (this.IsMoneyItem(weaponID))
        return false;

    switch (itemData.GetWpnType(weaponID)) {
        case weaponClassType.HandBlunt:
        case weaponClassType.HandCutting:
        case weaponClassType.SlingNoAmmo:
        case weaponClassType.SpellCaster:
        case weaponClassType.SpellLikeAbility:
            return false;

        case weaponClassType.HandThrow: // dec item qty by 1 when item is thrown
        case weaponClassType.Throw:
            return (Range > 1);

        case weaponClassType.Bow:
        case weaponClassType.Crossbow:  // dec ammo qty by 1
            return true;
    }
    return false;
}


// PORT NOTE:  Rewrote these a little
ITEM_DATA_TYPE.prototype.IsMoneyItem = function (itemID) {
    return this.IsMoney(itemID);
}

ITEM_DATA_TYPE.prototype.IsGemType = function(itemID) {
    return itemID == GemString;
}

ITEM_DATA_TYPE.prototype.IsJewelryType = function(itemID) {
    return itemID == JewelryString;
}

ITEM_DATA_TYPE.prototype.IsMoney = function(itemID) {
    return this.IsGemType(itemID) || this.IsJewelryType(itemID);
}


ITEM_DATA_TYPE.prototype.LocateItem = function (itemID) {
    var i = 0, n = 0;
    n = this.GetCount();
    if ((itemID.hint >= 0)
        && (itemID.hint < n)
        && (this.ItemData.GetAtPos(itemID.hint).m_uniqueName == itemID)) return itemID.hint;
    for (i = 0; i < n; i++) {
        if (this.ItemData.GetAtPos(i).m_uniqueName == itemID) {
            itemID.hint = i;            // PORT NOTE:  Even though this will not be used because no PBR in JS
            return i;
        };
    };
    return -1;
}

ITEM_DATA_TYPE.prototype.IsValidItem = function (weaponID) {
    return itemData.LocateItem(weaponID) >= 0;
}

ITEM_DATA_TYPE.prototype.IsNoItem = function (itemID) {
    return UAFUtil.IsEmpty(itemID);
}


ITEM_DATA_TYPE.prototype.AddItem = function(data) {
    // Returns the CArray index of the new item
    var itemID;
    var result;
    if (data.UniqueName().length == 0) return -1;
    // Do not allow multiple items with same full name.
    itemID = data.ItemID();
    if (this.LocateItem(itemID) >= 0) return -1;
    if (this.GetCount() < Items.MAX_ITEMS) {
        result = this.ItemData.Add(data);
        this.AddAmmoType(data.AmmoType);
        return result;
    }
    else
        return -1;
}

ITEM_DATA_TYPE.prototype.AddAmmoType = function(data) {
    data = data.trim();
    if (UAFUtil.IsEmpty(data)) return;
    if (this.HaveAmmoType(data) == -1)
        this.m_AmmoTypes.SetAtGrow(this.m_AmmoTypes.GetCount(), data);          // PORT NOTE: Changed from GetSize() to GetCount()
}



ITEM_DATA_TYPE.prototype.LoadFromLoader = function (data) {
    var enumerator = data.Keys.GetEnumerator();
    var performanceIdxCounter = 0;
    while (enumerator.MoveNext()) {
        var startTime;
        key = enumerator.Current;
        var itemRecord = data[key];
        if (itemRecord["id_name"][0] == null || itemRecord["id_name"][0] == "") { continue; }
        var itemData = new ITEM_DATA();
        itemData.m_uniqueName = itemRecord["name"][0];
        itemData.m_commonName = itemRecord["id_name"][0];
        itemData.m_idName = itemRecord["name"][0];
        itemData.AmmoType = itemRecord["ammo_type"][0];
        itemData.HitSound = itemRecord["hit_sound"][0];
        itemData.MissSound = itemRecord["miss_sound"][0];
        itemData.LaunchSound = itemRecord["launch_sound"][0];
        itemData.Experience = parseInt(itemRecord["experience"][0]);
        itemData.Cost = parseInt(itemRecord["cost"][0]);
        itemData.Encumbrance = parseInt(itemRecord["encumbrance"][0]);
        itemData.Attack_Bonus = parseInt(itemRecord["attack_bonus"][0]);
        itemData.Cursed = itemRecord["cursed"][0] == "yes";
        itemData.Bundle_Qty = parseInt(itemRecord["bundle_quantity"][0]);
        itemData.Num_Charges = parseInt(itemRecord["number_of_charges"][0]);
        itemData.Location_Readied = Items.GetReadiedLocationByString(itemRecord["readied_location"][0]);
        itemData.Hands_to_Use = parseInt(itemRecord["hands_to_carry"][0]);
        var sm_dice = UAFUtil.parseDice(itemRecord["small_medium_damage_dice"][0]);
        itemData.Dmg_Dice_Sm = sm_dice.die;
        itemData.Nbr_Dice_Sm = sm_dice.nbr;
        itemData.Dmg_Bonus_Sm = sm_dice.bonus;
        var lg_dice = UAFUtil.parseDice(itemRecord["large_damage_dice"][0]);
        itemData.Dmg_Dice_Lg = lg_dice.die;
        itemData.Nbr_Dice_Lg = lg_dice.nbr;
        itemData.Dmg_Bonus_Lg = lg_dice.bonus;
        itemData.ROF_Per_Round = parseFloat(itemRecord["ROF_per_round"][0]);
        itemData.Protection_Base = parseInt(itemRecord["AC_base"][0]);
        itemData.Protection_Bonus = parseInt(itemRecord["AC_bonus"][0]);
        itemData.Wpn_Type = weaponClassType.getByString(itemRecord["weapon_type"][0]);
        itemData.m_priorityAI = 0; // Not serialized.  Initialized from Special Ability.
        itemData.m_usageFlags = 0;
        itemData.USAGE_usable = itemRecord["usable"][0] == "yes";
        itemData.USAGE_scribable = itemRecord["scribtable"][0] == "yes";
        itemData.USAGE_notMagical = itemRecord["not_magical"][0] == "yes";
        itemData.CanBeHalvedJoined = itemRecord["CanHalveJoin"][0] == "yes";
        itemData.CanBeTradeDropSoldDep = itemRecord["can_drop"][0] == "yes";
        itemData.RangeMax = parseInt(itemRecord["combat_range"][0]);
        itemData.RangeShort = (this.RangeMax + 2) / 3; // These may be overridden by $AI_ShortRange
        itemData.RangeMedium = 2 * this.RangeMax / 3;  // and $AI_MediumRange Special Abilities. 
        itemData.ExamineEvent = parseInt(itemRecord["Examine_Event_ID"][0]);  //DWORD
        itemData.ExamineLabel = itemRecord["Examine_Label"][0];
        itemData.attackMsg = "";
        itemData.Recharge_Rate = itemRechargeRate.GetByString(itemRecord["recharge_rate"][0]);
        itemData.IsNonLethal = itemRecord["non_lethal"][0] == "yes";
        itemData.usableByBaseclass = [];
        var baseClasses = itemRecord["BaseClasses"];
        for (var idx = 0; idx < baseClasses.Count; idx++) {
            itemData.usableByBaseclass.push(baseClasses[idx]);
        }
        /**TODO 
        item.MissileArt = new PIC_DATA(); // in-route sprite
        item.HitArt = new PIC_DATA();     // target hit sprite
        item.item_asl = new A_ASLENTRY_L();
        item.temp_asl = new A_ASLENTRY_L();
        item.spellID = itemRecord["spell_effect"] ?????
        item.specAbs = new SPECIAL_ABILITIES(true);
        */
        this.AddItem(itemData);
        performanceIdxCounter++;
    }
}

ITEM_DATA_TYPE.prototype.HaveAmmoType = function (data) {
    if (UAFUtil.IsEmpty(data)) return -1;
    for (var i = 0; i < this.m_AmmoTypes.GetCount(); i++)                  // PORT NOTE: Changed from GetSize() to GetCount()
    {
        if (this.m_AmmoTypes.PeekAt(i) == data)             // PORT NOTE: Changed to PeekAt from [] because can't implement [] on CList object in JS
            return i;
    }
    return -1;
}

ITEM_DATA_TYPE.prototype.getItemEncumbrance = function(itemID, qty) {
    if (qty <= 0) return 0;

    if (this.IsMoneyItem(itemID))
    {
        if (globalData.moneyData.GetWeight() > 0) {
            // GetWeight() returns number of coins
            // per unit of encumbrance
            var result = qty / globalData.moneyData.GetWeight();
            if (result <= 0) result = 1;
            return result;
        }
        else
            return qty;
    }

    var data = itemData.GetItemFromID(itemID);
    if (data == null)
        return 0;
    var bundle = Math.max(data.Bundle_Qty, 1);

    // if bundle qty > 1 then stated encumbrance is for
    // the whole group of items. Figure out the encumbrance
    // for just one item.
    var encumbrance = (data.Encumbrance / bundle);
    /*
    // item encumbrance is stated in terms of number of gold peices.
    // the encumbrance rating given from strength table is in terms
    // of pounds. Divide item encumbrance by number of GP per pound
    int w = globalData.moneyData.GetWeight();
    encumbrance /= w;
    */

    // multiply encumbrance by qty of this item
    encumbrance = qty;
    return Math.max(encumbrance, 0);
}

ITEM_DATA_TYPE.prototype.itemUsesRdySlot = function(pItem) {
    if (pItem == null) return false;
    return (!itemReadiedLocation.Cannot.EqualsDWORD(pItem.Location_Readied));
}

ITEM_DATA_TYPE.prototype.WpnCanAttackAtRange = function(weaponID, Range) {
    if (weaponID != null) { Globals.ASSERT(!weaponID.itemID, "ITEM_DATA_TYPE.WpnCanAttackAtRange - parameter is of an incorrect type"); }  // PORT NOTE: Check for wrong type of ID passed in

    if (this.IsMoneyItem(weaponID))
        return false;

    switch (itemData.GetWpnType(weaponID)) {
        case weaponClassType.NotWeapon:
        case weaponClassType.Ammo:
            return false;

        case weaponClassType.SlingNoAmmo:
        case weaponClassType.Bow:
        case weaponClassType.Crossbow:
        case weaponClassType.Throw:
            return ((Range >= 2)
                && (Range <= itemData.GetItemRange(weaponID)));

        case weaponClassType.HandBlunt:
        case weaponClassType.HandCutting:
        case weaponClassType.HandThrow:
            return (Range <= itemData.GetItemRange(weaponID));
        case weaponClassType.SpellCaster:
        case weaponClassType.SpellLikeAbility:
            {
                var pSpell;
                var pItem;
                pItem = itemData.PeekItem(weaponID);
                if (pItem.SpellID().IsEmpty()) {
                    return (Range <= itemData.GetItemRange(weaponID));
                };
                pSpell = spellData.PeekSpell(pItem.SpellID());
                if ((pSpell == null) || (pSpell.Targeting != Self)) {
                    return (Range <= itemData.GetItemRange(weaponID));
                };
                return Range == 0;
            };
        default:
            WriteDebugString("Bogus item type in IsWeapon()\n");
            break;
    }
    return false;
}

ITEM_DATA_TYPE.prototype.GetItemRange = function(itemID) {
    if (itemID != null) { Globals.ASSERT(!itemID.itemID, "ITEM_DATA_TYPE.GetItemRange - parameter is of an incorrect type"); }  // PORT NOTE: Check for wrong type of ID passed in
    var pItem;
    pItem = this.PeekItem(itemID);
    if (pItem != null) {
        return pItem.RangeMax;
    }
    else
        return 0;
}

ITEM_DATA_TYPE.prototype.WpnUsesAmmo = function (itemID) {
    if (itemID != null) { Globals.ASSERT(!itemID.itemID, "ITEM_DATA_TYPE.WpnUsesAmmo - parameter is of an incorrect type"); }  // PORT NOTE: Check for wrong type of ID passed in
    if (this.IsMoneyItem(itemID))
        return false;
    return (this.WpnUsesAmmoType(itemData.GetWpnType(itemID)));
}

ITEM_DATA_TYPE.prototype.WpnUsesAmmoType = function (type) {
    switch (type) {
        case weaponClassType.NotWeapon:
        case weaponClassType.Ammo:
        case weaponClassType.HandBlunt:
        case weaponClassType.HandCutting:
        case weaponClassType.HandThrow:
        case weaponClassType.SlingNoAmmo:
        case weaponClassType.Throw:
        case weaponClassType.SpellCaster:
        case weaponClassType.SpellLikeAbility:
            return false;
        case weaponClassType.Bow:
        case weaponClassType.Crossbow:
            return true;
        default:
            Globals.WriteDebugString("Bogus item type in WpnUsesAmmo()\n");
            break;
    }
    return false;
}

ITEM_DATA_TYPE.prototype.GetAmmoClass = function (itemID) {
    if (itemID != null) { Globals.ASSERT(!itemID.itemID, "ITEM_DATA_TYPE.GetAmmoClass - parameter is of an incorrect type"); }  // PORT NOTE: Check for wrong type of ID passed in
    var pItem;
    pItem = this.PeekItem(itemID);
    if (pItem != null) {
        return pItem.AmmoType;
    }
    else
        return "";
}


ITEM_DATA_TYPE.prototype.PlayHit = function(itemID) {
    if (itemID != null) { Globals.ASSERT(!itemID.itemID, "ITEM_DATA_TYPE.PlayHit - parameter is of an incorrect type"); }  // PORT NOTE: Check for wrong type of ID passed in
    var pItem;
    pItem = this.GetItemFromID(itemID);
    if (pItem != null) {
        pItem.PlayHit();
    }
}

ITEM_DATA_TYPE.prototype.PlayMiss = function(itemID) {
    if (itemID != null) { Globals.ASSERT(!itemID.itemID, "ITEM_DATA_TYPE.PlayMiss - parameter is of an incorrect type"); }  // PORT NOTE: Check for wrong type of ID passed in
    var pItem;
    pItem = this.GetItemFromID(itemID);
    if (pItem != null) {
        pItem.PlayMiss();
    }
}
