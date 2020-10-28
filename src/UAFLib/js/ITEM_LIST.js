/** TODO **/
function ITEM_LIST() {
    this.rdyItems_Deprecated = new READY_ITEMS();
    this.m_items = new CList();
}

ITEM_LIST.prototype.Clear = function () {
    /**TODO**/
}

ITEM_LIST.prototype.CanReady = function (rdyLoc, pChar, pItem) {
    throw "todo -- CanReady"
}

ITEM_LIST.prototype.CanUnReady = function(item) {
    if (!this.ValidItemListIndex(item))
        return false;
    if (!this.IsReady(item))
        return false;
    return (!this.IsCursed(item));
}

ITEM_LIST.prototype.CanReady = function (itemKey) {
    {
        var count;
        var pItemData;
        var actor;
        var result = "";
        var answer = false;
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        actor = pChar.GetContextActor();
        pItemData = itemData.GetItem(pItem.itemID);
        scriptContext.SetCharacterContext(pChar);
        scriptContext.SetItemContext(pItemData);
        scriptContext.SetItemContextKey(pItem.key);
        count = GetReadiedCount(rdyLoc);
        hookParameters[5] = count;
        result = pItemData.RunItemScripts(SPECAB.CAN_READY,
            SPECAB.ScriptCallback_RunAllScripts,
            null,
            "Test if item can be readied");
        answer = true;
        if (result == null || result == "") {
            answer = count == 0;
        }
        else {
            if (result[0] != 'Y') answer = false;
        };
        if (!(hookParameters[6] == null || hookParameters[6] == "")) {
            errorText = hookParameters[6];
            SetMiscError(ErrorText);
        }
        return answer;
    };
 }

ITEM_LIST.prototype.SetReady = function(index, rdyLoc) {
    var pos;
    if ((pos = this.m_items.FindKeyPos(index)) != null) {
        if (rdyLoc == Items.NotReady) {
            this.m_items.GetAtPos(pos).ReadyLocation(Items.NotReady); //readyLocation.Clear();
        }
        else {
            this.m_items.GetAtPos(pos).ReadyLocation(rdyLoc);
        };
    };
}

ITEM_LIST.prototype.IsReady = function(index) {
    var pos;
    if ((pos = this.m_items.FindKeyPos(index)) != null)
        return this.m_items.PeekAtPos(pos).ReadyLocation() != Items.NotReady;
    else
        return false; 
}



ITEM_LIST.prototype.UnReady = function (item) {
    if (!this.IsReady(item)) return true;
    if (!this.CanUnReady(item)) return false;
    this.SetReady(item, Items.NotReady);
    return true;
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

