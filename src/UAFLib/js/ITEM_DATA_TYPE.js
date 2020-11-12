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
    while (enumerator.MoveNext()) {
        key = enumerator.Current;
        var itemRecord = data[key];
        if (itemRecord["id_name"] == null || itemRecord["id_name"] == "") { continue; }
        var item = new ITEM_DATA();
        item.m_uniqueName = itemRecord["name"];
        item.m_commonName = itemRecord["id_name"];
        item.m_idName = itemRecord["name"];
        item.AmmoType = itemRecord["ammo_type"];
        item.HitSound = itemRecord["hit_sound"];
        item.MissSound = itemRecord["miss_sound"];
        item.LaunchSound = itemRecord["launch_sound"];
        item.Experience = parseInt(itemRecord["experience"]);
        item.Cost = parseInt(itemRecord["cost"]);
        item.Encumbrance = parseInt(itemRecord["encumbrance"]);
        item.Attack_Bonus = parseInt(itemRecord["attack_bonus"]);
        item.Cursed = itemRecord["cursed"] == "yes";
        item.Bundle_Qty = parseInt(itemRecord["bundle_quantity"]);
        item.Num_Charges = parseInt(itemRecord["number_of_charges"]);
        item.Location_Readied = Items.GetReadiedLocationByString(itemRecord["readied_location"]);
        item.Hands_to_Use = parseInt(itemRecord["hands_to_carry"]);
        var sm_dice = UAFUtil.parseDice(itemRecord["small_medium_damage_dice"]);
        item.Dmg_Dice_Sm = sm_dice.die;
        item.Nbr_Dice_Sm = sm_dice.nbr;
        item.Dmg_Bonus_Sm = sm_dice.bonus;
        var lg_dice = UAFUtil.parseDice(itemRecord["large_damage_dice"]);
        item.Dmg_Dice_Lg = lg_dice.die;
        item.Nbr_Dice_Lg = lg_dice.nbr;
        item.Dmg_Bonus_Lg = lg_dice.bonus;
        item.ROF_Per_Round = parseFloat(itemRecord["ROF_per_round"]);
        item.Protection_Base = parseInt(itemRecord["AC_base"]);
        item.Protection_Bonus = parseInt(itemRecord["AC_bonus"]);
        item.Wpn_Type = weaponClassType.NotWeapon;
        item.m_priorityAI = 0; // Not serialized.  Initialized from Special Ability.
        item.m_usageFlags = 0;
        item.USAGE_usable = itemRecord["usable"] == "yes";
        item.USAGE_scribable = itemRecord["scribtable"] == "yes";
        item.USAGE_notMagical = itemRecord["not_magical"] == "yes";
        item.CanBeHalvedJoined = itemRecord["CanHalveJoin"] == "yes";
        item.CanBeTradeDropSoldDep = itemRecord["can_drop"] == "yes";
        item.RangeMax = parseInt(itemRecord["combat_range"]);
        item.RangeShort = (this.RangeMax + 2) / 3; // These may be overridden by $AI_ShortRange
        item.RangeMedium = 2 * this.RangeMax / 3;  // and $AI_MediumRange Special Abilities. 
        item.ExamineEvent = parseInt(itemRecord["Examine_Event_ID"]);  //DWORD
        item.ExamineLabel = itemRecord["Examine_Label"];
        item.attackMsg = "";
        item.Recharge_Rate = itemRechargeRate.GetByString(itemRecord["recharge_rate"]);
        item.IsNonLethal = itemRecord["non_lethal"] == "yes";
        item.usableByBaseclass = [];
        /**TODO 
        item.MissileArt = new PIC_DATA(); // in-route sprite
        item.HitArt = new PIC_DATA();     // target hit sprite
        item.item_asl = new A_ASLENTRY_L();
        item.temp_asl = new A_ASLENTRY_L();
        item.spellID = itemRecord["spell_effect"] ?????
        item.specAbs = new SPECIAL_ABILITIES(true);
        */
        this.AddItem(item);

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