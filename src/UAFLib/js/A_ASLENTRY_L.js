/** TODO **/
function A_ASLENTRY_L() {
    this.m_specialAbilities = new A_CStringPAIR_L();
    this.m_readOnly = true;
    this.mItems = [];
}

A_ASLENTRY_L.prototype.Clear = function () {
    this.m_specialAbilities.Clear();
}



A_ASLENTRY_L.prototype.SerializeCAR = function (ar, mapName) {

    if (Globals.GetGameVersion(globalData) >= _ASL_LEVEL_) {
        if (ar.IsStoring()) {
            var count;
            var p;
            pEntry = new ASLENTRY();
            ar.writeString(mapName);
            count = this.GetCount();
            this.writeString(count);
            p = this.GetStartPosition();
            while (p != null) {
                pEntry = GetNextAssoc(p);
                pEntry.SerializeCAR(ar);
            };
        }
        else {
            var temp;
            var count;
            temp = ar.readString();
            if (temp != mapName) {
                var msg;
                msg = "Expected:\n" + mapName + "\nEncountered:\n" + temp + "\nwhile reading file.";
                Globals.WriteDebugString(msg);

                Globals.MsgBoxError(msg);
                msg = ar.GetFile().GetFileName();
                throw 7;
                //   I replaced the AfsThrowArchiveExecption because I could not
                //   figure out how catch it properly.  It caused a memory leak.
                //AfxThrowArchiveException(CArchiveException::badIndex,
                //          ar.GetFile()->GetFileName());
            };
            // 16-bit count; {CString key; char flags; CString value}
            count = ar.readWORD();
            for (; count > 0; count--) {
                var tempASL = new ASLENTRY("", "", 0);
                tempASL.DeSerialize(ar);
                this.InsertASLENTRY(tempASL);
            };
        };

    };
    return ar;
}



A_ASLENTRY_L.prototype.InsertASLENTRY = function(pEntry) {
    // PORT NOTE:  Rewrite to make this simpler - seems like a collection that replaces if the entry is already present
    for (var i = 0; i < this.mItems.length; i++) {
        var aslEntry = this.mItems[i];
        if (pEntry.Equals(aslEntry)) {
            this.mItems[i] = pEntry;
            return true;
        }
    }
    this.mItems.push(pEntry);
    return false;
}


/**TODO
BOOL A_ASLENTRY_L:: Insert(const CString& key, const CString& data, unsigned char flags)
**/