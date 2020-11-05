function ITEM_DATA() {
    /**TODO
    {
        Clear(TRUE);
    }
    */

    //*********************************************************************************
    // In the editor, uniqueName means the fully qualified name sith                 **
    // optional modifier.  Example: BatttleAxe|Dull // PRS 20110107  Battle Axe(Dull)**.
    // When we write the binary items.dat file, the modifiers                        **
    // are taken off.                                                                **
    //                                                                               **
    // PRS 20110107                                                                  **
    // Sorry about that.  We shall be using the unique names                         **
    // in scripts.  Scripts need to uniquely fetch and compare names.                **
    // When we load the items database we will construct a 'common name' by          **
    // removing the vertal bar and qualifier from the unique name                    **
    // What we are going to do here is to make the name a private member,            **
    // provide a function to get the unique name and a function                      ** 
    // to get the decorated name.                                                    ** 
    this.m_uniqueName = "";                                                 //        **
    this.m_commonName = "";                                                 //        **
    this.m_idName = "";                                                     //        **
    this.preSpellNameKey; // See comment at def    this.ion of VersionSaveIDs          **
    this.FNV_Hash = 0; //DWORD // Not Serialized....Computed when initialized.               **
    this.AmmoType = "";
    this.HitSound = "";
    this.MissSound = "";
    this.LaunchSound = "";
    this.hHitSound = 0;
    this.hMissSound = 0;
    this.hLaunchSound = 0;
    this.Experience = 0;
    this.Cost = 0;
    this.Encumbrance = 0;
    this.Attack_Bonus = 0 ;
    this.Cursed = false;
    this.Bundle_Qty = 0;
    this.Num_Charges = 0;
    this.Location_Readied = 0;  //DWORD // A base-38 name!
    this.Hands_to_Use = 0;
    this.Dmg_Dice_Sm = 0;
    this.Nbr_Dice_Sm = 0;
    this.Dmg_Bonus_Sm = 0;
    this.Dmg_Dice_Lg = 0;
    this.Nbr_Dice_Lg = 0;
    this.Dmg_Bonus_Lg = 0;
    this.ROF_Per_Round = 0.0;
    this.Protection_Base = 0;
    this.Protection_Bonus = 0;
    this.Wpn_Type = weaponClassType.NotWeapon;
    this.m_priorityAI = 0; // Not serialized.  Initialized from Special Ability.
    this.m_usageFlags = 0;
    this.USAGE_usable = UAFUtil.ByteFromHexString("0x00000001"),
    this.USAGE_scribable = UAFUtil.ByteFromHexString("0x00000002");
    this.USAGE_notMagical = UAFUtil.ByteFromHexString("0x00000004");
    this.CanBeHalvedJoined = false;
    this.CanBeTradeDropSoldDep = false;
    this.usableByBaseclass = [];
    this.RangeMax = 0;
    this.RangeShort = 0; // Computed...not serialized
    this.RangeMedium = 0; // Computed...not serialized
    this.spellID = 0;
    this.m_useEvent = 0;  //DWORD
    this.ExamineEvent = 0;  //DWORD
    this.ExamineLabel = "";
    this.attackMsg = "";
    this.Recharge_Rate = 0;
    this.IsNonLethal = false;
    this.specAbs = new SPECIAL_ABILITIES(true);
    this.MissileArt = new PIC_DATA (); // in-route sprite
    this.HitArt = new PIC_DATA();     // target hit sprite
    this.item_asl = new A_ASLENTRY_L();
    this.temp_asl = new A_ASLENTRY_L();
}

ITEM_DATA.prototype.SpellID = function () {
    return this.spellID;
}


ITEM_DATA.prototype.SerializeCAR = function (ar, ver) {
    var temp;  // Long

    if (ar.IsStoring()) {
        if (ver <= _VERSION_0830_)
            this.m_idName.Replace('/', '|');
        ar.writeInt(this.preSpellNameKey);
        ar.writeInt(this.spellID);
        AS(ar, this.m_uniqueName);
        AS(ar, this.m_idName);

        this.HitSound = Globals.StripFilenamePath(this.HitSound);
        this.MissSound = Globals.StripFilenamePath(this.MissSound);
        this.LaunchSound = Globals.StripFilenamePath(this.LaunchSound);
        AS(ar, HitSound);
        AS(ar, MissSound);
        AS(ar, LaunchSound);

        this.HitArt.SerializeCAR(ar, ver, "");
        this.MissileArt.SerializeCAR(ar, ver, "");

        AS(ar, this.AmmoType);
        ar.writeLong(this.Experience);
        ar.writeLong(this.Cost);
        ar.writeLong(this.Encumbrance);
        ar.writeLong(this.Attack_Bonus);
        ar.writeBool(this.Cursed);
        ar.writeLong(this.Bundle_Qty);
        ar.writeLong(this.Num_Charges);
        ar.writeDWORD(Location_Readied);
        ar.writeLong(this.Hands_to_Use);
        ar.writeLong(this.Dmg_Dice_Sm);
        ar.writeLong(this.Nbr_Dice_Sm);
        ar.writeLong(this.Dmg_Bonus_Sm);
        ar.writeLong(this.Dmg_Dice_Lg);
        ar.writeLong(this.Nbr_Dice_Lg);
        ar.writeLong(this.Dmg_Bonus_Lg);
        ar.writeDouble(this.ROF_Per_Round);
        ar.writeLong(this.Protection_Base);
        ar.writeLong(this.Protection_Bonus);

        temp = this.Wpn_Type;
        ar.writeLong(temp);
        ar.writeInt(this.m_usageFlags);
        {
            var i, n;
            n = this.usableByBaseclass.GetSize();
            ar.writeInt(n);
            for (i = 0; i < n; i++) {
                ar.writeInt(this.GetBaseclass(i));
            }
        }
        ar.writeLong(this.RangeMax);
        ar.writeDWORD(this.m_useEvent);
        ar.writeDWORD(this.ExamineEvent);
        AS(ar, ExamineLabel);
        AS(ar, attackMsg);
        ar.writeInt(this.Recharge_Rate);
        ar.writeBool(this.IsNonLethal);
        this.HitArt.SerializeCAR(ar, ver, RUNTIME_ENVIRONMENT.CombatArtDir());
        ar.writeBool(this.CanBeHalvedJoined);
        ar.writeBool(this.CanBeTradeDropSoldDep);
        // MissileArt is serialized in attribute map
    }
    else {
        var uTemp;
        if ((ver < VersionSpellNames) || (ver >= VersionSaveIDs)) {
            this.preSpellNameKey = ar.readInt();  // For conversion only.
        }
        else {
            this.preSpellNameKey = -1;
        };
        if (ver >= 0.999647) {
            this.spellID = readInt();
        };
        this.m_uniqueName = DAS(ar);
        // In the runtime, Name means only the un-decorated
        // part of the name.  For example:  "Battle Axe|Dull" will be
        // read as simply "Battle Axe".  There can be multiple objects with
        // the same name but once we are at the point where we have the binary 
        // data then everything is identified uniquely and is referenced by the 
        // binary key.  The name is only for printing.
        //if (m_uniqueName.Find('|') >= 0) m_uniqueName = m_uniqueName.Left(m_uniqueName.Find('|')); // Remove qualifier.
        this.m_commonName = this.m_uniqueName;
        if (this.m_commonName.indexOf('|') >= 0) this.m_commonName = this.m_commonName.substr(this.m_commonName.indexOf('|')); // Remove qualifier.
        this.m_idName = DAS(ar);

        this.HitSound = DAS(ar);
        this.MissSound = DAS(ar);
        if (ver >= _VERSION_05691_)
            this.LaunchSound = DAS(ar);
        {
            this.HitArt.SerializeCAR(ar, ver, "");
            this.MissileArt.SerializeCAR(ar, ver, "");
        };

        if (globalData.version < 0.930279) {
            this.HitSound = Globals.StripFilenamePath(this.HitSound);
            this.MissSound = Globals.StripFilenamePath(this.MissSound);
            this.LaunchSound = Globals.StripFilenamePath(this.LaunchSound);
        };


        if (ver >= _VERSION_0690_)
            this.AmmoType = DAS(ar);
        if (this.AmmoType.toLowerCase() == "none") {
            this.AmmoType = "";
        };
        this.Experience = ar.readLong();
        this.Cost = ar.readLong();
        this.Encumbrance = ar.readLong();
        this.Attack_Bonus = ar.readLong();
        this.Cursed = ar.readBool();
        this.Bundle_Qty = ar.readLong();
        this.Num_Charges = ar.readLong();

        uTemp = ar.readDWORD();
        switch (uTemp)  // Switch old numbers to base-38 names.
        {
            case 0: uTemp = itemReadiedLocation.WeaponHand; break;  // Sword								
            case 1: uTemp = itemReadiedLocation.ShieldHand; break;  // Shield
            case 2: uTemp = itemReadiedLocation.BodyArmor; break;  // Armor
            case 3: uTemp = itemReadiedLocation.Hands; break;  // Guantlets
            case 4: uTemp = itemReadiedLocation.Head; break;  // Helm
            case 5: uTemp = itemReadiedLocation.Waist; break;  // Belt
            case 6: uTemp = itemReadiedLocation.BodyRobe; break;  // Robe
            case 7: uTemp = itemReadiedLocation.Back; break;  // Cloak
            case 8: uTemp = itemReadiedLocation.Feet; break;  // Boots
            case 9: uTemp = itemReadiedLocation.Fingers; break;  // Rings
            case 10: uTemp = itemReadiedLocation.AmmoQuiver; break;  // Arrows, Bolts
        };
        this.Location_Readied = uTemp;

        this.Hands_to_Use = ar.readLong();
        this.Dmg_Dice_Sm = ar.readLong();
        this.Nbr_Dice_Sm = ar.readLong();
        this.Dmg_Bonus_Sm = ar.readLong();
        this.Dmg_Dice_Lg = ar.readLong();
        this.Nbr_Dice_Lg = ar.readLong();
        this.Dmg_Bonus_Lg = ar.readLong();
        this.ROF_Per_Round = ar.readDouble();
        this.Protection_Base = ar.readLong();
        this.Protection_Bonus = ar.readLong();

        temp = ar.readLong();
        this.Wpn_Type = temp;

        this.m_usageFlags = ar.readInt();
        {
            var i, n;
            var baseclassID = new BASECLASS_ID();
            n = ar.readInt();
            this.usableByBaseclass.RemoveAll();
            for (i = 0; i < n; i++) {
                baseclassID = ar.readInt();
                this.usableByBaseclass.Add(baseclassID);
            }
        }
        this.RangeMax = ar.readLong();
        this.RangeShort = (this.RangeMax + 2) / 3; // These may be overridden by $AI_ShortRange
        this.RangeMedium = 2 * this.RangeMax / 3;  // and $AI_MediumRange Special Abilities. 

        if (ver >= _VERSION_0662_)
            this.m_useEvent = ar.readDWORD();

        if (ver >= _VERSION_0800_) {
            this.ExamineEvent = ar.readDWORD();
            this.ExamineLabel = DAS(ar);
        }

        if (ver >= _VERSION_0860_) {
            this.attackMsg = DAS(ar);
        }
        else
            this.attackMsg = "attacks";

        if (ver >= _VERSION_0690_) {
            temp = ar.readLong();
            this.Recharge_Rate = temp;
            this.IsNonLethal = ar.readBool();
            this.HitArt.SerializeCAR(ar, ver, RUNTIME_ENVIRONMENT.CombatArtDir());
        }
        if (ver >= _VERSION_0881_)
            this.CanBeHalvedJoined = ar.readBool();
        else
            this.CanBeHalvedJoined = true;

        if (ver >= _VERSION_0904_)
            this.CanBeTradeDropSoldDep = ar.readBool();
        else
            this.CanBeTradeDropSoldDep = true;

        // MissileArt is serialized in attribute map
    }

    this.specAbs.SerializeCAR(ar, ver, this.m_idName, "item");
    if (ar.IsStoring())
        this.item_asl.SerializeCAR(ar, "ITEM_DATA_ATTRIBUTES");
    else
        this.item_asl.SerializeCAR(ar, "ITEM_DATA_ATTRIBUTES");
}


/**TODO
    CString IdName(void)const { return m_idName; }
    ITEM_DATA(ITEM_DATA & src) :
    void Serialize(CArchive & ar, double ver);
    void Save(CAR & car) const ;
    void PrepareForRestore(void);
    void Restore(CArchive & ar);
    void Restore(CAR & car);
    void CommitRestore(void);

    ActorType GetContext() const ;
    void SetASL(CString key, CString value, BYTE flags);
    BOOL GetASL(CString key, CString & value) const ;
    void PreSerialize(BOOL IsStoring);
    void PostSerialize(BOOL IsStoring, double version);
    void Clear(BOOL ctor = FALSE);
    bool operator == (const ITEM_DATA& src) const ;
    int  GetDataSize(void) const ;
    void PlayHit();
    void PlayMiss();
    void PlayLaunch();
    void ClearSounds();
    void LoadArt();
    void ClearArt(); // release surface and clear pic data
    void FreeArt(); // release surface
    void CastSpell(int spell) const ;
    void SpellActivate(const PENDING_SPELL & data) const ;
    friend BOOL ImportUAItemsToUAF(const char * path);
    friend static void DitemScribable(CONFIG_PKT * pkt);
    friend static void DitemDroppable(CONFIG_PKT * pkt);
void SetIdName(const CString& name);                               //        **
CString  CommonName(void)const { return m_commonName;}; //Not serialized.//        **
inline   CString  UniqueName(void)const { return m_uniqueName;};         //        **
void SetUniqueName(const CString& name);                           //        **
ITEM_ID  ItemID(void)  const { ITEM_ID x; x = m_uniqueName; return x;};    //        **
void SpellID(const SPELL_ID& s) { spellID = s; };                     //        **
BOOL IsUsableByClass(const CHARACTER * pChar) const ;
BOOL IsUsableByBaseclass(const BASECLASS_ID& baseclassID) const ;
    inline BOOL IsUsable(void) const { return (m_usageFlags & USAGE_usable) != 0;};
inline void IsUsable(BOOL usable){ if (usable) m_usageFlags |= USAGE_usable; else m_usageFlags &= ~USAGE_usable; };
inline BOOL IsScribable(void) const { return (m_usageFlags & USAGE_scribable) != 0;};
inline BOOL IsNotMagical(void) const { return (m_usageFlags & USAGE_notMagical) != 0;};
inline void IsNotMagical(BOOL t){ if (t) m_usageFlags |= USAGE_notMagical; else m_usageFlags &= ~USAGE_notMagical; };
inline void IsScribable(BOOL scribable){ if (scribable) m_usageFlags |= USAGE_scribable; else m_usageFlags &= ~USAGE_scribable; };

 CString RunItemScripts(LPCSTR     scriptName,
    CBRESULT(* fnc)(CBFUNC func, CString * scriptResult, void * pkt),
    void      * pkt,
    LPCSTR     comment) const
        {
            return specAbs.RunScripts(
                scriptName,
                fnc,
                pkt,
                comment,
                ScriptSourceType_Item,
                m_uniqueName);
  };

  **/
