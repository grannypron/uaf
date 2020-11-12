/** TODO **/
function ITEM() {
    this.key;
    this.itemID = new ITEM_ID(); // was GLOBAL_ITEM_ID m_giID;  // exactly one int in size.
    this.readyLocation = itemReadiedLocation.NotReady;
    this.pItemData = new ITEM_DATA();  // Not Serialized.....temporary cache
    this.qty = 0;
    this.identified = 0;
    this.charges = 0;
    this.cursed = 0;
    // how much was paid for item (important for selling)
    // -1 means not purchased, use base item cost in database
    this.paid = 0;
    this.m_scriptProcessed = false; // USed by the GPDL $ForEachItem() function
}

ITEM.prototype.Clear = function ()
{
    this.key = 0;
    this.itemID.Clear();
    this.readyLocation.Clear();
    this.qty = 0;
    this.identified = false;
    this.charges = 0;
    this.cursed = false;
    this.paid = -1;
    this.pItemData = null;
};

ITEM.prototype.SerializeCAR = function (ar, ver) {
    var count = 0;
    var data = new ITEM();
    var pos;

    if (ar.IsStoring()) {
        ar.writeInt(this.GetCount());
        pos = this.GetHeadPosition();
        while (pos != null) {
            this.GetNext(pos).SerializeCAR(ar, ver);
            pos = this.NextPos(pos);
        }
    }
    else {
        this.Clear();
        count = ar.readInt();
        for (i = 0; i < count; i++) {
            var pItem = new ITEM_DATA();
            data.SerializeCAR(ar, ver);
            pItem = ITEM_DATA.GetItem(data.itemID);
            if (pItem != null) {
                data.charges = pItem.Num_Charges;
                this.AddItemWithCurrKey(data);
            }
            else {
                Globals.debugSeverity = 7;
                WriteDebugString("Undefined item named %s\n", data.itemID);
            };
        }
    }

    this.rdyItems_Deprecated.SerializeCAR(ar);
}
ITEM.prototype.ClearReadyLocation = function () {
    this.readyLocation.Clear();
}

ITEM.prototype.GetReadyLocation = function() {
    return this.readyLocation;
}

ITEM.prototype.SetReadyLocation = function (readyLoc) {
    return this.readyLocation = new itemReadiedLocationObj(readyLoc);
}
/**TODO**
void Serialize(CArchive & ar, double ver);
void ReadyLocation(DWORD rdyLoc);
**/