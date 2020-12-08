/** TODO **/
function ITEM_LIST() {
    this.rdyItems_Deprecated = new READY_ITEMS();
    this.m_items = new OrderedQueue();
}

ITEM_LIST.prototype.Clear = function () {
    this.rdyItems_Deprecated = new READY_ITEMS();
    this.m_items = new OrderedQueue();
}


ITEM_LIST.prototype.CanUnReady = function(item) {
    if (!this.ValidItemListIndex(item))
        return false;
    if (!this.IsReady(item))
        return false;
    return (!this.IsCursed(item));
}

ITEM_LIST.prototype.CanReady = function (rdyLoc, pChar, pItem) {
    {
        var count;
        var pItemData;
        var actor;
        var result = "";
        var answer = false;
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        actor = pChar.GetContextActor();
        pItemData = itemData.GetItemFromID(pItem.itemID);
        scriptContext.SetCharacterContext(pChar);
        scriptContext.SetItemContext(pItemData);
        scriptContext.SetItemContextKey(pItem.key);
        count = this.GetReadiedCount(rdyLoc);
        hookParameters[5] = "" + count;
        result = pItemData.RunItemScripts(SPECAB.CAN_READY,
            SPECAB.ScriptCallback_RunAllScripts,
            null,
            "Test if item can be readied");
        answer = true;
        if (UAFUtil.IsEmpty(result)) {
            answer = count == 0;
        }
        else {
            if (result[0] != 'Y') answer = false;
        };
        if (!UAFUtil.IsEmpty(hookParameters[6])) {
            errorText = hookParameters[6];
            SetMiscError(ErrorText);
        }
        return answer;
    };
 }

ITEM_LIST.prototype.GetReadiedCount = function(rdyLoc) {
    var pos;
    var result = 0;
    var pItem = null;
    var pItemData;
    pos = this.m_items.GetHeadPosition();
    if (pos != null) pItem = this.PeekAtPos(pos);
    result = 0;
    while (pItem != null) {
        if (!itemReadiedLocation.NotReady.Equals(pItem.GetReadyLocation())) {
            pItemData = itemData.GetItemFromID(pItem.itemID);
            if (pItemData.Location_Readied == rdyLoc) result++;
        };
        pItem = this.m_items.PeekNext(pos);
        pos = this.m_items.NextPos(pos);
    };
    return result;
}


ITEM_LIST.prototype.SetReady = function(index, rdyLoc) {
    var pos;
    if ((pos = this.m_items.FindKeyPos(index)) != null) {
        if (itemReadiedLocation.NotReady.EqualsDWORD(rdyLoc)) {
            this.m_items.GetAtPos(pos).SetReadyLocation(itemReadiedLocation.NotReady); //readyLocation.Clear();
        }
        else {
            this.m_items.GetAtPos(pos).SetReadyLocation(rdyLoc);
        };
    };
}

ITEM_LIST.prototype.IsReady = function(index) {
    var pos;
    if ((pos = this.m_items.FindKeyPos(index)) != null)
        return !itemReadiedLocation.NotReady.Equals(this.m_items.PeekAtPos(pos).GetReadyLocation());
    else
        return false; 
}



ITEM_LIST.prototype.UnReady = function (item) {
    if (!this.IsReady(item)) return true;
    if (!this.CanUnReady(item)) return false;
    this.SetReady(item, itemReadiedLocation.NotReady);
    return true;
}

ITEM_LIST.prototype.GetReadiedItem = function (rdyLoc, readiedItemCount) {
    var pos = null;
    var pCharItem;
    pos = this.GetHeadPosition();
    //PORT NOTE: I don't know why the pCharItem is being set to PeekNext here.  It seems like the item at the head position would never be checked.  Changed in my code, but the original line is below
    // for (pCharItem=PeekNext(pos); pCharItem!=NULL; pCharItem=PeekNext(pos))
    for (pCharItem = this.PeekAtPos(pos); pCharItem != null; pCharItem = this.PeekAtPos(pos)) {
        if (pCharItem.GetReadyLocation().Equals(rdyLoc)) {
            if (readiedItemCount == 0) {
                return pos;//pCharItem.key;     // PORT NOTE:  Changed to use array-index based indexing
            }
            else {
                readiedItemCount--;
            }
        }
        pos = this.NextPos(pos);           //  PORT NOTE:  Added this - no "auto"-increment because no pass-by-reference
    }

    return NO_READY_ITEM;
}


ITEM_LIST.prototype.GetHeadPosition = function () {
    return this.m_items.GetHeadPosition();
}

ITEM_LIST.prototype.PeekFirst = function(pos) {
    pos = this.m_items.GetHeadPosition();
    return (pos == null) ? null : this.PeekAtPos(pos);
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

ITEM_LIST.prototype.GetItem = function (index, data) {
    if (data) { throw "Pass-by-reference not supported in this function:  ITEM_LIST.prototype.GetItem";}  // PORT NOTE:  Doing this check because I don't want bugs to creep in since there is no method signature checking
    return this.GetAtPos(index);    // PORT NOTE:  Simplified with array indexing scheme
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
        for (var i = 0; i < count; i++)
        {
            var pItem = new ITEM_DATA();
            data.SerializeCAR(ar, ver);
            //pItem = itemData.GetItemData(data.m_giID);
            pItem = itemData.GetItemFromID(data.itemID);
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

ITEM_LIST.prototype.GetProtectModForRdyItems = function() {
    var acMod = 0;
    var pos;
    pos = this.m_items.GetHeadPosition();
    while (pos != null) {
        if (this.PeekAtPos(pos).GetReadyLocation() != itemReadiedLocation.NotReady) {
            var data = itemData.GetItemFromID(this.PeekAtPos(pos).itemID);
            if (data != null)
                acMod += (data.Protection_Base + data.Protection_Bonus);
        }
        pos = this.NextPos(pos);
    }
    return acMod;
}

//*****************************************************************************
// NAME: addItem
//
// PURPOSE:  Add a single item to the inventory (or single bundle qty)
//*****************************************************************************
ITEM_LIST.prototype.addItem5 = function(itemID, qty, numCharges, id, cost) {
    if (qty < 1)
        return false;

    var newItem = new ITEM();

    if (itemData.IsMoneyItem(itemID))
    {
        if (numCharges < 0)
            numCharges = 0;

        newItem.itemID = itemID;
        newItem.qty = qty;
        newItem.ClearReadyLocation();
        newItem.charges = numCharges;
        newItem.identified = id;
        newItem.cursed = false;
    }
    else
    {
        var theItem = itemData.GetItemByPos(itemData.LocateItem(itemID));     // PORT NOTE: Had to add the call to LocateItem - not sure how this worked with GetItem as I have seen GetItem being called with an integer index in other places
        if (theItem == null)
            return false;

        if ((theItem.Bundle_Qty <= 1) && (qty > 1)) {
            Globals.WriteDebugString("qty > 1 && BundleQty <= 1 in addItem()\n");
            qty = 1;
        }

        if (numCharges < 0)
            numCharges = theItem.Num_Charges;

        newItem.itemID = itemID;
        newItem.qty = qty;
        newItem.ClearReadyLocation();
        newItem.charges = numCharges;
        newItem.identified = id;
        newItem.cursed = theItem.Cursed;
        newItem.paid = cost;
    }

    return this.AddItem(newItem, false); // no auto-join
}

ITEM_LIST.prototype.AddItem = function (data, AutoJoin) {
    if (AutoJoin == null || AutoJoin == undefined) {
        AutoJoin = true;
    }

    var newkey = 0;
    var joined = false;
    if (AutoJoin) {
        if (itemData.itemCanBeJoined(data.itemID)) {
            // look for another instance of item
            var joinIndex = -1;
            var pos = this.GetHeadPosition();
            while ((pos != null) && (joinIndex < 0)) {
                if ((this.PeekAtPos(pos).itemID == data.itemID))
                    joinIndex = this.PeekAtPos(pos).key;
                this.GetNext(pos); pos = this.NextPos(pos);
            }

            // if item instance in list
            if (joinIndex >= 0) {
                var joinItem = new ITEM();
                joinItem.CopyConstructor(this.GetAtPos(joinIndex));
                joinItem.qty += data.qty;
                this.SetItem(joinIndex, joinItem);
                joined = true;
                newkey = joinIndex;
            }
        }
    }

    if (!joined) {
        if (this.m_items.GetCount() < Items.MAX_ITEMS) {
            data.key = this.GetNextKey();
            this.m_items.Insert(data, data.key);
            newkey = data.key;
        }
    }
    return newkey;
}

ITEM_LIST.prototype.GetNextKey = function () {
    return this.m_items.GetCount();                  // PORT NOTE:  Simplified this
}

ITEM_LIST.prototype.NextPos = function (pos) {          // PORT NOTE:  Adding because pos is not "auto"-incremented as a pass-by-reference parameter
    return this.m_items.NextPos(pos);
}

ITEM_LIST.prototype.CanReadyKey = function (itemKey, pChar) {
    var pos;
    var pCharItem;
    if ((pos = this.m_items.FindKeyPos(itemKey)) == null)
        return miscErrorType.UnknownError;

    pCharItem = this.m_items.PeekAtPos(pos);
    return this.CanReadyItem(pCharItem, pChar);

}

ITEM_LIST.prototype.CanReadyItem = function(pCharItem, pChar) {
    if (itemData.IsMoneyItem(pCharItem.itemID))
        return miscErrorType.UnknownError;

    if (!pCharItem.GetReadyLocation().Equals(itemReadiedLocation.NotReady))
        return miscErrorType.NoError;

    if (pCharItem.qty <= 0)
        return miscErrorType.UnknownError;

    var pItem = itemData.GetItemFromID(pCharItem.itemID);
    if (pItem == null)
        return miscErrorType.UnknownError;

    // dont handle items using more than 2 hands yet
    if (pItem.Hands_to_Use > 2)
        return miscErrorType.UnknownError;

      /* The item is usable by this character's class if any of his current sub-classes can
       * use the item.
       */
    if (!pItem.IsUsableByClass(pChar)) {
        return miscErrorType.WrongClass;
    }
    if (itemData.itemUsesRdySlot(pItem)) {
        if ((pItem.Hands_to_Use == 2)
            && ((itemReadiedLocation.WeaponHand.EqualsDWORD(pItem.Location_Readied)) || itemReadiedLocation.ShieldHand.EqualsDWORD(pItem.Location_Readied))) {
            if ((this.GetReadiedItem(itemReadiedLocation.WeaponHand, 0) != NO_READY_ITEM)
                || (this.GetReadiedItem(itemReadiedLocation.ShieldHand, 0) != NO_READY_ITEM)) {
                return TakesTwoHands;
            }
        }
        else if (pItem.Hands_to_Use > 0) {
            // PORT NOTE: Not sure why this was so complicated but I decided to simplify and I hope I didn't miss anything
            // pItem.Hands_to_Use is necessarily 1
            var pItemIdx = this.GetReadiedItem(itemReadiedLocation.WeaponHand, 0);
            if (pItemIdx != null && pItemIdx >= 0) {
                var pItem = this.GetAtPos(pItemIdx);
                if (pItem != null) {
                    var pItemData = itemData.GetItemFromID(pItem.itemID);
                    if (pItemData.Hands_to_Use > 1) {
                        return miscErrorType.NoFreeHands;
                    }
                }
            }

            /*
            var readiedItem = 0, hand = 0;
            var rdyLoc = 0;
            for (hand = 0, itemReadiedLocation.WeaponHand.EqualsDWORD(rdyLoc); hand < 2; hand++, itemReadiedLocation.ShieldHand.EqualsDWORD(rdyLoc)) {
                readiedItem = this.GetReadiedItem(rdyLoc, 0);
                if (readiedItem != NO_READY_ITEM) {
                    var readiedPos;
                    readiedPos = this.m_items.FindKeyPos(readiedItem);
                    if (readiedPos != null) {
                        var pCharReadiedItem;
                        var pReadiedItem;
                        pCharReadiedItem = this.m_items.PeekAtPos(readiedPos);
                        pReadiedItem = itemData.GetItemFromID(pCharReadiedItem.itemID);
                        if (pReadiedItem != null) {
                            if (pReadiedItem.Hands_to_Use > 1) {
                                return miscErrorType.NoFreeHands;
                            }
                        }
                    }
                }
            }
                */
        }
        if (!this.CanReady(pItem.Location_Readied, pChar, pCharItem)) {
            {
                return miscErrorType.ItemAlreadyReadied;
            };
        }
    }

    return miscErrorType.NoError;
}

ITEM_LIST.prototype.Ready = function(itemKey, pChar, rdyLoc) {
    var pItem = itemData.GetItemFromID(this.GetItemIDByPos(itemKey));
    if (pItem == null) return false;

    if (this.CanReadyKey(itemKey, pChar) != miscErrorType.NoError) return false;

    var result = true;
    if (result)
        this.SetReady(itemKey, rdyLoc);
    return (this.IsReady(itemKey));
}

ITEM_LIST.prototype.GetItemIDByPos = function (index) {
    // PORT NOTE:  Simplified this function some
    if (this.GetAtPos(index))
        return this.GetAtPos(index).itemID;
    else
        return ""; 
}

ITEM_LIST.prototype.ValidItemListIndex = function (index) {
    return (this.GetAtPos(index) != null && this.GetAtPos(index) != undefined); 
}

ITEM_LIST.prototype.IsCursed = function (index) {
    // PORT NOTE:  Changed a little
    if (this.GetAtPos(index) != null)
        return this.GetAtPos(index).cursed;
    else
        return false;
}


ITEM_LIST.prototype.GetKeyAt = function (index) {
    //PORT NOTE:  Simplified this by just indexing the array by its indices and not separate keys
    return index;
}

// PORT NOTE: Simplified some - eliminated keys
ITEM_LIST.prototype.GetQty = function(index) {
    var item = this.GetAtPos(index);
    if (item != null)
        return item.qty;
    else
        return 0; 
}

ITEM_LIST.prototype.AdjustQty = function (index, qty) {
    var pos;
    if ((pos = this.m_items.FindKeyPos(index)) != null) {
        this.m_items.GetAtPos(pos).qty += qty;
        qty = this.m_items.PeekAtPos(pos).qty;
        if (qty < 0) qty = 0;
        if (qty == 0) {
            if (this.UnReady(index))
                this.DeleteItem(index);
        }
        return qty;
    }
    return -1;
}

ITEM_LIST.prototype.HaveItem = function (itemID) {
    var pos;
    pos = this.GetHeadPosition();
    while (pos != null) {
        if (this.m_items.GetNext(pos).itemID == itemID)
            return true;
        pos = this.m_items.NextPos(pos);
    }
    return false;
}


ITEM_LIST.prototype.DeleteItem = function(index) {
    var pos;
    if ((pos = this.m_items.FindKeyPos(index)) != null) {
        if (this.UnReady(index)) {
            this.m_items.RemoveAt(pos);
            itemsModified = true;
            return true;
        }
    }
    return false;
}  

ITEM_LIST.prototype.GetAtPos = function (pos) {
    return this.m_items.GetAtPos(pos);
}

ITEM_LIST.prototype.PeekAtPos = function (pos) {
    return this.m_items.PeekAtPos(pos);
}

ITEM_LIST.prototype.GetNext = function (pos) {
    return this.m_items.GetNext(pos);
}

ITEM_LIST.prototype.SetItem = function (index, data) {
    return this.m_items.Set(index, data);
}


ITEM_LIST.prototype.halveItem = function(index) {
    var theItem = new ITEM();
    if (this.GetAtPos(index) != null)
        theItem.CopyConstructor(this.GetAtPos(index));   // PORT NOTE:  Changed slightly.  Null check vs. true/false on pbr accessor

    if (!itemData.itemCanBeHalved(theItem.itemID))
        return;

    if (theItem.qty <= 1)
        return;

    var newQty = Math.trunc(theItem.qty / 2);

    // update original qty
    theItem.qty -= newQty;
    this.SetItem(index, theItem);

    // insert new item
    var newItem = new ITEM();
    newItem.CopyConstructor(theItem);
    newItem.qty = newQty;
    newItem.ClearReadyLocation();

    this.AddItem(newItem, false); // don't auto join them back together!
}


ITEM_LIST.prototype.joinItems = function(index) {
    var origItem = new ITEM();
    if (this.GetAtPos(index) != null)
        origItem.CopyConstructor(this.GetAtPos(index));   // PORT NOTE:  Changed slightly.  Null check vs. true/false on pbr accessor

    if (!itemData.itemCanBeJoined(origItem.itemID))
        return;

    // look for another instance of item
    var joinIndex = -1;
    var joinQty = 0;
    var pos;
    pos = this.m_items.GetHeadPosition();

    var delList = new CList();
    while ((pos != null) && (joinIndex < 0)) {
        if ((this.m_items.PeekAtPos(pos).itemID == origItem.itemID)
            && (this.m_items.PeekAtPos(pos).key != origItem.key)) {
            joinQty += this.m_items.PeekAtPos(pos).qty;
            delList.AddTail(this.m_items.PeekAtPos(pos).key);
        }
        pos = this.m_items.NextPos(pos);
    }

    if (joinQty > 0) {
        var joinItem = new ITEM();
        if (this.GetAtPos(index) != null) {
            origItem.CopyConstructor(this.GetAtPos(index));   // PORT NOTE:  Changed slightly.  Null check vs. true/false on pbr accessor
            origItem.qty += joinQty;
            this.SetItem(index, origItem);
            pos = delList.GetHeadPosition();
            while (pos != null) {
                this.DeleteItem(delList.GetAtPos(pos));
                pos = delList.NextPos(pos);
            }
        }
    }
}
