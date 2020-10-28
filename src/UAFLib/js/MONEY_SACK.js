/// <reference path="money_data_type.js" />
/** TODO **/
function MONEY_SACK() {
    this.Coins = [];
    this.Gems = [];   // GEM_TYPE
    this.Jewelry = [];      // JEWELRY_TYPE
}

MONEY_SACK.prototype.Clear = function () {
    /**TODO**/
}
MONEY_SACK.prototype.NumCoinTypes = function () {
    return moneyData.MAX_COIN_TYPES;
}
MONEY_SACK.prototype.SerializeCAR = function(ar, version)
{
    {
        var i;
        for (i = 0; i < this.NumCoinTypes(); i++) {
            if (ar.IsStoring())
                ar.writeInt(this.Coins[i]);
            else
                this.Coins[i] = ar.readInt();
        };
    };

    var pos;
    if (ar.IsStoring()) {
        ar.writeInt(this.Gems.GetCount());
        pos = this.Gems.GetHeadPosition();
        while (pos != null) {
            this.Gems.GetNext(pos).Serialize(ar);
            pos = this.Gems.NextPos(pos);
        }

        ar.writeInt(this.Jewelry.GetCount());
        pos = this.Jewelry.GetHeadPosition();
        while (pos != null) {
            this.Jewelry.GetNext(pos).Serialize(ar);
            pos = this.Jewelry.NextPos(pos);
        }
    }
    else {
        /** TODO: test this block for older gems/jewelry save version support **/
        if (version < _VERSION_0911_) {
            // old style for loading gems and jewelry
            var tempGEMS;
            var i, n;
            n = ar.ReadCount();
            tempGEMS.SetSize(n);
            for (i = 0; i < n; i++) {
                tempGEMS[i].Serialize(ar);
            }

            for (i = 0; i < tempGEMS.GetSize(); i++)
                this.AddGemWithCurrKey(tempGEMS[i]);

            // now load the jewelry
            tempGEMS.RemoveAll();
            n = ar.ReadCount();  // Two-byte count
            tempGEMS.SetSize(n);
            for (i = 0; i < n; i++) {
                tempGEMS[i].Serialize(ar);
            }
            for (i = 0; i < tempGEMS.GetSize(); i++)
                this.AddJewelryWithCurrKey(tempGEMS[i]);
        }
        else {
            var data;
            var temp, i;
            temp = ar.readInt();
            Globals.ASSERT(temp < moneyData.MAX_GEMS, "MONEY_SACK.js::SerializeCAR");

            for (i = 0; i < temp; i++) {
                data.Clear();
                data.Serialize(ar);
                this.AddGemWithCurrKey(data);
            }

            temp = ar.readInt();
            Globals.ASSERT(temp < moneyData.MAX_JEWELRY, "MONEY_SACK.js::SerializeCAR");

            for (i = 0; i < temp; i++) {
                data.Clear();
                data.Serialize(ar);
                this.AddJewelryWithCurrKey(data);
            }
        }
    }
}

MONEY_SACK.prototype.GetTotalWeight = function () {
    var total = 0;

    // total nbr of coins
    for (var i = 0; i < this.NumCoinTypes(); i++)
        total += this.Coins[i];

    total += this.Gems.length;
    total += this.Jewelry.length;

    // now compute weight based on qty per unit weight

    var CoinsPerUnitOfEncumbrance = globalData.moneyData.GetWeight();

    if (CoinsPerUnitOfEncumbrance > 0) {
        total = total / CoinsPerUnitOfEncumbrance;
        total = Math.max(1, total);
    }
    else // this stuff weighs nothing
        total = 0;

    return total;
}

MONEY_SACK.prototype.AddItemClassTypeAmount = function (nIndex, amount) {
    if (!this.IsActive(nIndex)) return;
    var i = moneyData.GetIndex(nIndex);
    this.Coins[i] += amount;
}

MONEY_SACK.prototype.AddMoneySack = function (src) {
    for (var i = 0; i < this.NumCoinTypes(); i++)
        this.Coins[i] += src.Coins[i];

    this.AddGemsGemList(src.Gems);
    this.AddJewelryJewelryList(src.Jewelry);
}

MONEY_SACK.prototype.IsActive = function(nIndex) {
    return (globalData.moneyData.IsActive(nIndex));
}

MONEY_SACK.prototype.AddGemsGemList = function (data) {
    for (var idx = 0; idx < data.length; idx++) {
        if (this.AddGem(data[idx]) == 0)
            return false; // could not add
    }

    return true;
}

MONEY_SACK.prototype.AddGemGemType = function(data) {
    if (this.Gems.GetCount() >= MAX_GEMS)
        return 0;

    var id = this.GetNextGemKey();
    this.Gems[id] = data;
    return id;
}

MONEY_SACK.prototype.GetNextGemKey = function () {
    return this.Gems.length + 1;
}


MONEY_SACK.prototype.AddGemsGemList = function (data) {
    for (var idx = 0; idx < data.length; idx++) {
        if (this.AddGem(data[idx]) == 0)
            return false; // could not add
    }

    return true;
}

MONEY_SACK.prototype.AddJewelryJewelryType = function (data) {
    if (this.Jewelry.GetCount() >= MAX_JEWELRY)
        return 0;

    var id = this.GetNextJewelryKey();
    this.Jewelry[id] = data;
    return id;
}

MONEY_SACK.prototype.GetNextJewelryKey = function () {
    return this.Jewelry.length + 1;
}

MONEY_SACK.prototype.AddJewelryJewelryList = function (data) {
    for (var idx = 0; idx < data.length; idx++) {
        if (this.AddJewelry(data[idx]) == 0)
            return false; // could not add
    }

    return true;
}
