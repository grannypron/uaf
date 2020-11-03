/** TODO **/
function ITEM_DATA_TYPE() {
    this.mItemDataTypes = new CList();
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

ITEM_DATA_TYPE.prototype.PeekItem = function(itemID) {
    /** TODO: itemID is type ITEM_ID - lookup by that datatype somehow **/
    return null;
}

ITEM_DATA_TYPE.prototype.GetItem = function (id) {
    return this.mItemDataTypes.GetAtPos(id);
}

ITEM_DATA_TYPE.prototype.FreeArt = function() {
    var i, n;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.GetItem(i).FreeArt();
    }
}

ITEM_DATA_TYPE.prototype.GetCount = function () {
    return this.mItemDataTypes.GetCount();
}

ITEM_DATA_TYPE.prototype.ClearSounds = function() {
    var i, n;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.GetItem(i).ClearSounds();
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
};

ITEM_DATA_TYPE.prototype.IsGemType = function(itemID) {
    return itemID == GemString;
};

ITEM_DATA_TYPE.prototype.IsJewelryType = function(itemID) {
    return itemID == JewelryString;
};

ITEM_DATA_TYPE.prototype.IsMoney = function(itemID) {
    return this.IsGemType(itemID) || this.IsJewelryType(itemID);
};