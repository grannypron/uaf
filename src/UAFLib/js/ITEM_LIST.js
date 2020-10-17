/** TODO **/
function ITEM_LIST() {
    this.rdyItems_Deprecated = new READY_ITEMS();
    this.m_items = new CList();
}

ITEM_LIST.prototype.Clear = function () {
    /**TODO**/
}

ITEM_LIST.prototype.GetReadiedItem = function (rdyLoc, readiedItemCount) {
    var pos = null;
    var pCharItem;
    pos = this.GetHeadPosition();
    for (pCharItem = this.PeekNext(pos); pCharItem != null; pCharItem = this.PeekNext(pos)) {
        if (pCharItem.ReadyLocation() == rdyLoc) {
            if (readiedItemCount == 0) {
                return pCharItem.key;
            }
            else {
                readiedItemCount--;
            }
        }
    }
    return Items.NO_READY_ITEM;
}


ITEM_LIST.prototype.GetHeadPosition = function () {
    return this.m_items.GetHeadPosition();
}

ITEM_LIST.prototype.PeekFirst = function(pos) {
    pos = this.m_items.GetHeadPosition();
    return (pos == null) ? null : PeekAtPos(pos);
  }

ITEM_LIST.prototype.GetAtPos = function(pos)  {
    return this.m_items.GetAtPos(pos);
}

ITEM_LIST.prototype.PeekAtPos = function (pos) {
    return this.m_items.PeekAtPos(pos);
}

ITEM_LIST.prototype.PeekNext = function (pos) {
    return this.m_items.PeekNext(pos);
}

ITEM_LIST.prototype.GetItem = function (index) {
    var pos;
    if ((pos = this.m_items.FindKeyPos(index)) != null)
        return this.m_items.PeekAtPos(pos).itemID;
    else
        return new ITEM_ID();
}

ITEM_LIST.prototype.SerializeCAR = function(ar, version) {
    var count;
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
        for (i = 0; i < count; i++)
        {
            var pItem = new ITEM_DATA();
            data.SerializeCAR(ar, ver);
            //pItem = itemData.GetItemData(data.m_giID);
            pItem = itemData.GetItem(data.itemID);
            if (pItem != null) {
                data.charges = pIte.Num_Charges;
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

