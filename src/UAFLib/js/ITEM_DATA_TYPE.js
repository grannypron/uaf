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

var ITEM_DATA_TYPE = new ITEM_DATA_TYPE();