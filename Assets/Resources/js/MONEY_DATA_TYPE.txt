/** TODO **/
function MONEY_DATA_TYPE() {

    this.Coins = [];
    this.Gems = new GEM_CONFIG();
    this.Jewelry = new GEM_CONFIG();
    this.HighestRate = 0;
    this.weight = 0;
    this.HRType = null;
    this.DefType = null;
    this.weight = 0;

    this.MAX_COIN_TYPES = 10;
    this.DEFAULT_COINS_PER_POUND = 10;
    this.MAX_NON_APPRAISED_GEM_JEWELRY = INT_MAX - 1;
    this.MAX_GEMS = this.MAX_NON_APPRAISED_GEM_JEWELRY;
    this.MAX_JEWELRY = this.MAX_NON_APPRAISED_GEM_JEWELRY;

    this.Clear();

}

MONEY_DATA_TYPE.prototype.GetWeight = function () {
    return this.weight;
}

MONEY_DATA_TYPE.prototype.GetIndex = function (type) {
    switch (type) {
        case itemClassType.PlatinumType: return 0;
        case itemClassType.ElectrumType: return 1;
        case itemClassType.GoldType: return 2;
        case itemClassType.SilverType: return 3;
        case itemClassType.CopperType: return 4;
        case itemClassType.CoinType6: return 5;
        case itemClassType.CoinType7: return 6;
        case itemClassType.CoinType8: return 7;
        case itemClassType.CoinType9: return 8;
        case itemClassType.CoinType10: return 9;
    }
    Globals.die(UAFUtil.ByteFromHexString("0xab526"));
    return 0;
}

MONEY_DATA_TYPE.prototype.IsActive = function (index) {
    var i = this.GetIndex(index);
    Globals.ASSERT(i >= 0, "i >= 0");
    return ((this.Coins[i].GetRate() > 0.0) && (strlen(this.Coins[i].GetName()) > 0));
}

MONEY_DATA_TYPE.prototype.Clear = function() {
    for (var i = 0; i < this.MAX_COIN_TYPES; i++)
        this.Coins[i] = new COIN_TYPE();
    this.HighestRate = 0;
    this.weight = 10;
    this.HRType = itemClassType.BogusItemType;
    this.DefType = itemClassType.PlatinumType;
    this.Gems.Clear();
    this.Gems.minValue = 1;
    this.Gems.maxValue = 100;
    this.Gems.SetName("Gem");
    this.Jewelry.Clear();
    this.Jewelry.minValue = 10;
    this.Jewelry.maxValue = 1000;
    this.Jewelry.SetName("Jewelry");
}



/** TODO
void SetUADefaults();
void ComputeHighestRate();
double Convert(double amount, itemClassType srcType, itemClassType dstType, double * overflow=NULL);
double GetConvertRate(itemClassType srcType, itemClassType dstType);

// highest rate
itemClassType GetBaseType();
int GetBaseRate();
double ConvertToBase(double amount, itemClassType srcType, double * overflow=NULL);

// default currency
itemClassType GetDefaultType();
void SetDefaultType(itemClassType type);
int GetDefaultRate();
double ConvertToDefault(double amount, itemClassType srcType, double * overflow=NULL);

#if defined(UAFEngine) || defined(UAFEDITOR)
void Serialize(CArchive & ar, double version);
void Serialize(CAR & ar, double version);
**/
