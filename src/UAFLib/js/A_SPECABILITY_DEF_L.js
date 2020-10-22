/** PORT NOTE: I just couldn't port over another one of these list types - it is too easy to just move it directly into the weakly typed JS array */
function A_SPECABILITY_DEF_L() {
    this.mItems = [];
}

A_SPECABILITY_DEF_L.prototype.Clear = function () {
    this.mItems = [];
}

A_SPECABILITY_DEF_L.prototype.FindAbility = function (key) {
    for (idx = 0; idx < this.mItems.length; idx++) {
        if (this.mItems[idx].m_specAbName == key) {
            return this.mItems[idx];
        }
    }
    return null;
}


A_SPECABILITY_DEF_L.prototype.SerializeCAR = function (car) {
    if (car.IsStoring()) {
        var count;
        var pos;
        var pSpecAb;
        car.writeString("SpecAbVer01");
        car.Compress(true);
        count = GetCount();
        car.writeInt(count);
        pos = GetStartPosition();
        while (pos != null) {
            pSpecAb = this.GetNext(pos);
            pos = this.NextPos(pos);
            pSpecAb.Serialize(car);
            count--;
        };
        if (count != 0) {
            Globals.MsgBoxInfo("Error writing special abilities", "error");
        };
    }
    else {
        var count;
        var pSpecAb;
        this.Clear();
        var version;
        var name;
        this.version = car.readString();
        car.Compress(true);
        count = car.readInt(); // should be 434
        while (count != 0) {
            var i, n;
            this.name = car.readString();
            n = this.name.length;
            for (i = 0; i < n; i++) {
                if (this.name.charCodeAt(i) < 0x20) {
                    this.name = UAFUtil.setCharAt(this.name, i, String.fromCharCode(this.name.charCodeAt(i) + x20));
                };
            }
            pSpecAb = this.InsertAbility(this.name);
            pSpecAb.m_abilityStrings.SerializeCAR(car, "SPECIAL_ABILITIES_DB");
            count--;
        };
    };
}

A_SPECABILITY_DEF_L.prototype.InsertAbility = function (specAbilityName) {
    var pSpecAbilityDef = null;
    var foundData = null;
    pSpecAbilityDef = new SPECABILITY_DEF();
    pSpecAbilityDef.m_specAbName = specAbilityName;

    // PORT NOTE:  Tried to simplify this.   I think this specAbilityDefEnvironment is just a singleton list of special abilities
    specAbilityDefEnvironment[specAbilityName] = pSpecAbilityDef;
    return pSpecAbilityDef;
    //specAbilityDefEnvironment.bTreeData = (int)pSpecAbilityDef;
    //foundData = (SPECABILITY_DEF *)(BTree:: Insert(& specAbilityDefEnvironment));
    //specAbilityDefEnvironment
    //if (foundData != null) {
        //foundData -> Swap(* pSpecAbilityDef); // Update contents
        //delete pSpecAbilityDef;
        //return foundData;
    //};
    //return pSpecAbilityDef;

}




var specAbilityDefEnvironment = {};