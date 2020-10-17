function itemReadiedLocation() {
    this.location = 0;  //DWORD;
};
itemReadiedLocation.WeaponHand = UAFUtil.StringToBase38('WEAPON');
itemReadiedLocation.ShieldHand = UAFUtil.StringToBase38('SHIELD');
itemReadiedLocation.BodyArmor = UAFUtil.StringToBase38('ARMOR');
itemReadiedLocation.AmmoQuiver = UAFUtil.StringToBase38('QUIVER');
itemReadiedLocation.Hands = UAFUtil.StringToBase38('HANDS');
itemReadiedLocation.Head = UAFUtil.StringToBase38('HEAD');
itemReadiedLocation.Waist = UAFUtil.StringToBase38('WAIST');
itemReadiedLocation.BodyRobe = UAFUtil.StringToBase38('ROBE');
itemReadiedLocation.Back = UAFUtil.StringToBase38('CLOAK');
itemReadiedLocation.Feet = UAFUtil.StringToBase38('FEET');
itemReadiedLocation.Fingers = UAFUtil.StringToBase38('FINGER');
itemReadiedLocation.Cannot = UAFUtil.StringToBase38('CANNOT');
itemReadiedLocation.NotReady = UAFUtil.StringToBase38('NOTRDY');
itemReadiedLocation.Arms = UAFUtil.StringToBase38('ARMS');
itemReadiedLocation.Legs = UAFUtil.StringToBase38('LEGS');
itemReadiedLocation.Face = UAFUtil.StringToBase38('FACE');
itemReadiedLocation.Neck = UAFUtil.StringToBase38('NECK');
itemReadiedLocation.Pack = UAFUtil.StringToBase38('PACK');
itemReadiedLocation.Undefined = UAFUtil.StringToBase38('UNDEF');

/**TODO
itemReadiedLocation(DWORD n, const ITEM_ID& itemID, const ITEM_DATA ** ppItemDataCache);
itemReadiedLocation(void){ location = NotReady; };
void Clear(void){ location = NotReady; };
void CanNot(void){ location = Cannot; };
DWORD Synonym(DWORD rdyLoc) const ;
void CheckLocation(const ITEM_ID& itemID, 
                     const ITEM_DATA ** ppItemDataCache);
//void operator = (DWORD loc);
DWORD Location(void) const { return location;};
void Serialize(CArchive & car);
**/

itemReadiedLocation.prototype.Equals = function(rdyLoc)
{
    return location == rdyLoc.location;
}

itemReadiedLocation.prototype.EqualsDWORD = function(rdyLoc) {
    return location == rdyLoc;
}

itemReadiedLocation.prototype.SerializeCAR = function(car) {
    if (car.IsStoring()) {
        car.writeDWORD(this.location);
    }
    else {
        var temp;
        temp = car.readDWORD();
        this.location = itemReadiedLocation.Synonym(temp);
    };

}

itemReadiedLocation.Synonym = function(rdyLoc) {
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
