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