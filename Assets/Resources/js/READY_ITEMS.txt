/** TODO **/
function READY_ITEMS() {
    this.m_readyWeaponHand = NO_READY_ITEM;
    this.m_readyShieldHand = NO_READY_ITEM;
    this.m_readyMissileAmmo = NO_READY_ITEM;
    this.m_readyArmor = NO_READY_ITEM;
    this.m_readyGauntlets = NO_READY_ITEM;
    this.m_readyHelmet = NO_READY_ITEM;
    this.m_readyBelt = NO_READY_ITEM;
    this.m_readyRobe = NO_READY_ITEM;
    this.m_readyCloak = NO_READY_ITEM;
    this.m_readyBoots = NO_READY_ITEM;
    this.m_readyRing1 = NO_READY_ITEM;
    this.m_readyRing2 = NO_READY_ITEM;
    this.Clear();
};

READY_ITEMS.prototype.Clear = function () {
    this.m_readyWeaponHand = NO_READY_ITEM;
    this.m_readyShieldHand = NO_READY_ITEM;
    this.m_readyMissileAmmo = NO_READY_ITEM;
    this.m_readyArmor = NO_READY_ITEM;
    this.m_readyGauntlets = NO_READY_ITEM;
    this.m_readyHelmet = NO_READY_ITEM;
    this.m_readyBelt = NO_READY_ITEM;
    this.m_readyRobe = NO_READY_ITEM;
    this.m_readyCloak = NO_READY_ITEM;
    this.m_readyBoots = NO_READY_ITEM;
    this.m_readyRing1 = NO_READY_ITEM;
    this.m_readyRing2 = NO_READY_ITEM;
}

READY_ITEMS.prototype.SerializeCAR = function (ar) {
    if (ar.IsStoring()) {
        ar.writeInt(this.m_readyWeaponHand);
        ar.writeInt(this.m_readyShieldHand);
        ar.writeInt(this.m_readyMissileAmmo);
        ar.writeInt(this.m_readyArmor);
        ar.writeInt(this.m_readyGauntlets);
        ar.writeInt(this.m_readyHelmet);
        ar.writeInt(this.m_readyBelt);
        ar.writeInt(this.m_readyRobe);
        ar.writeInt(this.m_readyCloak);
        ar.writeInt(this.m_readyBoots);
        ar.writeInt(this.m_readyRing1);
        ar.writeInt(this.m_readyRing2);
    }
    else {
        this.m_readyWeaponHand = ar.readInt();
        this.m_readyShieldHand = ar.readInt();
        this.m_readyMissileAmmo = ar.readInt();
        this.m_readyArmor = ar.readInt();
        this.m_readyGauntlets = ar.readInt();
        this.m_readyHelmet = ar.readInt();
        this.m_readyBelt = ar.readInt();
        this.m_readyRobe = ar.readInt();
        this.m_readyCloak = ar.readInt();
        this.m_readyBoots = ar.readInt();
        this.m_readyRing1 = ar.readInt();
        this.m_readyRing2 = ar.readInt();
    }
}

/** TODO
READY_ITEMS.prototype.Serialize(ar) { }
READY_ITEMS.prototype.Export = function(jw) { }
**/
