function itemReadiedLocationObj(val38) {
    if (!val38) {
        this.location = UAFUtil.StringToBase38('NOTRDY');  //DWORD;
    } else {
        this.location = val38;
    }
}


/**TODO
itemReadiedLocation(DWORD n, const ITEM_ID& itemID, const ITEM_DATA ** ppItemDataCache);
itemReadiedLocation(void){ location = NotReady; };
void CanNot(void){ location = Cannot; };
DWORD Synonym(DWORD rdyLoc) const ;
void CheckLocation(const ITEM_ID& itemID, 
                     const ITEM_DATA ** ppItemDataCache);
//void operator = (DWORD loc);
DWORD Location(void) const { return location;};
void Serialize(CArchive & car);
**/

itemReadiedLocationObj.prototype.Equals = function(rdyLoc)
{

    return this.location == rdyLoc.location;
}

itemReadiedLocationObj.prototype.EqualsDWORD = function(rdyLoc) {
    return this.location == rdyLoc;
}

itemReadiedLocationObj.prototype.SerializeCAR = function(car) {
    if (car.IsStoring()) {
        car.writeDWORD(this.location);
    }
    else {
        var temp;
        temp = car.readDWORD();
        this.location = itemReadiedLocation.Synonym(temp);
    };

}

itemReadiedLocationObj.prototype.Synonym = function(rdyLoc) {
    switch (rdyLoc) {
        case 0: return itemReadiedLocation.WeaponHand;
        case 1: return itemReadiedLocation.ShieldHand;
        case 2: return itemReadiedLocation.BodyArmor;
        case 3: return itemReadiedLocation.AmmoQuiver;
        case 4: return itemReadiedLocation.Head;
        case 5: return itemReadiedLocation.Waist;
        case 6: return itemReadiedLocation.BodyRobe;
        case 7: return itemReadiedLocation.Back;
        case 8: return itemReadiedLocation.Feet;
        case 9: return itemReadiedLocation.Fingers;
        case 10: return itemReadiedLocation.AmmoQuiver;
        case 11: return itemReadiedLocation.Cannot;
        case 12: return itemReadiedLocation.Arms;
        case 13: return itemReadiedLocation.Legs;
        case 14: return itemReadiedLocation.Face;
        case 15: return itemReadiedLocation.Neck;
        case 16: return itemReadiedLocation.Pack;
        default: return itemReadiedLocation.rdyLoc;
    };
}

itemReadiedLocationObj.prototype.Clear = function () {
    this.location = itemReadiedLocation.NotReady.location;
}


var itemReadiedLocation = {};
itemReadiedLocation.WeaponHand = new itemReadiedLocationObj(UAFUtil.StringToBase38('WEAPON'));
itemReadiedLocation.ShieldHand = new itemReadiedLocationObj(UAFUtil.StringToBase38('SHIELD'));
itemReadiedLocation.BodyArmor = new itemReadiedLocationObj(UAFUtil.StringToBase38('ARMOR'));
itemReadiedLocation.AmmoQuiver = new itemReadiedLocationObj(UAFUtil.StringToBase38('QUIVER'));
itemReadiedLocation.Hands = new itemReadiedLocationObj(UAFUtil.StringToBase38('HANDS'));
itemReadiedLocation.Head = new itemReadiedLocationObj(UAFUtil.StringToBase38('HEAD'));
itemReadiedLocation.Waist = new itemReadiedLocationObj(UAFUtil.StringToBase38('WAIST'));
itemReadiedLocation.BodyRobe = new itemReadiedLocationObj(UAFUtil.StringToBase38('ROBE'));
itemReadiedLocation.Back = new itemReadiedLocationObj(UAFUtil.StringToBase38('CLOAK'));
itemReadiedLocation.Feet = new itemReadiedLocationObj(UAFUtil.StringToBase38('FEET'));
itemReadiedLocation.Fingers = new itemReadiedLocationObj(UAFUtil.StringToBase38('FINGER'));
itemReadiedLocation.Cannot = new itemReadiedLocationObj(UAFUtil.StringToBase38('CANNOT'));
itemReadiedLocation.NotReady = new itemReadiedLocationObj(UAFUtil.StringToBase38('NOTRDY'));
itemReadiedLocation.Arms = new itemReadiedLocationObj(UAFUtil.StringToBase38('ARMS'));
itemReadiedLocation.Legs = new itemReadiedLocationObj(UAFUtil.StringToBase38('LEGS'));
itemReadiedLocation.Face = new itemReadiedLocationObj(UAFUtil.StringToBase38('FACE'));
itemReadiedLocation.Neck = new itemReadiedLocationObj(UAFUtil.StringToBase38('NECK'));
itemReadiedLocation.Pack = new itemReadiedLocationObj(UAFUtil.StringToBase38('PACK'));
itemReadiedLocation.Undefined = new itemReadiedLocationObj(UAFUtil.StringToBase38('UNDEF'));
