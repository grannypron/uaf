function Items() {
    this.MAX_ITEMS = UAFUtil.ByteFromHexString("0x00FFFFFF");
}

Items.prototype.GetReadiedLocationByString = function (str) {
    if (str == null || str == "") {
        return this.Undefined;
    } else {
        return UAFUtil.StringToBase38(str);
    }
}

Items.prototype.WpnCanAttackAtRange = function (weaponID, Range) {
    if (weaponID.itemID) { weaponID = weaponID.itemID; }  // PORT NOTE:  Added this "unwrapping" because I used strings as IDs instead

    if (itemData.IsMoneyItem(weaponID))
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
                if (UAFUtil.IsEmpty(pItem.SpellID())) {
                    return (Range <= itemData.GetItemRange(weaponID));
                };
                pSpell = spellData.PeekSpell(pItem.SpellID());
                if ((pSpell == null) || (pSpell.Targeting != spellTargetingType.Self)) {
                    return (Range <= itemData.GetItemRange(weaponID));
                };
                return Range == 0;
            };
        default:
            Globals.WriteDebugString("Bogus item type in IsWeapon()\n");
            break;
    }
    return false;
}

Items.prototype.isMagical = function(itemID) {
    if (itemData.IsMoneyItem(itemID))
        return false;

    var data = itemData.GetItemFromID(itemID);
    if (data == null)
        return false;

    if (data.IsNotMagical())
        return false;

    if (data.Cursed)
        return true;

    if (data.Protection_Bonus)
        return true;

    if (data.Attack_Bonus)
        return true;

    if ((data.Dmg_Bonus_Lg) || (data.Dmg_Bonus_Sm))
        return true;

    if ((data.IsUsable()) && (data.spellID.IsValidSpell()))
        return true;

    if (data.specAbs.GetCount() != 0) {
        return true;
    }

    return false;
}

ITEM_LIST.prototype.GetAmmoClass = function(index) {
    var pos;
    if ((pos = this.m_items.FindKeyPos(index)) != null) {
        return itemData.GetAmmoClass(this.m_items.PeekAtPos(pos).itemID);
    }
    else
        return ""; 
}