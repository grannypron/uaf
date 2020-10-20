///*** LEFT OFF: THAC0 is returning NaN


var DEFAULT_SPELL_EFFECT_FLAGS = SPELL_EFFECTS_DATA.ALL_TARG_TYPES;
var MAX_CHARACTERS = INT_MAX; // Max pre-gen NPC's 
var MAX_CHAR_NAME = 30; // should match MAX_MONSTER_NAME!
var MAX_AC = 10;
var MIN_AC = -500;
var MIN_THAC0 = -500;
var MAX_THAC0 = 20;

function CHARACTER(character) {
    /** TODO 
    CHARACTER (CHARACTER &src) // TODO 
    CHARACTER & 	operator= (CHARACTER &src) // TODO 
    CHARACTER.prototype.operator== (CHARACTER &src) // TODO
    */
    this.itemData = ITEM_DATA_TYPE;
    this.preSpellNamesKey = 0;
    this.name = "";
    this.undeadType = "";
    this.spellAbility = new SPELL_ABILITY();
    this.money = new MONEY_SACK();
    this.icon = new PIC_DATA();
    this.smallPic = new PIC_DATA();
    this.monsterID = new MONSTER_ID();
    this.characterID = new CHARACTER_ID();
    this.specAbs = new SPECIAL_ABILITIES();
    this.blockageData = new BLOCKAGE_STATUS();
    this.m_spellbook = new spellBookType();
    this.myItems = new ITEM_LIST();
    this.char_asl = new A_ASLENTRY_L();

    this.Clear(true);
};

CHARACTER.prototype.IsSameCharacter = function(dude) {
  if (dude == this) return true;
  if (typeof(this) != typeof(dude)) return false;
  if (this.IsPreGen != dude.IsPreGen) return false;
  if (this.name != dude.name) return false;  
  return true;
};

/**TODO** virtual COMBATANT * 	GetCombatant () */

CHARACTER.prototype.Clear = function(isConstructor) { 
    if (!isConstructor) {isConstructor = false;}

    this.DisableTalkIfDead=true;
    this.TalkEvent=0;
    this.TalkLabel="TALK";
    this.ExamineEvent=0;
    this.ExamineLabel="EXAMINE";
    this.type = CHAR_TYPE;
    this.race = "";
    this.gender = genderType.Male;
    this.classID = "";
    this.alignment = alignmentType.ChaoticNeutral;
    this.allowInCombat = true;
    this.status = charStatusType.Unconscious;
    this.undeadType = "";
    this.creatureSize = creatureSizeType.Medium;
    this.name = "";
    this.characterID = this.name;
    this.THAC0 = 20;
    this.morale = 50; // 0.100%
    this.encumbrance = 0;
    this.maxEncumbrance = 0;
    this.m_AC = 10;
    this.hitPoints = 0;
    this.maxHitPoints = 0;
    this.nbrHitDice = 0;
    this.age = 0;
    this.maxAge = 0;
    this.SetBirthday(1);
    this.m_iMaxCureDisease = 0;
    this.m_iUsedCureDisease = 0;
    this.unarmedDieS = 0;
    this.unarmedNbrDieS = 0;
    this.unarmedBonus = 0;
    this.unarmedDieL = 0;
    this.unarmedNbrDieL = 0;
    this.maxMovement = 0;
    this.readyToTrain = false;
    this.canTradeItems = true;
    this.strength = 0;
    this.strengthMod = 0;
    this.intelligence = 0;
    this.wisdom = 0;
    this.dexterity = 0;
    this.constitution = 0;
    this.charisma = 0;
    this.strength_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
    this.strengthMod_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
    this.intelligence_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
    this.wisdom_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
    this.dexterity_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
    this.constitution_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
    this.charisma_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
    this.spellAbility.valid = false;
    this.openDoors = 0;
    this.openMagicDoors = 0;
    this.BB_LG = 0;
    this.automatic = false;
    this.allowPlayerControl = true;
    this.detectingInvisible = false;
    this.detectingTraps = false;
    this.hitBonus = 0;
    this.dmgBonus = 0;
    this.magicResistance = 0;
    this.baseclassStats = [];

    this.IsPreGen = false;
    this.CanBeSaved = true;
    this.HasLayedOnHandsToday = false;
    this.money.Clear();
    this.NbrAttacks = 0.0;
    this.icon.Clear();
    this.iconIndex = 1;
    this.smallPic.Clear();
    this.origIndex = -1;
    this.monsterID.Clear();
    this.characterID = "";
    this.uniquePartyID = UAFUtil.ByteFromHexString("0xff");
    this.specAbs.Clear();
    this.specAbQueue = {};
    this.blockageData.Clear();
    this.m_spellbook.Clear();
    this.myItems.Clear();
    this.char_asl.Clear();
    this.m_spellCastingLevel = -1;
    this.AIBaseclass = -1;
    this.m_spellEffects = new CList();
    this.spellAdjustments = {};
    this.skillAdjustments = {};
    this.m_pCombatant = null;

};

CHARACTER.prototype.Serialize = function (ar, version) {
    var characterVersion = 0;
    var raceProblem = false;
    var raceProblemTemp = 999;
    var classProblem = false;
    var classProblemTemp = 999;
    if (ar.IsStoring()) {

        /** PORT NOTE: Not sure why temp was used here, but I kept it for documentation/just in case **/ 
        var temp;
        ar.writeInt(Globals.CHARACTER_VERSION);
        ar.writeByte(this.type);
        ar.writeString(this.race);
        temp = this.gender;
        ar.writeInt(temp);
        ar.writeString(this.classID);
        temp = this.alignment;
        ar.writeInt(temp);

        ar.writeBool(this.allowInCombat);

        temp = this.status;
        ar.writeInt(temp);
        ar.writeString(this.undeadType);
        temp = this.creatureSize;
        ar.writeInt(temp);

        AS(ar, this.name);
        ar.writeInt(this.THAC0);
        ar.writeInt(this.morale);
        ar.writeInt(this.encumbrance);
        ar.writeInt(this.maxEncumbrance);
        ar.writeInt(this.m_AC);
        ar.writeInt(this.hitPoints);
        ar.writeInt(this.maxHitPoints);
        ar.writeDouble(this.nbrHitDice);
        ar.writeInt(this.age);
        ar.writeInt(this.maxAge);
        ar.writeInt(this.birthday);
        ar.writeInt(this.m_iMaxCureDisease);
        ar.writeInt(this.unarmedDieS);
        ar.writeInt(this.unarmedNbrDieS);
        ar.writeInt(this.unarmedBonus);
        ar.writeInt(this.unarmedDieL);
        ar.writeInt(this.unarmedNbrDieL);
        ar.writeByte(this.maxMovement);
        ar.writeBool(this.readyToTrain);
        ar.writeBool(this.canTradeItems);

        Globals.die("Not Needed?"); //Not Implemented(0x442bc, false); // Should not be using CArchive.

        ar.writeByte(this.openDoors);
        ar.writeByte(this.openMagicDoors);
        ar.writeByte(this.BB_LG);
        ar.writeInt(this.hitBonus);
        ar.writeInt(this.dmgBonus);
        ar.writeInt(this.magicResistance);
        {
            var i, count;
            count = this.GetBaseclassStatsCount();
            ar.writeInt(count);
            for (i = 0; i < count; i++) {
                this.GetBaseclassStats(i).Serialize(ar);
                //baseclassStats[i].Serialize(ar);
            };
        };

        ar.writeBool(this.IsPreGen);
        ar.writeBool(this.CanBeSaved);
        ar.writeBool(this.HasLayedOnHandsToday);
        this.money.Serialize(ar, version);
        ar.writeDouble(this.NbrAttacks);
        this.icon.Serialize(ar, version, rte.IconArtDir());
        ar.writeInt(this.iconIndex);
        ar.writeInt(this.origIndex);
        ar.writeByte(this.uniquePartyID);
        ar.writeBool(this.DisableTalkIfDead);
        ar.writeDword(this.TalkEvent);
        this.AS(ar, this.TalkLabel);
        ar.writeDword(this.ExamineEvent);
        this.AS(ar, this.ExamineLabel);

        //ar.write(spells;
        this.m_spellbook.Serialize(ar, version);

        //ar.write(detectingMagic;
        ar.writeBool(this.detectingInvisible);
        ar.writeBool(this.detectingTraps);
        ar.writeInt(this.m_spellEffects.GetCount());
        var pos = this.m_spellEffects.GetHeadPosition();
        while (pos != null) {
            this.m_spellEffects.GetNext(pos).Serialize(ar, version);
        };
        blockageData.Serialize(ar);
    }
    else {

        var temp;
        temp = ar.readInt();  // Possible character version
        if ((temp & 0x80000000) != 0) {
            // This is a character version, not an index.
            this.characterVersion = temp;
        }
        else {
            this.characterVersion = 0;   // This is a key....not a version
            this.preSpellNamesKey = temp;
            //uniqueKey = temp;
        };
        this.type = ar.readByte();
        {
            this.race = RACE_ID.get(ar.readInt());
        };
        this.gender = genderType.get(ar.readInt());
        {
            this.classID = CLASS_ID.get(ar.readInt(new CLASS_ID()));
        };
        this.alignment = ar.readObject(new alignmentType());

        if (version >= _VERSION_0912_)
            this.allowInCombat = ar.readBool();
        temp = charStatusType.get(ar.readInt());
        {
            this.undeadType = ar.readCString();
        };
        this.creatureSize = creatureSizeType.get(ar.readInt());
        this.DAS(ar, name);
        if (version < Globals.VersionSpellNames) {
            this.characterID = name;
        };
        if (raceProblem) {
            var msg = "Character " + this.name + " has race number " + raceProblemTemp + ".  " + 
                 "We cannot determine which race this should be so we " +
                 "set the race to " + this.race;
            alert(msg);// **TODO**
        };

        if (classProblem) {
            var msg = "Character " + this.name + " has class number " + classProblemTemp + ".  " +
                 "We cannot determine which class this should be so we " +
                 "set the class to " + this.classID;
            alert(msg);// **TODO**
        };

        this.THAC0 = ar.readInt();
        this.morale = ar.readInt();
        this.encumbrance = ar.readInt();
        this.maxEncumbrance = ar.readInt();
        this.m_AC = ar.readInt();
        this.hitPoints = ar.readInt();
        this.maxHitPoints = ar.readInt();
        this.nbrHitDice = ar.readDouble();
        this.age = ar.readInt();
        this.maxAge = ar.readInt();
        if (version >= _VERSION_0830_) {
            this.birthday = ar.readInt();
            this.SetBirthday(birthday);
        }

        this.m_iMaxCureDisease = ar.readInt();
        this.unarmedDieS = ar.readInt();
        this.unarmedNbrDieS = ar.readInt();
        this.unarmedBonus = ar.readInt();
        this.unarmedDieL = ar.readInt();
        this.unarmedNbrDieL = ar.readInt();

        this.maxMovement = ar.readByte();
        this.readyToTrain = ar.readBool();
        if (version >= _VERSION_0695_)
            this.canTradeItems = ar.readBool();

        if (version < 0.999702) {
            var tmp;

            tmp = ar.readByte(); this.SetPermStr(tmp);
            tmp = ar.readByte(); this.SetPermStrMod(tmp);
            tmp = ar.readByte(); this.SetPermInt(tmp);
            tmp = ar.readByte(); this.SetPermWis(tmp);
            tmp = ar.readByte(); this.SetPermDex(tmp);
            tmp = ar.readByte(); this.SetPermCon(tmp);
            tmp = ar.readByte(); this.SetPermCha(tmp);
            var strength_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
            var strengthMod_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
            var intelligence_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
            var wisdom_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
            var dexterity_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
            var constitution_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
            var charisma_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
        }
        else {
            die("Not Needed?"); //Not Implemented(0x442a, false);  // Cannot happen
        };
        this.openDoors = ar.readByte();
        this.openMagicDoors = ar.readByte();
        this.BB_LG = ar.readByte();
        this.hitBonus = ar.readInt();
        this.dmgBonus = ar.readInt();
        this.magicResistance = ar.readInt();
        {
            var bcl = new BASECLASS_STATS();
            var i, count;
            var count = ar.readInt();
            for (i = 0; i < count; i++) {
                bcl.Serialize(ar);
                this.baseclassStats.Add(bcl);
            };
        };
        this.spellAbility.valid = false;
        this.UpdateSpellAbility();
        this.IsPreGen = ar.readBool();

        if (version >= _VERSION_0698_)
            this.CanBeSaved = ar.readBool();

        if (version >= _VERSION_0900_)
            this.HasLayedOnHandsToday = ar.readBool();

        if (version < _VERSION_0661_) {
            temp = ar.readInt(); this.money.Coins[itemClassType.PlatinumType] = temp;
            temp = ar.readInt(); this.money.Coins[itemClassType.GoldType] = temp;
            temp = ar.readInt(); this.money.Coins[itemClassType.ElectrumType] = temp;
            temp = ar.readInt(); this.money.Coins[itemClassType.SilverType] = temp;
            temp = ar.readInt(); this.money.Coins[itemClassType.CopperType] = temp;
            temp = ar.readInt(); { for (var g = 0; g < temp; g++) this.money.AddGem(); }
            temp = ar.readInt(); { for (var j = 0; j < temp; j++) this.money.AddJewelry(); }
            if (version >= _VERSION_0660_) {
                temp = ar.readInt(); this.money.Coins[itemClassType.CoinType6] = temp;
                temp = ar.readInt(); this.money.Coins[itemClassType.CoinType7] = temp;
                temp = ar.readInt(); this.money.Coins[itemClassType.CoinType8] = temp;
                temp = ar.readInt(); this.money.Coins[itemClassType.CoinType9] = temp;
                temp = ar.readInt(); this.money.Coins[itemClassType.CoinType10] = temp;
            }
        }
        else {
            this.money.Serialize(ar, version);
        }
        this.NbrAttacks = ar.readDouble();

        if (version < _VERSION_0640_) {
            var tmp;
            var notused;
            this.DAS(ar, tmp);
            notused = ar.readLong();
            tmp = Globals.StripFilenamePath(tmp);
            //tmp = rte.IconArtDir() + tmp;
            //xAddArtFolderToPath(tmp);
            this.icon.filename = tmp;
            this.icon.picType = IconDib;
            this.icon.SetDefaults();
        }
        else {
            icon.Serialize(ar, version, rte.IconArtDir());
            if (version <= _VERSION_0840_)
                icon.SetPre0840NonLoopDefaults();
            icon.filename = Globals.StripFilenamePath(icon.filename);  // Some old saves have a path built in
        }
        if (version >= _VERSION_0640_)
            this.iconIndex = ar.readInt();
        this.origIndex = ar.readInt();
        this.uniquePartyID = ar.readByte();

        if (version >= _VERSION_0870_)
            this.DisableTalkIfDead = ar.readBool();
        if (version >= _VERSION_0662_)
            this.TalkEvent = ar.readDword();
        if (version >= _VERSION_0710_)
            this.DAS(ar, TalkLabel);
        if (version >= _VERSION_0800_) {
            this.ExamineEvent = ar.readDword();
            this.DAS(ar, ExamineLabel);
        }

        this.m_spellbook.Serialize(ar, version);


        if (version >= _VERSION_06991_) {
            if (version < _VERSION_0850_) {
                var unused1;
                unused1 = ar.readBool();
            }
            this.detectingInvisible = ar.readBool();
            this.detectingTraps = ar.readBool();
        }
        if (version >= _VERSION_0630_) {
            if (version < _VERSION_06991_) {
                this.m_spellEffects.Serialize(ar);
            }
            else {
                this.m_spellEffects.RemoveAll();
                var count;
                count = ar.readInt();
                for (var i = 0; i < count; i++)
                {
                    var data = new SPELL_EFFECTS_DATA();
                    data.Serialize(ar, version);
                    this.m_spellEffects.AddTail(data);
                    Globals.debug("Loading spell effect for " + GetName() + ": " + data.AffectedAttr() + "\n");
                }
            }
        }
        if (version >= _VERSION_0696_)
            blockageData.Serialize(ar);

        this.UpdateSpellAbility();
    }

    this.smallPic.Serialize(ar, version, rte.PicArtDir());
    if (!ar.IsStoring()) {
        if (version <= _VERSION_0840_)
            this.smallPic.SetPre0840LoopForeverDefaults();
    }

    this.myItems.Serialize(ar, version);
    if (ar.IsLoading() && (this.characterVersion == 0)) {
        // Old versions included armor and such in m_AC.
        // We need to subtract it out for newer versions.
        this.m_AC -= this.myItems.GetProtectModForRdyItems();
    };
    this.specAbs.Serialize(ar, version, this.name, "char");
    //specAbQueue: no need to save this data
    this.char_asl.Serialize(ar, "CHARACTER_ATTRIBUTES");
    {
        // Make a note of any spells that the character has but which are
        // missing from the spell database.
        var i, n;
        n = m_spellbook.GetCount();
        for (i = 0; i < n; i++) {
            var pCharSp;
            pCharSp = m_spellbook.PeekCharacterSpell(i);
            if (!pCharSp.spellID.IsValidSpell()) {
                UAFUtil.WriteDebugString("Character " + GetName() + " is missing spell " + pCharSp.spellID.UniqueName() + "\n");
            };
        };
    };

    this.encumbrance = this.determineEffectiveEncumbrance();

    this.DisplayCurrSpellEffects("Serialize");
};

CHARACTER.prototype.SerializeCAR = function (car, version) { 
    // This data can be serialized along with the globalData, and
    // also separately as a 'CharName.CHR' file.
    // The version specified distinguishes between saved char file 
    // version and game.dat file version
    //
    var characterVersion = 0;
    if (car.IsStoring()) {
        // **PORT NOTE: I think temp was only used here to cast to an int, but I kept it for documentation/just in case **
        var temp;
        car.writeInt(Globals.CHARACTER_VERSION);
        car.writeInt(this.preSpellNamesKey);
        car.writeByte(this.type);
        car.writeString(this.race);
        temp = this.gender;
        car.writeInt(temp);
        car.writeString(this.classID);
        temp = this.alignment;
        car.writeInt(temp);
        car.writeBool(this.allowInCombat);
        temp = this.status;
        car.writeInt(temp);
        car.writeString(this.undeadType);
        temp = this.creatureSize;
        car.writeInt(temp);

        AS(car, this.name);
        AS(car, this.characterID);
        car.writeInt(this.THAC0);
        car.writeInt(this.morale);
        car.writeInt(this.encumbrance);
        car.writeInt(this.maxEncumbrance);
        car.writeInt(this.m_AC);
        car.writeInt(this.hitPoints);
        car.writeInt(this.maxHitPoints);
        car.writeDouble(this.nbrHitDice);
        car.writeInt(this.age);
        car.writeInt(this.maxAge);
        car.writeInt(this.birthday);
        car.writeInt(this.m_iMaxCureDisease);
        car.writeInt(this.unarmedDieS);
        car.writeInt(this.unarmedNbrDieS);
        car.writeInt(this.unarmedBonus);
        car.writeInt(this.unarmedDieL);
        car.writeInt(this.unarmedNbrDieL);
        car.writeByte(this.maxMovement);
        car.writeBool(this.readyToTrain);
        car.writeBool(this.canTradeItems);

        car.writeInt(this.GetPermStr());
        car.writeInt(this.GetPermStrMod());
        car.writeInt(this.GetPermInt());
        car.writeInt(this.GetPermWis());
        car.writeInt(this.GetPermDex());
        car.writeInt(this.GetPermCon());
        car.writeInt(this.GetPermCha());

        car.writeByte(this.openDoors);
        car.writeByte(this.openMagicDoors);
        car.writeByte(this.BB_LG);
        car.writeInt(this.hitBonus);
        car.writeInt(this.dmgBonus);
        car.writeInt(this.magicResistance);
        {
            var BSVersion = "BS0";
            var i, count;
            car.writeCString(BSVersion);
            count = this.GetBaseclassStatsCount();
            car.writeInt(count);
            for (i = 0; i < count; i++) {
                this.GetBaseclassStats(i).Serialize(car, BSVersion);
                //baseclassStats[i].Serialize(car, BSVersion);
            };
        }
        {
            var SAVersion = "SA0";
            var i, count;
            count = GetSkillAdjCount();
            car.writeCString(SAVersion);
            car.writeInt(count);
            for (i = 0; i < count; i++) {
                GetSkillAdj(i).Serialize(car, SAVersion);
                //skillAdjustments[i].Serialize(car, SAVersion);
            };
        };
        {
            var SAVersion = "SA0";
            var i, count;
            count = GetSpellAdjCount();
            car.writeCString(SAVersion);
            car.writeInt(count);
            for (i = 0; i < count; i++) {
                GetSpellAdj(i).Serialize(car, SAVersion);
                //spellAdjustments[i].Serialize(car, SAVersion);
            };
        };
        car.writeBool(this.IsPreGen);
        car.writeBool(this.CanBeSaved);
        car.writeBool(this.HasLayedOnHandsToday);
        this.money.Serialize(car, version);
        car.writeDouble(this.NbrAttacks);
        this.icon.Serialize(car, version, rte.IconArtDir());
        car.writeInt(this.iconIndex);
        car.writeInt(this.origIndex);
        car.writeByte(this.uniquePartyID);
        car.writeBool(this.DisableTalkIfDead);
        car.writeDword(this.TalkEvent);
        this.AS(car, this.TalkLabel);
        car.writeDword(this.ExamineEvent);
        this.AS(car, this.ExamineLabel);

        this.m_spellbook.Serialize(car, version);

        //ar.write(detectingMagic;
        car.writeBool(this.detectingInvisible);
        car.writeBool(this.detectingTraps);
        car.writeInt(this.m_spellEffects.GetCount());
        var pos = this.m_spellEffects.GetHeadPosition();
        while (pos != null) {
            this.m_spellEffects.GetNext(pos).Serialize(car, version);
        };
        blockageData.Serialize(car);
    }
    else {
        var temp;
        temp = car.readInt();  // Possible character version
        if ((temp & UAFUtil.ByteFromHexString("0x80000000")) != 0) {
            // This is a character version, not an index.
            this.characterVersion = temp;
            if (version >= 0.998917) {
                //ar >> uniqueKey;
                this.preSpellNamesKey = car.readInt();
            };
        }
        else {
            this.characterVersion = 0; // This is a key, not a version.
            //uniqueKey = temp;
        };
        this.type = car.readByte();
        this.race = car.readString();            //PORT NOTE: Changed this to read a string since this class was subclassed from CString
        this.gender = car.readInt();
        this.classID = car.readString();        //PORT NOTE: Changed this to read a string since this class was subclassed from CString
        this.alignment = car.readInt();

        if (version >= _VERSION_0912_)
            this.allowInCombat = car.readBool();

        this.status = car.readInt();
        {
            //undead = (undeadType)temp;
            //undeadLevel = temp;
            undeadType = car.readByte();      //** TODO: String reader needs to account for 1 0-byte entry as an empty string **//
        };

        creatureSize = creatureSizeType.get(car.readInt());

        this.name = DAS(car);
        {
            this.characterID = car.readString();  //PORT NOTE: Changed this to read a string since this class was subclassed from CString
        }
        this.THAC0 = car.readInt();
        this.morale = car.readInt();
        this.encumbrance = car.readInt();
        this.maxEncumbrance = car.readInt();
        this.m_AC = car.readInt();
        this.hitPoints = car.readInt();
        this.maxHitPoints = car.readInt();
        this.nbrHitDice = car.readDouble();
        this.age = car.readInt();
        this.maxAge = car.readInt();
        if (version >= _VERSION_0830_) {
            this.birthday = car.readInt();
            this.SetBirthday(this.birthday);
        }

        this.m_iMaxCureDisease = car.readInt();
        this.unarmedDieS = car.readInt();
        this.unarmedNbrDieS = car.readInt();
        this.unarmedBonus = car.readInt();
        this.unarmedDieL = car.readInt();
        this.unarmedNbrDieL = car.readInt();

        this.maxMovement = car.readInt();
        this.readyToTrain = car.readByte();     // PORT NOTE:  Not sure why, but this seems to be stored as a single byte  -- **TODO** Investigate
        if (version >= _VERSION_0695_)
            this.canTradeItems = car.readBool();

        if (version < 0.999702) {
            var tmp;
            tmp = car.readByte(); this.strength = tmp;
            tmp = car.readByte(); this.strengthMod = tmp;
            tmp = car.readByte(); this.intelligence = tmp;
            tmp = car.readByte(); this.wisdom = tmp;
            tmp = car.readByte(); this.dexterity = tmp;
            tmp = car.readByte(); this.constitution = tmp;
            tmp = car.readByte(); this.charisma = tmp;
        }
        else {
            var tmp;
            tmp = car.readInt(); this.strength = tmp;
            tmp = car.readInt(); this.strengthMod = tmp;
            tmp = car.readInt(); this.intelligence = tmp;
            tmp = car.readInt(); this.wisdom = tmp;
            tmp = car.readInt(); this.dexterity = tmp;
            tmp = car.readInt(); this.constitution = tmp;
            tmp = car.readInt(); this.charisma = tmp;
        };
        this.strength_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
        this.strengthMod_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
        this.intelligence_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
        this.wisdom_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
        this.dexterity_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
        this.constitution_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
        this.charisma_adjustment = UAFUtil.ByteFromHexString("0x7fffffff");
        this.openDoors = car.readByte();
        this.openMagicDoors = car.readByte();
        this.BB_LG = car.readByte();
        this.hitBonus = car.readInt();
        this.dmgBonus = car.readInt();
        this.magicResistance = car.readInt();

        {
            var i, count;
            var BSVersion = car.readString();
            var count = car.readInt();
            for (i = 0; i < count; i++) {
                var bcl = new BASECLASS_STATS();
                bcl.SerializeCAR(car, BSVersion);
                this.baseclassStats.push(bcl);
            };
        };
        {/** TODO - this block - my test cases had no SKILL_ADJ records**/
            var i, count;
            var SAVersion;
            SAVersion = car.readString();
            count = car.readInt();
            for (i = 0; i < count; i++) {
                var sa = new SKILL_ADJ();
                var isk;
                sa.Serialize(car, SAVersion);
                isk = LocateSkillAdj(sa.skillID, sa.adjID);
                if (isk >= 0) {
                    DeleteSkillAdj(isk);
                };
                this.skillAdjustments.Add(sa);
            };
        };
        {/** TODO - this block - my test cases had no SA records**/
            var i, count;
            var SAVersion;
            SAVersion = car.readString();
            count = car.readInt();
            for (i = 0; i < count; i++) {
                var sa = new SPELL_ADJ();
                sa.Serialize(car, SAVersion);
                this.spellAdjustments.Add(sa);
            };
        };

        this.IsPreGen = car.readBool();

        if (version >= _VERSION_0698_)
            this.CanBeSaved = car.readBool();

        if (version >= _VERSION_0900_)
            this.HasLayedOnHandsToday = car.readBool();

        if (version < _VERSION_0661_) {
            temp = car.readInt(); this.money.Coins[itemClassType.PlatinumType] = temp;
            temp = car.readInt(); this.money.Coins[itemClassType.GoldType] = temp;
            temp = car.readInt(); this.money.Coins[itemClassType.ElectrumType] = temp;
            temp = car.readInt(); this.money.Coins[itemClassType.SilverType] = temp;
            temp = car.readInt(); this.money.Coins[itemClassType.CopperType] = temp;
            temp = car.readInt(); { for (var g = 0; g < temp; g++) this.money.AddGem(); }
            temp = car.readInt(); { for (var j = 0; j < temp; j++) this.money.AddJewelry(); }
            if (version >= _VERSION_0660_) {
                temp = car.readInt(); this.money.Coins[itemClassType.CoinType6] = temp;
                temp = car.readInt(); this.money.Coins[itemClassType.CoinType7] = temp;
                temp = car.readInt(); this.money.Coins[itemClassType.CoinType8] = temp;
                temp = car.readInt(); this.money.Coins[itemClassType.CoinType9] = temp;
                temp = car.readInt(); this.money.Coins[itemClassType.CoinType10] = temp;
            }
        }
        else {
            this.money.SerializeCAR(car, version);
        }
        this.NbrAttacks = car.readFloat();

        if (version < _VERSION_0640_) {
            var tmp;
            var notused;
            this.DAS(car, tmp);
            notused = car.readLong();
            tmp = Globals.StripFilenamePath(tmp);
            //tmp = rte.IconArtDir() + tmp;
            //xAddArtFolderToPath(tmp);
            this.icon.filename = tmp;
            this.icon.picType = IconDib;
            this.icon.SetDefaults();
        }
        else {
            this.icon.SerializeCAR(car, version, RUNTIME_ENVIRONMENT.IconArtDir());
            if (version <= _VERSION_0840_)
                this.icon.SetPre0840NonLoopDefaults();
            this.icon.filename = Globals.StripFilenamePath(this.icon.filename);
        }

        if (version >= _VERSION_0640_)
            this.iconIndex = car.readInt(); // 1
        this.origIndex = car.readInt();    // -1
        if (version < 0.998917) {
            this.preSpellNamesKey = origIndex;  // 0-does not go in here
        };
        this.uniquePartyID = car.readByte();        // 255

        if (version >= _VERSION_0870_)
            this.DisableTalkIfDead = car.readInt();  // Druid: 160:8?    // PORT NOTE:  Not sure why, but this boolean seems to be stored as in int with 4 bytes
        if (version >= _VERSION_0662_)
            this.TalkEvent = car.readDWORD();
        if (version >= _VERSION_0710_)
            this.TalkLabel = DAS(car);
        if (version >= _VERSION_0800_) {
            this.ExamineEvent = car.readDWORD();
            this.ExamineLabel = DAS(car);
        }

        this.m_spellbook.SerializeCAR(car, version);


        if (version >= _VERSION_06991_) {
            if (version < _VERSION_0850_) {
                var unused1;
                unused1 = car.readBool();
            }
            this.detectingInvisible = car.readBool();
            this.detectingTraps = car.readBool();
        }
        if (version >= _VERSION_0630_) {
            if (version < _VERSION_06991_) {
            /** TODO: Add support
                //m_spellEffects.Serialize(ar);
                // 20121208 PRS
                var i, n;
                var sed = new SPELL_EFFECTS_DATA();
                n = car.readInt();
                for (i = 0; i < n; i++) {
                    sed.Serialize(car, version);
                    this.m_spellEffects.AddTail(sed);
                };
                **/
            }
            else {
                this.m_spellEffects.RemoveAll();
                var count;
                count = car.readInt();
                for (var i = 0; i < count; i++) {
                    var data = new SPELL_EFFECTS_DATA();
                    data.Serialize(car, version);
                    this.m_spellEffects.AddTail(data);
                    UAFUtil.WriteDebugString("Loading spell effect for " + GetName() + ": " + data.AffectedAttr() + "\n");
                }
            }
        }
        if (version >= _VERSION_0696_)
            this.blockageData.SerializeCAR(car);

        //UpdateSpellAbility();
        // 20101217TEMP m_spellbook.SortSpells();
    }
    this.smallPic.SerializeCAR(car, version, RUNTIME_ENVIRONMENT.PicArtDir());
    if (!car.IsStoring()) {
        if (version <= _VERSION_0840_)
            this.smallPic.SetPre0840LoopForeverDefaults();
    }
    this.myItems.SerializeCAR(car, version);
    if (car.IsLoading() && (this.characterVersion == 0)) {
        // Old versions included armor and such in m_AC.
        // We need to subtract it out for newer versions.
        this.m_AC -= this.myItems.GetProtectModForRdyItems();
    };
    this.specAbs.SerializeCAR(car, version, this.name, "char");
    //specAbQueue: no need to save this data
    this.char_asl.SerializeCAR(car, "CHARACTER_ATTRIBUTES");

    // design settings that affect encumbrance might
    // have changed since character was last saved
    this.encumbrance = this.determineEffectiveEncumbrance();

    this.DisplayCurrSpellEffects("Serialize");
    this.spellAbility.Clear();  // force computation.
}


//CHARACTER.prototype.serializeCharacter = function (save, fname) { }  OOS

//CHARACTER.prototype.SaveArt = function () { };  OOS
//CHARACTER.prototype.CrossReference = function(pCRList){};  OOS
CHARACTER.prototype.getCharWeaponText = function (wpn, dmg) {
    var wpn = "";
    var dmg = "";

    var temp;
    var temp2;

    temp = "";
    temp2 = "";
    var dmg_bonus = 0;

    var wpnHand = this.myItems.GetReadiedItem(WeaponHand, 0);

    if (wpnHand != Items.NO_READY_ITEM) {
        var itemID = new ITEM_ID();
        itemID = this.myItems.GetItem(wpnHand);

        data = this.itemData.GetItem(itemID);

        if (data != null) {
            if ((Items.isMagical(itemID)) && (this.myItems.GetId(wpnHand))) {
                wpn = data.IdName();
            }
            else {
                if (data.Wpn_Type == SpellLikeAbility) {
                    var n;
                    n = data.IdName().indexOf('|');
                    if (n < 0) {
                        wpn.Empty();
                    }
                    else {
                        wpn = data.IdName().substr(n+1);
                    };
                }
                else {
                    wpn = data.CommonName();
                };
            };

            var isMissile = false;
            // if using bow or crossbow, use ammo damage
            if ((data.Wpn_Type == Bow) || (data.Wpn_Type == Crossbow)) {
                var ammoHand = this.myItems.GetReadiedItem(AmmoQuiver, 0);
                if (ammoHand != Items.NO_READY_ITEM) {
                    var itemName;
                    itemName = this.myItems.GetItem(ammoHand);
                    data = this.itemData.GetItem(itemID);
                    isMissile = true;
                }
            }

            if ((data.Dmg_Dice_Sm > 0) && (data.Nbr_Dice_Sm > 0)) {
                dmg_bonus = data. Dmg_Bonus_Sm;
                temp.Format(data.Nbr_Dice_Sm + "D" + data.Dmg_Dice_Sm);

                if ((Items.isMagical(itemID)) && (this.myItems.GetId(wpnHand)))
                    dmg_bonus += max(data.Attack_Bonus, data.Dmg_Bonus_Sm);

                if (!isMissile)
                    dmg_bonus += GetAdjDmgBonus();

                if (dmg_bonus > 0) {
                    temp2 = "+" + dmg_bonus;
                    temp += temp2;
                }
            }

            dmg = temp;
        }
    }
    else {
        temp.Format(unarmedNbrDieS + "D" + unarmedDieS);

        dmg_bonus = unarmedBonus;
        dmg_bonus += this.GetAdjDmgBonus();

        if (dmg_bonus > 0) {
            temp2 = "+%i" + dmg_bonus;
            temp += temp2;
        }

        dmg = temp;
        wpn = "NONE";
    }

    var returnObj = {};
    returnObj.dmg = dmg;
    returnObj.wpn = wpn;
    return returnObj;
};

CHARACTER.prototype.getCharArmorText = function () {
    var index = this.myItems.GetReadiedItem(this.BodyArmor, 0); // dude's ready armor, if any

    if (index != Items.NO_READY_ITEM) {
        var itemData = this.itemData.GetItem(itemID);
        if ((Items.isMagical(this.myItems.GetItem(index))) && (this.myItems.GetId(index)))
            armor = itemData.GetItemIdName(myItems.GetItem(index));
        else
            armor = itemData.GetItemCommonName(myItems.GetItem(index));
    }
    else
        armor = "";

    return armor;

};

CHARACTER.prototype.getInfo = function () {
    var result;
    result = "name=" + this.GetName() + ";" +
                "pos=" + this.uniquePartyID + ";" +
                "THAC0=" + this.GetAdjTHAC0() + ";" +
                "AC=" + this.GetAdjAC() + ";" +
                "age=" + this.GetAdjAge() + ";" +
                "maxage=" + this.GetAdjMaxAge() + ";" +
                "HP=" + this.GetAdjHitPoints() + ";" +
                "maxHP=" + this.GetMaxHitPoints() + ";" +
                "enc=" + this.GetEncumbrance() + ";" +
                "maxenc=" + this.maxEncumbrance + ";" +
                "maxmove=" + this.GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, "Retrieve character stats - max movement") + ";" +
                "STR=" + this.GetAdjStr() + ";" +
                "INT=" + this.GetAdjInt() + ";" +
                "WIS=" + this.GetAdjWis() + ";" +
                "DEX=" + this.GetAdjDex() + ";" +
                "CON=" + this.GetAdjCon() + ";" +
                "CHA=" + this.GetAdjCha() + ";" +
                "race=" + RACE_DATA_TYPE.GetRaceName(this.race) + ";" +
                "gender=" + (this.GetAdjGender() == genderType.Male ? "male" : "female") + ";" +
                "class=" + CLASS_DATA_TYPE.PeekClass(this.GetClass()).m_name + ";" +
                "align=" + alignmentType.GetAlignmentName(this.GetAlignment()) + ";"

    return result;
};

CHARACTER.prototype.getItemList = function (id) {
    //id is an identification number that the caller would like
    //associated with each of the items in this character's possession
    var result;
    var itemID;
    var temp;
    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {
        itemID = this.myItems.PeekAtPos(pos).itemID;
        var itemData = this.itemData.GetItem(itemID);
        temp.Format("{" + itemData.GetItemUniqueName(itemID) + "[" + id + "]}");
        result += temp;
        this.myItems.GetNext(pos);
    }
    return result;
};

CHARACTER.prototype.addCharacterItem = function (itemID, qty, numCharges, id, cost) {
    if ((Items.getItemEncumbrance(itemID, qty) + this.GetEncumbrance()) > this.GetAdjMaxEncumbrance()) {
        Globals.SetMiscError(Globals.miscErrorType.TooMuchWeight);
        return false;
    }

    if (!this.myItems.addItem(itemID, qty, numCharges, id, cost)) {
        UAFUtil.WriteDebugString("Failed to addItem to character\n");
        return false;
    }

    this.encumbrance = this.determineEffectiveEncumbrance();
    this.determineMaxMovement();

    return true;
};

CHARACTER.prototype.delCharacterItem = function (index, qty) {
    if (!this.myItems.deleteItem(index, qty)) {
        UAFUtil.WriteDebugString("Failed to deleteItem in delCharacterItem()\n");
        return false;
    }

    this.encumbrance = this.determineEffectiveEncumbrance();
    this.determineMaxMovement();
    return true;
};

CHARACTER.prototype.HasItem = function (itemID) {
    return this.myItems.HaveItem(itemID);
};

CHARACTER.prototype.buyItem = function (itemID, type) {

    if (this.GetAdjStatus() != charStatusType.Okay) {
        switch (this.GetAdjStatus()) {
            case charStatusType.Unconscious: Globals.SetMiscError(Globals.miscErrorType.UnconsciousChar); break;
            case charStatusType.Dead: Globals.SetMiscError(Globals.miscErrorType.DeadChar); break;
            case charStatusType.Fled: Globals.SetMiscError(Globals.miscErrorType.FledChar); break;
            case charStatusType.Petrified: Globals.SetMiscError(Globals.miscErrorType.PetrifiedChar); break;
            case charStatusType.Gone: Globals.SetMiscError(Globals.miscErrorType.GoneChar); break;
            case charStatusType.Animated: Globals.SetMiscError(Globals.miscErrorType.AnimatedChar); break;
            case charStatusType.TempGone: Globals.SetMiscError(Globals.miscErrorType.TempGoneChar); break;
            case charStatusType.Running: Globals.SetMiscError(Globals.miscErrorType.RunningChar); break;
            case charStatusType.Dying: Globals.SetMiscError(Globals.miscErrorType.DyingChar); break;
            default: Globals.SetMiscError(Globals.miscErrorType.UnknownError); break;
        }
        return;
    }

    var data = this.itemData.GetItem(itemID);
    if (data == null)
        return;

    var cost = this.ApplyCostFactor(cf, data.Cost);

    if (!this.enoughMoney(cost, 0, 0, globalData.moneyData.GetDefaultType())) {
        Globals.SetMiscError(Globals.miscErrorType.NotEnoughMoney);
        return;
    }

    if ((Items.getItemEncumbrance(itemID, 1) + this.GetEncumbrance()) > this.GetAdjMaxEncumbrance())
        Globals.SetMiscError(Globals.miscErrorType.TooMuchWeight);
    else {
        var bundle = Math.max(data.Bundle_Qty, 1);
        if (!this.addCharacterItem(itemID, bundle, data.Num_Charges, true, cost))
            Globals.SetMiscError(Globals.miscErrorType.MaxItemsReached);
        else
            this.payForItem(cost, globalData.moneyData.GetDefaultType());
    }
};

CHARACTER.prototype.payForItem = function (moneyCost, moneyType, gems, jewelry) {
    if (!gems) { gems = 0; }
    if (!jewelry) { jewelry = 0; }

    if (moneyCost > 0) {
        if (this.GetParty() != null && (this.GetParty().moneyPooled) && (this.GetParty().poolSack.HaveEnough(moneyType, moneyCost)))
            this.GetParty().poolSack.Subtract(moneyType, moneyCost);
        else
            this.money.Subtract(moneyType, moneyCost);
    }

    if (gemCost > 0)
        this.money.RemoveMultGems(gemCost);

    if (jewelryCost > 0)
        this.money.RemoveMultJewelry(jewelryCost);

    this.encumbrance = this.determineEffectiveEncumbrance();
    this.determineMaxMovement();

};

CHARACTER.prototype.IsIdentified = function (key, num) {
    var itemID = new ITEM_ID();
    var pITEM, pNextITEM;
    var pItem;
    var newVal;
    var count;
    var pos;

    //** PORT NOTE:  It looks to me like this is "is the n'th item of type key in the character's inventory identified.  And if n=0, then it's the just the first one in the inventory **/
    // e.g. Is the 4th "long sword + 1" in the character's inventory identified? 

    pITEM = this.myItems.GetITEM(key);
    if (pITEM == null) return -1;
    itemID = pITEM.itemID;
    pItem = this.itemData.GetItem(itemID);
    if (pItem == null) return -1;
    if (num == 0) {
        return (pITEM.identified) ? 1 : 0;
    };
    if (num > 0) { newVal = true; }
    else { newVal = false; num = -num; };
    count = 0;
    if (pITEM.identified != newVal) {
        pITEM.identified = newVal;
        count++;
    };
    num--;
    pos = myItems.GetHeadPosition();
    while ((pos != null) && (num > 0)) {
        pNextITEM = this.myItems.GetAtPos(pos);
        pos = this.myItems.NextPos(pos);             //PORT NOTE: Added - no out params in Javascript, so have to advance pointer (pos)
        if (pNextITEM != pITEM) {
            if (pNextITEM.itemID == itemID) {
                if (pNextITEM.identified != newVal) {
                    pNextITEM.identified = newVal;
                    count++;
                    num--;
                };
            };
        };
        this.myItems.GetNext(pos);
    };
    return count;
};

CHARACTER.prototype.CanReady = function (index) {
    if (index != Items.NO_READY_ITEM) {
        {
            var actor;
            var pItem;
            var hookParameters = new HOOK_PARAMETERS();
            var scriptContext = new SCRIPT_CONTEXT();
            var answer;
            actor = this.GetContext();
            actor = RunTimeIF.SetCharContext(actor);
            pItem = this.itemData.GetItem(this.myItems.GetItem(index));
            scriptContext.SetCharacterContext(this);
            scriptContext.SetItemContext(pItem);
            scriptContext.SetItemContextKey(index);
            pItem.RunItemScripts(SPECAB.CAN_READY,
                                SPECAB.ScriptCallback_RunAllScripts,
                                 null,
                                "Test if character can ready item");
            answer = this.RunCharacterScripts(SPECAB.CAN_READY,
                                         SPECAB.ScriptCallback_RunAllScripts,
                                         null,
                                         "Test if character can ready item");
            RunTimeIF.ClearCharContext();
            if ((answer.GetLength() > 0) && (answer.GetAt(0) == 'N')) {
                return false;
            };
        };
    };
    return false;
};

CHARACTER.prototype.toggleReadyItem = function (item) {
    var success = false;
    var rdyLoc;
    var data = this.itemData.GetItem(this.myItems.GetItem(item));
    if (data == null) return false;
    rdyLoc = data.Location_Readied;
    if (this.myItems.IsReady(item)) {
        this.myItems.UnReady(item);
        success = !this.myItems.IsReady(item);
        if (success) {
            if (rdyLoc == Items.WeaponHand) UnReadyWeaponScript(item);
            else if (rdyLoc == Items.ShieldHand) UnReadyShieldScript(item);
            else if (rdyLoc == Items.BodyArmor) UnReadyArmorScript(item);
            else if (rdyLoc == Items.Hands) UnReadyGauntletsScript(item);
            else if (rdyLoc == Items.Head) UnReadyHelmScript(item);
            else if (rdyLoc == Items.Waist) UnReadyBeltScript(item);
            else if (rdyLoc == Items.BodyRobe) UnReadyRobeScript(item);
            else if (rdyLoc == Items.Back) UnReadyCloakScript(item);
            else if (rdyLoc == Items.Feet) UnReadyBootsScript(item);
            else if (rdyLoc == Items.Fingers) UnReadyRingScript(item);
            // should separate this into ring1 and ring2?
            else if (rdyLoc == Items.AmmoQuiver) UnReadyAmmoScript(item);
            else UnReadyXXXScript(SPECAB.ON_UNREADY, item);
        };
    }
    else {
        if (rdyLoc == Items.WeaponHand) ReadyWeaponScript(item);
        else if (rdyLoc == Items.ShieldHand) ReadyShieldScript(item);
        else if (rdyLoc == Items.BodyArmor) ReadyArmorScript(item);
        else if (rdyLoc == Items.Hands) ReadyGauntletsScript(item);
        else if (rdyLoc == Items.Head) ReadyHelmScript(item);
        else if (rdyLoc == Items.Waist) ReadyBeltScript(item);
        else if (rdyLoc == Items.BodyRobe) ReadyRobeScript(item);
        else if (rdyLoc == Items.Back) ReadyCloakScript(item);
        else if (rdyLoc == Items.Feet) ReadyBootsScript(item);
        else if (rdyLoc == Items.Fingers) ReadyRingScript(item);
        // should separate this into ring1 and ring2?
        else if (rdyLoc == Items.AmmoQuiver) ReadyAmmoScript(item);
        else ReadyXXXScript(data.Location_Readied, SPECAB.ON_READY, item);
        success = this.myItems.IsReady(item);
    }

    if (success)
        this.SetCharAC();

    return success;
};

CHARACTER.prototype.ReadyBestWpn = function (dist, isLargeTarget) {

    this.ReadyWeaponScript(Items.NO_READY_ITEM);

    // need to check for usable items
    // need to check for targets requiring magical weapons

    var dmgIdx = Items.NO_READY_ITEM;
    var dmg = 0;
    var dmgSum = 0;
    var defIdx = Items.NO_READY_ITEM;
    var def = 0;
    var attIdx = Items.NO_READY_ITEM;
    var att = 0;
    var data;

    // determine which avail item has best damage, 
    // attack, and protection values for given distance
    // to target
    //
    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {
        if ((data = this.itemData.GetItem(myItems.PeekAtPos(pos).itemID)) != null) {
            if (data.Location_Readied == Items.WeaponHand) {
                var err;
                err = this.myItems.CanReadyItem(this.myItems.PeekAtPos(pos), this);
                if (err == Globals.miscErrorType.NoError) {
                    if (Items.WpnCanAttackAtRange(this.myItems.PeekAtPos(pos).itemID, dist)) {
                        if (data.Protection_Base + data.Protection_Bonus <= def) {
                            def = data.Protection_Base + data.Protection_Bonus;
                            defIdx = this.myItems.GetKeyAt(pos);
                        }

                        if (isLargeTarget)
                            dmgSum = (data.Dmg_Dice_Lg * data.Nbr_Dice_Lg) + data.Dmg_Bonus_Lg;
                        else
                            dmgSum = (data.Dmg_Dice_Sm * data.Nbr_Dice_Sm) + data.Dmg_Bonus_Sm;

                        // check for fighter multiple attacks (swords/daggers only?)

                        if (data.ROF_Per_Round > 0)
                            dmgSum *= data.ROF_Per_Round;

                        if (dmgSum >= dmg) {
                            dmg = dmgSum;
                            dmgIdx = this.myItems.GetKeyAt(pos);
                        }

                        if (data.Attack_Bonus.att) {
                            att = data.Attack_Bonus;
                            attIdx = this.myItems.GetKeyAt(pos);
                        };
                    };
                }
                else {
                    Globals.SetMiscError(err);
                };
            };
        };
        this.myItems.GetNext(pos);
    };
    // clear miscError that may have been set by CanReady()
    Globals.SetMiscError(Globals.miscErrorType.NoError);

    // we now have best attack, protection, and damage items, if any,
    // for given distance to target

    // total damage potential per round
    var wpn_sum = dmgSum; // already includes rate of fire for weapon
    this.DetermineUnarmedHitDice();

    // ignore potential for natural attacks if
    // too far away to use them (distance to targ > 1)
    //
    if (dist <= 1) {
        var nonwpn_sum = 0;
        if (!isLargeTarget)
            nonwpn_sum = (unarmedDieS * unarmedNbrDieS) + unarmedBonus;
        else
            nonwpn_sum = (unarmedDieL * unarmedNbrDieL) + unarmedBonus;

        nonwpn_sum *= GetNbrAttacks();

        // better off using natural attacks
        if (wpn_sum < nonwpn_sum) return;
    }

    // for now, choose item with most damage first
    var IdxToUse = dmgIdx;
    if (IdxToUse == Items.NO_READY_ITEM)
        IdxToUse = attIdx;
    if (IdxToUse == Items.NO_READY_ITEM)
        IdxToUse = defIdx;
    if (IdxToUse == Items.NO_READY_ITEM)
        return;

    data = this.itemData.GetItem(myItems.GetItem(IdxToUse));
    if (data != null) {
        if (data.Hands_to_Use > 1)
            myItems.UnReady(this.myItems.GetReadiedItem(Items.ShieldHand, 0));
    }

    this.ReadyWeaponScript(IdxToUse);
    if (Items.WpnUsesAmmo(this.myItems.GetItem(IdxToUse)))
        this.ReadyBestAmmo(isLargeTarget);
};

CHARACTER.prototype.ReadyBestShield = function () {
    // if 2-handed weapon being used, can't ready a shield
    if ((this.myItems.GetReadiedItem(ShieldHand, 0) != Items.NO_READY_ITEM)
        && (this.myItems.GetReadiedItem(ShieldHand, 0) == this.myItems.GetReadiedItem(Items.WeaponHand, 0)))
        return;

    this.ReadyShieldScript(Items.NO_READY_ITEM);

    var defIdx = Items.NO_READY_ITEM;
    var def = 0;
    var data;

    // determine which avail item has best
    // protection value
    //
    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {
        if ((data = this.itemData.GetItem(this.myItems.PeekAtPos(pos).itemID)) != null) {
            if (data.Location_Readied == Items.ShieldHand) {
                var err;
                err = this.myItems.CanReadyItem(myItems.PeekAtPos(pos), this);
                if (err == Globals.miscErrorType.NoError) {
                    if (data.Protection_Base + data.Protection_Bonus < def) {
                        def = data.Protection_Base + data.Protection_Bonus;
                        defIdx = myItems.GetKeyAt(pos);
                    }
                }
                else {
                    Globals.SetMiscError(err);
                };
            }
        };
        this.myItems.GetNext(pos);
    }

    // clear miscError that may have been set by CanReady()
    Globals.SetMiscError(Globals.miscErrorType.NoError);

    // else try for best defensive bonus item
    if (defIdx != Items.NO_READY_ITEM) {
        this.ReadyShieldScript(defIdx);
        return;
    }
};

CHARACTER.prototype.ReadyBestArmor = function () {
    this.ReadyArmorScript(Items.NO_READY_ITEM);  //Discard current Armor

    var defIdx = Items.NO_READY_ITEM;
    var def = 0;
    var data;

    // determine which avail item has best
    // protection value
    //
    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {
        if ((data = this.itemData.GetItem(this.myItems.PeekAtPos(pos).itemID)) != null) {

            if (data.Location_Readied == Items.BodyArmor) {
                var err;
                err = this.myItems.CanReadyItem(myItems.PeekAtPos(pos), this);
                if (err == Globals.miscErrorType.NoError) {
                    if (data.Protection_Base + data.Protection_Bonus < def) {
                        def = data.Protection_Base + data.Protection_Bonus;
                        defIdx = myItems.GetKeyAt(pos);
                    }
                }
                else {
                    Globals.SetMiscError(err);
                };
            };
        };
        this.myItems.GetNext(pos);
    }

    // clear miscError that may have been set by CanReady()
    Globlals.SetMiscError(Globlals.miscErrorType.NoError);

    // else try for best defensive bonus item
    if (defIdx != Items.NO_READY_ITEM) {
        this.ReadyArmorScript(defIdx);
        return;
    }
};

CHARACTER.prototype.ReadyBestAmmo = function (isLargeTarget) {
    this.ReadyAmmoScript(Items.NO_READY_ITEM);

    // need to check for targets requiring magical weapons

    var dmgIdx = Items.NO_READY_ITEM;
    var dmg = 0;
    var dmgSum = 0;
    var defIdx = Items.NO_READY_ITEM;
    var def = 0;
    var attIdx = Items.NO_READY_ITEM;
    var att = 0;
    var data = null;

    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {


        var err;
        err = this.myItems.CanReadyItem(this.myItems.PeekAtPos(pos), this);
        if ((err == Globals.miscErrorType.NoError) && ((data = this.itemData.GetItem(this.myItems.PeekAtPos(pos).itemID)) != null)) {
            if ((data.Location_Readied == Items.AmmoQuiver) && (data.Wpn_Type == Items.Ammo)) {
                if (data.Protection_Base + data.Protection_Bonus <= def) {
                    def = data.Protection_Base + data.Protection_Bonus;
                    defIdx = this.myItems.GetKeyAt(pos);
                }

                if (isLargeTarget)
                    dmgSum = (data.Dmg_Dice_Lg * data.Nbr_Dice_Lg) + data.Dmg_Bonus_Lg;
                else
                    dmgSum = (data.Dmg_Dice_Sm * data.Nbr_Dice_Sm) + data.Dmg_Bonus_Sm;

                // check for fighter multiple attacks (swords/daggers only?)
                dmgSum *= data.ROF_Per_Round;

                if (dmgSum >= dmg) {
                    dmg = dmgSum;
                    dmgIdx = this.myItems.GetKeyAt(pos);
                }

                if (data.Attack_Bonus >= att) {
                    att = data.Attack_Bonus;
                    attIdx = this.myItems.GetKeyAt(pos);
                }
            }
        }
        this.myItems.GetNext(pos);
    }

    // clear miscError that may have been set by CanReady()
    Globals.SetMiscError(Globals.miscErrorType.NoError);

    // for now, choose item with most damage
    if (dmgIdx != Items.NO_READY_ITEM) {
        this.ReadyAmmoScript(dmgIdx);
        return;
    }

    // else try for best attack bonus item
    if (attIdx != Items.NO_READY_ITEM) {
        this.ReadyAmmoScript(attIdx);
        return;
    }

    // else try for best defensive bonus item
    if (defIdx != Items.NO_READY_ITEM) {
        this.ReadyAmmoScript(defIdx);
        return;
    }
};

CHARACTER.prototype.ReadyItemByLocation = function (rdyLoc, index, specAbsOK) {
    if (index == Items.NO_READY_ITEM) {
        // unready all items of this type
        this.UnreadyItemByLocation(rdyLoc, specAbsOK);
        return true;
    }
    if (this.myItems.CanReadyKey(index, this) != Globals.miscErrorType.NoError) return false;
  //Not Implemented(0x4f9044, false);
  // We see if the character has any of the item's Allowed Baseclasses.
    this.myItems.Ready(index, this, rdyLoc);
    if (this.myItems.IsReady(index)) {
        var pData = this.itemData.GetItem(myItems.GetItem(index));
        if (pData != null) {
            var PreSpecAbCount = this.m_spellEffects.GetCount();
            if (!specAbsOK) {
                pData.specAbs.EnableAllFor(this);
            };

            var PostSpecAbCount = this.m_spellEffects.GetCount();
            var Diff = PostSpecAbCount - PreSpecAbCount;
            if (Diff > 0) {
                var spellID;
                pos = this.m_spellEffects.GetTailPosition();
                while ((pos != null) && (Diff > 0)) {
                    var SpecAbText = this.m_spellEffects.PeekAt(pos).AffectedAttr();
                    this.m_spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA.EFFECT_ITEMSPECAB;
                    this.m_spellEffects.GetAt(pos).SourceOfEffect(pData.ItemID(), true);
                    //this.m_spellEffects.GetPrev(pos);
                    pos = this.m_spellEffects.NextPos(pos);             //PORT NOTE: Added - no out params in Javascript, so have to advance pointer (pos)
                    Diff--;
                    var sa;
                    sa = SPECAB.ConvertRuntimeIfTextToSpecAb(SpecAbText);
                    spellID = pData.SpellID();
                    QueueUsedSpecAb(
                        sa,
                        SPELL_EFFECTS_DATA.EFFECT_ITEMSPECAB,
                        spellID);
                }

                var msg = this.FormatSpecAbMsg(SPECAB_MSG_READY);
                while (msg.GetLength() > 0) {
                    /**TODO: Find a way to report whatever this message is
                    FormatPausedText(pausedTextData, msg);
                    DisplayPausedText(pausedTextData, whiteColor, 0);
                    msg = FormatSpecAbMsg(SPECAB_MSG_READY);
                    */
                }
            }
            this.DisplayCurrSpellEffects("ReadyItemByType");
        }
        return true;
    }

    return false;
};

CHARACTER.prototype.UnreadyItemByLocation = function (rdyLoc, specAbsOK) {
    //int myitemidx = myItems.GetReady(loctype);
    var myitemidx = this.myItems.GetReadiedItem(rdyLoc, 0);

    var pData = this.itemData.GetItem(this.myItems.GetItem(myitemidx));
    if (pData != null) {
        if (!specAbsOK) {
            pData.specAbs.DisableAllFor(this);
        };
    };

    this.myItems.UnReady(myitemidx); 
};

CHARACTER.prototype.ReadyXXXScript = function (rdyLoc, scriptName, index) {

    if (this.ReadyItemByLocation(rdyLoc, index, true)) {
        if (index != Items.NO_READY_ITEM) {
            var actor;
            var pItem;
            var hookParameters = new HOOK_PARAMETERS();
            var scriptContext = new SCRIPT_CONTEXT();
            actor = this.GetContext();
            actor = RunTimeIF.SetCharContext(actor);
            pItem = this.itemData.GetItem(this.myItems.GetItem(index));
            scriptContext.SetCharacterContext(this);
            scriptContext.SetItemContext(pItem);
            scriptContext.SetItemContextKey(index);
            pItem.RunItemScripts(scriptName,
                SPECAB.ScriptCallback_RunAllScripts,
                null,
                "Character just readied an item");
            RunTimeIF.ClearCharContext();
        };
    };
};

CHARACTER.prototype.UnReadyXXXScript = function (scriptName, index) {
    var actor;
    var pItem;
    var hookParameters = new HOOK_PARAMETERS();
    var scriptContext = new SCRIPT_CONTEXT();
    actor = this.GetContext();
    actor = RunTimeIF.SetCharContext(actor);
    pItem = this.itemData.GetItem(this.myItems.GetItem(index));
    scriptContext.SetCharacterContext(this);
    scriptContext.SetItemContext(pItem);
    scriptContext.SetItemContextKey(index);
    pItem.RunItemScripts(scriptName,
        SPECAB.ScriptCallback_RunAllScripts,
        null,
        "Character just un-readied an item");
    //specAbs.RunScripts(scriptName, SPECAB.ScriptCallback_RunAllScripts, null, name,"");
    RunTimeIF.ClearCharContext();
};

CHARACTER.prototype.ReadyWeaponScript = function (index) {
    this.ReadyXXXScript(Items.WeaponHand, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyWeaponScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyShieldScript = function (index) {
    this.ReadyXXXScript(Items.ShieldHand, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyShieldScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyArmorScript = function (index) {
    this.ReadyXXXScript(Items.BodyArmor, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyArmorScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyGauntletsScript = function (index) {
    this.ReadyXXXScript(Items.Hands, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyGauntletsScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyHelmScript = function (index) {
    this.ReadyXXXScript(Items.Head, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyHelmScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyBeltScript = function (index) {
    this.ReadyXXXScript(Items.Waist, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyBeltScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyRobeScript = function (index) {
    this.ReadyXXXScript(Items.BodyRobe, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyRobeScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyCloakScript = function (index) {
    this.ReadyXXXScript(Items.Back, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyCloakScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyBootsScript = function (index) {
    this.ReadyXXXScript(Items.Feet, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyBootsScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyRingScript = function (index) {
    this.ReadyXXXScript(Items.Fingers, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyRingScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.ReadyAmmoScript = function (index) {
    this.ReadyXXXScript(Items.AmmoQuiver, SPECAB.ON_READY, index);
};

CHARACTER.prototype.UnReadyAmmoScript = function (index) {
    this.UnReadyXXXScript(SPECAB.ON_UNREADY, index);
};

CHARACTER.prototype.poolCharacterGold = function () {
    return this.money.ConvertToDefault(this.money.Total(), this.money.GetBaseType());
};

CHARACTER.prototype.enoughMoney = function (moneyCost, gemCost, jewelryCost, moneyType) {
    if (!gemCost) { gemCost = 0; }
    if (!jewelryCost) { jewelryCost = 0; }
    if (!moneyType) { moneyType = itemClassType.PlatinumType; }

    var enough = true;

    if (moneyCost > 0) {
        enough = false;

        if (this.GetParty().moneyPooled)
            enough = (this.GetParty().poolSack.HaveEnough(moneyType, moneyCost) ? true : false);

        if (!enough) // character's money
            enough = (this.money.HaveEnough(moneyType, moneyCost) ? true : false);
    }

    if ((enough) && (gemCost > 0))
        enough = (this.money.NumGems() >= gemCost);

    if ((enough) && (jewelryCost > 0))
        enough = (this.money.NumJewelry() >= jewelryCost);

    return enough; 
};

CHARACTER.prototype.addCharMoneyToItemList = function (list) {
    for (var i = 0; i < this.money.NumCoinTypes(); i++)
    {
        if (this.money.IsActive(globalData.moneyData.GetItemClass(i))) {
            var itype = moneyData.GetItemClass(i);
            list.Add(this.money.Coins[i], itype, money.Name(itype));
        }
    }

    list.Add(this.money.NumGems(), GemType, globalData.moneyData.GetName(GemType));
    list.Add(this.money.NumJewelry(), JewelryType, globalData.moneyData.GetName(JewelryType));

    /**TODO: Display-specific - remove?
    ItemsOnPage = list.GetCount();
    ItemsOnPage = Math.min(ItemsOnPage, Items_Per_Page); **/
};

CHARACTER.prototype.removeCharMoney = function (itype, qty) {
    if (!Items.itemIsMoney(itype)) {
        UAFUtil.WriteDebugString("Wrong item type passed to removeCharMoney()\n");
        return;
    }

    switch (itype) {
        case itemClassType.GemType:
            this.money.RemoveMultGems(qty);
            break;
        case itemClassType.JewelryType:
            this.money.RemoveMultJewelry(qty);
            break;
        default:
            this.money.Subtract(itype, qty);
            break;
    }
    this.encumbrance = this.determineEffectiveEncumbrance();
    this.determineMaxMovement();
};

CHARACTER.prototype.CanCastSpells = function () {
    if (GetType() == MONSTER_TYPE) return FALSE;

    // Search through the character's special abilities
    // to see if we can find a CanCastSpells script.
    {
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = SCRIPT_CONTEXT();
        var pCombatant;
        var pClass;
        pClass = classData.PeekClass(this.GetClass());
        scriptContext.SetCharacterContext(this);
        pCombatant = this.GetCombatant();
        if (pCombatant != null) {
            scriptContext.SetCombatantContext(this.GetCombatant());
        };
        scriptContext.SetClassContext(pClass);
        if (pCombatant != null) {
            if (!pCombatant.RunCombatantScripts(
                SPECAB.CAN_CAST_SPELLS,
                SPECAB.ScriptCallback_LookForChar,
                "N",
                "Test if combatant can cast spells"
            ).IsEmpty()) return false;
        };

        if (!this.RunCharacterScripts(
            SPECAB.CAN_CAST_SPELLS,
            SPECAB.ScriptCallback_LookForChar,
            "N",
            "Test if this combatant can cast spells"
        ).IsEmpty()) return false;

        if (pClass != null) {
            if (!pClass.RunClassScripts(SPECAB.CAN_CAST_SPELLS,
                SPECAB.ScriptCallback_LookForChar,
                "N",
                "Test if this combatant can cast spells")) {
                return false;
            };
        };
    };

    return true;
};

CHARACTER.prototype.SetBirthday = function (bday) {
    if (bday <= 0) bday = 1;
    if (bday > 365) bday = 365;
    this.birthday = bday;
};

CHARACTER.prototype.GetName = function () {
    return this.name;
};

CHARACTER.prototype.GetPermStr = function() {
    return this.strength;
}

CHARACTER.prototype.GetAdjStr = function(flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    if (this.strength_adjustment == UAFUtil.ByteFromHexString("0x7fffffff")) {
        this.strength_adjustment = 0;
        this.strength_adjustment = this.ApplySpellEffectAdjustments(flags, IF_KEYWORD_INDEX.CHAR_ADJUSTED_STR, this.strength_adjustment);
    };
    return this.strength + this.strength_adjustment;
}

CHARACTER.prototype.GetLimitedStr = function() {
    return this.LimitAb(this.GetAdjStr(), Globals.MIN_STRENGTH, Globals.MAX_STRENGTH);
}

CHARACTER.prototype.GetPermStrMod = function() {
    return this.strengthMod;
}

CHARACTER.prototype.GetAdjStrMod = function(flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    if (this.strengthMod_adjustment == UAFUtil.ByteFromHexString("0x7fffffff")) {
        this.strengthMod_adjustment = 0;
        this.strengthMod_adjustment = this.ApplySpellEffectAdjustments(flags, IF_KEYWORD_INDEX.CHAR_ADJUSTED_STRMOD, this.strengthMod_adjustment);
    };
    return this.strengthMod + this.strengthMod_adjustment;
}

CHARACTER.prototype.GetLimitedStrMod = function () {
    return this.LimitAb(this.GetAdjStrMod(), Globals.MIN_STRENGTHMOD, Globals.MAX_STRENGTHMOD);
}

CHARACTER.prototype.GetPermInt = function () {
    return this.intelligence;
}

CHARACTER.prototype.GetAdjInt = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    if (this.intelligence_adjustment == UAFUtil.ByteFromHexString("0x7fffffff")) {
        this.intelligence_adjustment = 0;
        this.intelligence_adjustment = this.ApplySpellEffectAdjustments(flags, IF_KEYWORD_INDEX.CHAR_ADJUSTED_INT, this.intelligence_adjustment);
    };
    return this.intelligence + this.intelligence_adjustment;
}

CHARACTER.prototype.GetLimitedInt = function() {
    return this.LimitAb(this.GetAdjInt(), Globals.MIN_INTELLIGENCE, Globals.MAX_INTELLIGENCE);
}

CHARACTER.prototype.GetPermWis = function() {
    return this.wisdom;
}

CHARACTER.prototype.GetAdjWis = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    if (this.wisdom_adjustment == UAFUtil.ByteFromHexString("0x7fffffff")) {
        this.wisdom_adjustment = 0;
        this.wisdom_adjustment = this.ApplySpellEffectAdjustments(flags, IF_KEYWORD_INDEX.CHAR_ADJUSTED_WIS, this.wisdom_adjustment);
    };
    return this.wisdom + this.wisdom_adjustment;
}

CHARACTER.prototype.GetLimitedWis = function () {
    return this.LimitAb(this.GetAdjWis(), Globals.MIN_WISDOM, Globals.MAX_WISDOM);
}

CHARACTER.prototype.GetPermDex = function () {
    return this.dexterity;
}

CHARACTER.prototype.GetAdjDex = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    if (this.dexterity_adjustment == UAFUtil.ByteFromHexString("0x7fffffff")) {
        this.dexterity_adjustment = 0;
        this.dexterity_adjustment = this.ApplySpellEffectAdjustments(flags, IF_KEYWORD_INDEX.CHAR_ADJUSTED_DEX, this.dexterity_adjustment);
    };
    return this.dexterity + this.dexterity_adjustment;
}

CHARACTER.prototype.GetLimitedDex = function () {
    return this.LimitAb(this.GetAdjDex(), Globals.MIN_DEXTERITY, Globals.MAX_DEXTERITY);
}

CHARACTER.prototype.GetPermCon = function () {
    return this.constitution;
}

CHARACTER.prototype.GetAdjCon = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    if (this.constitution_adjustment == UAFUtil.ByteFromHexString("0x7fffffff")) {
        this.constitution_adjustment = 0;
        this.constitution_adjustment = this.ApplySpellEffectAdjustments(flags, IF_KEYWORD_INDEX.CHAR_ADJUSTED_CON, this.constitution_adjustment);
    };
    return this.constitution + this.constitution_adjustment;
}

CHARACTER.prototype.GetLimitedCon = function() {
    return this.LimitAb(this.GetAdjCon(), Globals.MIN_CONSTITUTION, Globals.MAX_CONSTITUTION);
}

CHARACTER.prototype.GetPermCha = function () {
    return this.charisma;
}

CHARACTER.prototype.GetAdjCha = function(flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    if (this.charisma_adjustment == UAFUtil.ByteFromHexString("0x7fffffff")) {
        this.charisma_adjustment = 0;
        this.charisma_adjustment = this.ApplySpellEffectAdjustments(flags, IF_KEYWORD_INDEX.CHAR_ADJUSTED_CHA, this.charisma_adjustment);
    };
    return this.charisma + this.charisma_adjustment;
}

CHARACTER.prototype.GetLimitedCha = function() {
    return this.LimitAb(this.GetAdjCha(), Globals.MIN_CHARISMA, Globals.MAX_CHARISMA);
}

CHARACTER.prototype.GetAdjMaxEncumbrance = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_MAXENC;
    var val = this.GetMaxEncumbrance();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.max(1, val);
    return val;
}

CHARACTER.prototype.GetAdjGender = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_GENDER;
    var val = this.GetGender();
    
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    if ((val < 0) || (val >= GameRules.NUM_GENDER_TYPES))
        val = this.GetGender();
    return val;

};

CHARACTER.prototype.GetGender = function () {
    return this.gender;
}

CHARACTER.prototype.GetClass = function () {
    return this.classID;
};

CHARACTER.prototype.GetAlignment = function () {
    return this.alignment;
}

CHARACTER.prototype.GetStatus = function () {
    return this.status;
}

CHARACTER.prototype.GetUndeadType = function () {
    return this.undeadType == null || this.undeadType == "" ? "none" : undeadType;
}

CHARACTER.prototype.GetSize = function () {
    return this.creatureSize;
}

CHARACTER.prototype.GetAllowInCombat = function () {
    return this.allowInCombat;
}

CHARACTER.prototype.GetAdjAC = function (flags) {
    var key = IF_KEYWORD_INDEX.CHAR_AC;
    var val = this.GetBaseAC();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.min(MAX_AC, val);
    val = Math.max(MIN_AC, val);
    return val;
}

CHARACTER.prototype.GetAdjTHAC0 = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }

    var key = IF_KEYWORD_INDEX.CHAR_THAC0;

    var val = this.GetTHAC0();
    var itemID;

    var wpn = this.myItems.GetReadiedItem(Items.WeaponHand, 0);
    var itemID = this.myItems.GetItem(wpn);
    val -= this.GetAdjHitBonus(itemID, 0); // subtract strength bonus from base THAC0

    if (wpn != Items.NO_READY_ITEM) {
        var pData = itemData.GetItem(itemID);
        if (pData != null)
            val -= pData.Attack_Bonus; // subtract weapon attack bonus
    }

    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.min(MAX_THAC0, val);
    val = Math.max(MIN_THAC0, val);
    return val;
}

CHARACTER.prototype.GetTHAC0 = function () {
    return this.THAC0;
};

CHARACTER.prototype.GetBaseAC = function () {
    return this.m_AC;
};

CHARACTER.prototype.GetAge = function () {
    return this.age;
}

CHARACTER.prototype.GetMaxHitPoints = function () {
    return this.maxHitPoints;
}

CHARACTER.prototype.GetMaxAge = function () {
    return this.maxAge;
}

CHARACTER.prototype.GetHitPoints = function () {
    if ((this.m_pCombatant != null) && this.m_pCombatant.m_useTempHP) {
        return this.m_pCombatant.m_tempHP;
    };

    return this.hitPoints;
}

CHARACTER.prototype.GetAdjAge = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_AGE;
    var val = this.GetAge();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.max(1, val);
    return val;
}

CHARACTER.prototype.GetAdjMaxAge = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_MAXAGE;
    var val = this.GetMaxAge();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.max(1, val);
    return val;
}

CHARACTER.prototype.GetAdjHitPoints = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_HITPOINTS;
    var val = this.GetHitPoints();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.max(-10, val);
    val = Math.min(val, this.GetMaxHitPoints());
    return val;
}

CHARACTER.prototype.GetEncumbrance = function () {
    return this.encumbrance;
}

CHARACTER.prototype.SetEncumbrance = function (val) {
    val = Math.max(0, val);
    this.encumbrance = val;
}

CHARACTER.prototype.GetMaxEncumbrance = function () {
    return this.maxEncumbrance;
}

CHARACTER.prototype.SetMaxEncumbrance = function (val) {
    val = Math.max(0, val);
    this.maxEncumbrance = val;
}

CHARACTER.prototype.GetMaxMovement = function () {
    return this.maxMovement;
}

CHARACTER.prototype.GetAdjMaxMovement = function (flags, comment) {
    if (Globals.IsCombatActive()) {
        {
            var actor;
            var hookParameters = new HOOK_PARAMETERS();
            var scriptContext = new SCRIPT_CONTEXT();
            var minmax = [2];
            var pCombatant;
            minmax[0] = -1;
            minmax[1] = UAFUtil.ByteFromHexString("0x7fffffff");
            actor = this.GetContext();
            actor = RunTimeIF.SetCharContext(actor);
            hookParameters[5].Format("%i", GetMaxMovement());
            scriptContext.SetCharacterContext(this);
            // Removed 20101228 PRS....sometimes called outside of combat!!! Like taking treasure.
            //pCombatant = this->GetCombatant();
            //scriptContext.SetCombatantContext(pCombatant);
            this.RunCharacterScripts(SPECAB.GET_ADJ_MAX_MOVEMENT,
                SPECAB.ScriptCallback_MinMax,
                minmax,
                comment);

            pCombatant = m_pCombatant;
            if (pCombatant != null) {
                pCombatant.RunCombatantScripts(SPECAB.GET_ADJ_MAX_MOVEMENT,
                    SPECAB.ScriptCallback_MinMax,
                    minmax,
                    comment);
            };
            scriptContext.Clear();
            RunTimeIF.ClearCharContext();
            if ((minmax[0] == -1) && (minmax[1] == 0x7fffffff)) return GetMaxMovement();
            else return minmax[1];
        }
    }
    else {
        var key = IF_KEYWORD_INDEX.CHAR_MAXMOVE;
        var val = this.GetMaxMovement();
        val = this.ApplySpellEffectAdjustments(flags, key, val);
        val = Math.max(0, val);
        return val;
    }
}

CHARACTER.prototype.GetHitBonus = function () {
    return this.hitBonus;
}

CHARACTER.prototype.GetAdjHitBonus = function (weaponID, distance, flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }

    var key = IF_KEYWORD_INDEX.CHAR_HITBONUS;
    var val;

    var weaponType;

    weaponType = itemData.GetWpnType(weaponID);
    switch (weaponType) {
        case weaponClassType.NotWeapon:
        case weaponClassType.HandBlunt:
        case weaponClassType.HandCutting:
            val = this.GetHitBonus();
            break;
        case weaponClassType.HandThrow:
            val = (distance == 0) ? GetHitBonus() : 0;
            break;
        case weaponClassType.SlingNoAmmo:
        case weaponClassType.Bow:
        case weaponClassType.Crossbow:
        case weaponClassType.Throw:
            val = 0;
            break;
        case weaponClassType.Ammo:
            val = 0;
            break;
        case weaponClassType.SpellCaster:
        case weaponClassType.SpellLikeAbility:
            val = 0;
            break;
    };

    val = this.ApplySpellEffectAdjustments(flags, key, val);
    return val;

};

CHARACTER.prototype.determineEffectiveEncumbrance = function () {
    if (this.GetType() == MONSTER_TYPE) return 0;

    var total = this.money.GetTotalWeight();

    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {
        {
            total += getItemEncumbrance(myItems.PeekAtPos(pos).itemID, this.myItems.PeekAtPos(pos).qty);
        }
        pos = this.myItems.NextPos(pos);
    }

    return total;
}

CHARACTER.prototype.GetType = function () {
    return (this.type & (~IN_PARTY_TYPE));
}


CHARACTER.prototype.DisplayCurrSpellEffects = function (SrcFunc) {
    if (DUMP_CHAR_SPELL_ATTS) {

        if (this.m_spellEffects.GetCount() == 0)
            return;

        var src = "";

        if (this.GetName().IsEmpty())
            return; // sometimes gets called for empty CHARACTER slots in PARTY struct

        //Sleep(10);  // PORT NOTE: Yeah, no.

        Globals.WriteDebugString("*** BEGIN SPELL_EFFECT records for \'" + this.GetName() + "\' from " + SrcFunc + "\n");

        var pos = this.m_spellEffects.GetHeadPosition();
        while (pos != null) {
            var data = this.m_spellEffects.GetNext(pos);
            pos = this.spellEffects.NextPos(pos);
            if (data.flags & SPELL_EFFECTS_DATA.EFFECT_SPELL)
            src = "SPELL";
        else if (data.flags & SPELL_EFFECTS_DATA.EFFECT_CHARSPECAB)
            src = "CHARSPECAB";
        else if (data.flags & SPELL_EFFECTS_DATA.EFFECT_SPELLSPECAB)
            src = "SPELLSPECAB";
        else if (data.flags & SPELL_EFFECTS_DATA.EFFECT_ITEMSPECAB)
            src = "ITEMSPECAB";
        else
            src = "!!INVALID!!";


            Globals.WriteDebugString("   Attrib \'" + data.affectedAttr + "\', Parent " + data.parent + ", Src " + src + ", Flags " + data.flags + "\n");
        }

        Globals.WriteDebugString("*** END SPELL_EFFECT records for \'" + GetName() + "\'\n", );
    }
};



CHARACTER.prototype.CanMemorizeSpells = function (circumstance) { throw "todo"; };
CHARACTER.prototype.GetBestMemorizedHealingSpell = function (pSpellID) { throw "todo"; };
CHARACTER.prototype.GetHealingPointsNeeded = function () { throw "todo"; };
CHARACTER.prototype.giveCharacterDamage = function (eventSave, attackTHAC0, dmgDice, dmgDiceQty, dmgBonus, spellSave, saveBonus, pAttacker) { throw "todo"; };
CHARACTER.prototype.giveCharacterDamage = function (damage) { throw "todo"; };
CHARACTER.prototype.TakeDamage = function (dmg, IsNonLethal, invokeOptions, canFinishCasting, pDeathIndex) { throw "todo"; };
CHARACTER.prototype.rollStats = function () { throw "todo"; };
CHARACTER.prototype.GetLevelCap = function (pBaseclass) { throw "todo"; };
CHARACTER.prototype.UpdateStats = function (IsNewChar) { throw "todo"; };
CHARACTER.prototype.UpdateBaseclassLevels = function () { throw "todo"; };
CHARACTER.prototype.UpdateLevelBasedStats = function () { throw "todo"; };
CHARACTER.prototype.UpdateSkillBasedStats = function () { throw "todo"; };
CHARACTER.prototype.UpdateSpellAbility = function () { throw "todo"; };
CHARACTER.prototype.UpdateSpellAbilityForBaseclass = function(pBaseclassStats) { throw "todo"; }
CHARACTER.prototype.CanBeModified = function() { throw "todo"; }
CHARACTER.prototype.generateNewCharacter = function(StartExperience, StartExpType) { throw "todo"; }
CHARACTER.prototype.getNewCharLevel = function(pTrainableBaseclasses, maxLevelGain) { throw "todo"; }
CHARACTER.prototype.checkNewCharClassScores = function() { throw "todo"; }
CHARACTER.prototype.checkNewCharRaceScores = function(ApplyModifiers) { throw "todo"; }
CHARACTER.prototype.getNewCharStartingMoney = function() { throw "todo"; }
CHARACTER.prototype.getNewCharStartingEquip = function() { throw "todo"; }
CHARACTER.prototype.getAllowedAlignments = function() { throw "todo"; }
CHARACTER.prototype.SetThiefSkills = function() { throw "todo"; }
CHARACTER.prototype.GetThiefBackstabDamageMultiplier = function() { throw "todo"; }
CHARACTER.prototype.GetThiefBackstabAttackAdjustment = function() { throw "todo"; }
CHARACTER.prototype.GetThiefSkillArmorAdustments = function() { throw "todo"; }
CHARACTER.prototype.UpdateSkill = function(abilityModifier, abilityName) { throw "todo"; }
CHARACTER.prototype.TrainCharacter = function(pTrainableBaseclasses) { throw "todo"; }
CHARACTER.prototype.CanChangeToClass = function(pFromClass, pToClass) { throw "todo"; }
CHARACTER.prototype.CreateChangeClassList = function(pClassArray) { throw "todo"; }
CHARACTER.prototype.HumanChangeClass = function(classID) { throw "todo"; }
CHARACTER.prototype.CanUseBaseclass = function(pBaseclassStats) { throw "todo"; }
CHARACTER.prototype.CanUseBaseclass = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.IsDualClass = function() { throw "todo"; }
CHARACTER.prototype.IsUsingBaseclass = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.IsAlive = function() { throw "todo"; }
CHARACTER.prototype.GetContext = function(pActor, baseclassID) { throw "todo"; }
CHARACTER.prototype.GetContext = function(pActor, schoolID) { throw "todo"; }
CHARACTER.prototype.GetContext = function(pActor, pSpell) { throw "todo"; }
CHARACTER.prototype.GetContext = function(pActor) { throw "todo"; }
CHARACTER.prototype.EvalDiceAsClass = function(dice, baseclassID, pRollerLevel) { throw "todo"; }
CHARACTER.prototype.EvalDiceAsClass = function(dice, schoolID, spellLevel, pRollerLevel) { throw "todo"; }
CHARACTER.prototype.EvalDiceAsClass = function(dice, pSpell, pRollerLevel) { throw "todo"; }
CHARACTER.prototype.WhichBaseclassCastThisSpell = function(pSpell) { throw "todo"; }
CHARACTER.prototype.SetName = function(val) { throw "todo"; }
CHARACTER.prototype.DetermineMaxCureDisease = function() { throw "todo"; }
CHARACTER.prototype.GetMaxCureDisease = function() { throw "todo"; }
CHARACTER.prototype.SetMaxCureDisease = function(val) { throw "todo"; }
CHARACTER.prototype.GetCureDisease = function() { throw "todo"; }
CHARACTER.prototype.SetCureDisease = function(val) { throw "todo"; }
CHARACTER.prototype.IncCureDisease = function() { throw "todo"; }
CHARACTER.prototype.HaveCureDisease = function() { throw "todo"; }
CHARACTER.prototype.GetMorale = function() { throw "todo"; }
CHARACTER.prototype.GetAdjMorale = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.SetMorale = function(val) { throw "todo"; }
CHARACTER.prototype.DetermineUnarmedHitDice = function() { throw "todo"; }
CHARACTER.prototype.DetermineCharHitDice = function() { throw "todo"; }
CHARACTER.prototype.DetermineHitDiceBonus = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.GetNbrHD = function() { throw "todo"; }
CHARACTER.prototype.SetNbrHD = function(val) { throw "todo"; }
CHARACTER.prototype.determineNbrAttacks = function() { throw "todo"; }
CHARACTER.prototype.GetNbrAttacks = function() { throw "todo"; }
CHARACTER.prototype.SetNbrAttacks = function(val) { throw "todo"; }
CHARACTER.prototype.GetCurrentLevel = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.CurrentBaseclassLevel = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.SetCurrentLevel = function(baseclassID, lvl) { throw "todo"; }
CHARACTER.prototype.SetHitBonus = function(val) { throw "todo"; }
CHARACTER.prototype.GetDmgBonus = function() { throw "todo"; }
CHARACTER.prototype.GetAdjDmgBonus = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.SetDmgBonus = function(val) { throw "todo"; }
CHARACTER.prototype.ComputeCharSavingThrows = function() { throw "todo"; }
CHARACTER.prototype.DidSaveVersus = function(type, bonus, pSpell, pAttackerr) { throw "todo"; }
CHARACTER.prototype.GetMagicResistance = function() { throw "todo"; }
CHARACTER.prototype.GetAdjMagicResistance = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.SetMagicResistance = function(val) { throw "todo"; }
CHARACTER.prototype.GetHighestLevelBaseclass = function() { throw "todo"; }
CHARACTER.prototype.GetAllowInCombat = function() { throw "todo"; }
CHARACTER.prototype.GetAdjClass = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetAdjAlignment = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetAdjStatus = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetAdjSize = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetIsFriendly = function() { throw "todo"; }
CHARACTER.prototype.SetGender = function(val) { throw "todo"; }
CHARACTER.prototype.SetGender = function(gender) { throw "todo"; }
CHARACTER.prototype.SetClass = function(classID) { throw "todo"; }
CHARACTER.prototype.SetAlignment = function(val) { throw "todo"; }
CHARACTER.prototype.SetStatus = function(val) { throw "todo"; }
CHARACTER.prototype.SetUndead = function(uType) { throw "todo"; }
CHARACTER.prototype.SetSize = function(val) { throw "todo"; }
CHARACTER.prototype.SetAllowInCombat = function(allow) { throw "todo"; }
CHARACTER.prototype.IsAlignmentEvil = function() { throw "todo"; }
CHARACTER.prototype.IsAlignmentGood = function() { throw "todo"; }
CHARACTER.prototype.determineEncumbrance = function() { throw "todo"; }
CHARACTER.prototype.determineNormalEncumbrance = function() { throw "todo"; }
CHARACTER.prototype.determineMaxEncumbrance = function() { throw "todo"; }
CHARACTER.prototype.GetAdjMaxEncumbrance = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetPerm = function () { };
CHARACTER.prototype.GetAdj = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetLimitedStr = function() { throw "todo"; }
CHARACTER.prototype.GetLimitedStrMod = function() { throw "todo"; }
CHARACTER.prototype.GetLimited= function() { throw "todo"; }
CHARACTER.prototype.SetPerm = function(val) { throw "todo"; }
CHARACTER.prototype.GetAbilityScore = function(abilityName) { throw "todo"; }
CHARACTER.prototype.CreateAllowedSchoolList = function(pSchoolList) { throw "todo"; }
CHARACTER.prototype.MatchSpellBaseclass = function(pSpell) { throw "todo"; }
CHARACTER.prototype.GetPrimeSpellCastingScore = function(pBaseclass, schoolID) { throw "todo"; }
CHARACTER.prototype.PrimeSpellCastingScore = function(spellSchool) { throw "todo"; }
CHARACTER.prototype.HaveSpell = function(spellID, checkmemorized) { throw "todo"; }
CHARACTER.prototype.GetSpellBookIndex = function(spellID, checkMemorized) { throw "todo"; }
CHARACTER.prototype.GetAbilityLimits = function(abilityID) { throw "todo"; }
CHARACTER.prototype.IsPartyMember = function() { throw "todo"; }
CHARACTER.prototype.SetPartyMember = function(flag) {if (flag == undefined) {flag = true;}};
CHARACTER.prototype.SetType = function(flag) { throw "todo"; }
CHARACTER.prototype.GetAutomatic = function() { throw "todo"; }
CHARACTER.prototype.SetAutomatic = function(flag) { throw "todo"; }
CHARACTER.prototype.GetAdjAutomatic = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetAllowPlayerControl = function() { throw "todo"; }
CHARACTER.prototype.SetAllowPlayerControl = function(flag) { throw "todo"; }
CHARACTER.prototype.GetAdjAllowPlayerControl = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetDetectingInvisible = function() { throw "todo"; }
CHARACTER.prototype.SetDetectingInvisible = function(flag) { throw "todo"; }
CHARACTER.prototype.GetAdjDetectingInvisible = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetDetectingTraps = function() { throw "todo"; }
CHARACTER.prototype.SetDetectingTraps = function(flag) { throw "todo"; }
CHARACTER.prototype.GetAdjDetectingTraps = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.RaceID = function() { throw "todo"; }
CHARACTER.prototype.RaceID = function(raceID) { throw "todo"; }
CHARACTER.prototype.PeekRaceData = function() { throw "todo"; }
CHARACTER.prototype.getCharTHAC0 = function() { throw "todo"; }
CHARACTER.prototype.SetTHAC0 = function(val) { throw "todo"; }
CHARACTER.prototype.GetEffectiveAC = function() { throw "todo"; }
CHARACTER.prototype.SetCharBaseAC = function() { throw "todo"; }
CHARACTER.prototype.SetAC = function(val) { throw "todo"; }
CHARACTER.prototype.SetCharAC = function() { throw "todo"; }
CHARACTER.prototype.DetermineCharMaxHitPoints = function() { throw "todo"; }
CHARACTER.prototype.DetermineNewCharMaxHitPoints = function(randomSeed) { throw "todo"; }
CHARACTER.prototype.SetHitPoints = function(val) { throw "todo"; }
CHARACTER.prototype.SetHitPoints = function(val, int, canFinishCasting) { throw "todo"; }
CHARACTER.prototype.RestoreMaxHitPoints = function() { throw "todo"; }
CHARACTER.prototype.SetMaxHitPoints = function(val) { throw "todo"; }
CHARACTER.prototype.RestoreToBestCondition = function() { throw "todo"; }
CHARACTER.prototype.SetAge = function(val) { throw "todo"; }
CHARACTER.prototype.GetIconIndex = function() { throw "todo"; }
CHARACTER.prototype.SetIconIndex = function(val) { throw "todo"; }
CHARACTER.prototype.GetBirthday = function() { throw "todo"; }
CHARACTER.prototype.DetermineCharStartAge = function() { throw "todo"; }
CHARACTER.prototype.DetermineCharMaxAge = function() { throw "todo"; }
CHARACTER.prototype.SetMaxAge = function(val) { throw "todo"; }
CHARACTER.prototype.determineMaxMovement = function() { throw "todo"; }
CHARACTER.prototype.GetAdjMaxMovement_GPDL = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.SetMaxMovement = function(val) { throw "todo"; }
CHARACTER.prototype.IsReadyToTrain = function(pBaseclassStats) { throw "todo"; }
CHARACTER.prototype.IsReadyToTrain = function() { throw "todo"; }
CHARACTER.prototype.GetReadyToTrain = function() { throw "todo"; }
CHARACTER.prototype.GetAdjReadyToTrain = function(flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.SetReadyToTrain = function(enable) { throw "todo"; }
CHARACTER.prototype.IsAbleToTrade = function() { throw "todo"; }
CHARACTER.prototype.SetAbleToTrade = function(enable) { throw "todo"; }
CHARACTER.prototype.getCharExpWorth = function() { throw "todo"; }
CHARACTER.prototype.giveCharacterExperience = function(exp, UseLimits) {if (UseLimits == undefined) {UseLimits = true;}};
CHARACTER.prototype.GetCurrExp = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.SetCurrExp = function(baseclassID, exp) { throw "todo"; }
CHARACTER.prototype.ClearLevels = function() { throw "todo"; }
CHARACTER.prototype.SetLevel = function(baseclassID, lvl) { throw "todo"; }
CHARACTER.prototype.SetLevel = function(lvl) { throw "todo"; }
CHARACTER.prototype.GetCurrLevel = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.GetAllowedLevel = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.IncCurrExp = function(baseclassID, exp) { throw "todo"; }
CHARACTER.prototype.GetAdjBaseclassExp = function(baseclassID, flags) {if (!flags) {flags = DEFAULT_SPELL_EFFECT_FLAGS;}};
CHARACTER.prototype.GetBaseclassExp = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.GetAdjSpecAb = function(sa, pSource, pSpellName) { throw "todo"; }
CHARACTER.prototype.SetSpecAb = function(sa, enable, flags) { throw "todo"; }
CHARACTER.prototype.QueueUsedSpecAb = function(sa, src, SpellID) { throw "todo"; }
CHARACTER.prototype.ClearQueuedSpecAb = function() { throw "todo"; }
CHARACTER.prototype.AdjustEffectData = function(key, val) { throw "todo"; }
CHARACTER.prototype.IsMammal = function() { throw "todo"; }
CHARACTER.prototype.IsAnimal = function() { throw "todo"; }
CHARACTER.prototype.IsSnake = function() { throw "todo"; }
CHARACTER.prototype.IsGiant = function() { throw "todo"; }
CHARACTER.prototype.IsAlwaysLarge = function() { throw "todo"; }
CHARACTER.prototype.HasDwarfACPenalty = function() { throw "todo"; }
CHARACTER.prototype.HasGnomeACPenalty = function() { throw "todo"; }
CHARACTER.prototype.HasDwarfTHAC0Penalty = function() { throw "todo"; }
CHARACTER.prototype.HasGnomeTHAC0Penalty = function() { throw "todo"; }
CHARACTER.prototype.HasRangerDmgPenalty = function() { throw "todo"; }
CHARACTER.prototype.HasPoisonImmunity = function() { throw "todo"; }
CHARACTER.prototype.HasDeathImmunity = function() { throw "todo"; }
CHARACTER.prototype.HasConfusionImmunity = function() { throw "todo"; }
CHARACTER.prototype.HasVorpalImmunity = function() { throw "todo"; }
CHARACTER.prototype.CanBeHeldOrCharmed = function() { throw "todo"; }
CHARACTER.prototype.AffectedByDispelEvil = function() { throw "todo"; }
CHARACTER.prototype.CharacterID = function(id) { throw "todo"; }
CHARACTER.prototype.ProcessLingeringSpellEffects = function() { throw "todo"; }
//CHARACTER.prototype.RunCharacterScripts ( scriptName, enum CBRESULT(*fnc)(enum CBFUNC func, CString *scriptResult, void *pkt), void *pkt, LPCSTR comment) { throw "todo"; }
CHARACTER.prototype.GetSpellCount = function() { throw "todo"; }
CHARACTER.prototype.ClearSpellbook = function() { throw "todo"; }
CHARACTER.prototype.FetchCharacterSpell = function(spellID, pCharSp) { throw "todo"; }
CHARACTER.prototype.LocateCharacterSpell = function(spellID) { throw "todo"; }
CHARACTER.prototype.PeekCharacterSpell = function(index) { throw "todo"; }
CHARACTER.prototype.GetCharacterSpell = function(index) { throw "todo"; }
CHARACTER.prototype.DecMemorized = function(spellID, count) { throw "todo"; }
CHARACTER.prototype.SpellsKnown = function() { throw "todo"; }
CHARACTER.prototype.IsMemorized = function(spellID) { throw "todo"; }
CHARACTER.prototype.IncAllMemorizedTime = function(minuteInc, all) { throw "todo"; }
CHARACTER.prototype.KnowSpell = function(baseclassID, spellSchoolID, spellID, level, known) { throw "todo"; }
CHARACTER.prototype.KnowSpellyyy = function(pSpell, know) { throw "todo"; }
CHARACTER.prototype.CopySpellbook = function(b) { throw "todo"; }
CHARACTER.prototype.UnselectIfNotMemorized = function() { throw "todo"; }
CHARACTER.prototype.CanKnowSpell = function(schoolID, spellLevel) { throw "todo"; }
CHARACTER.prototype.HasKnowableSpell = function(uniqueName) { throw "todo"; }
CHARACTER.prototype.AddKnowableSpell = function(uniqueName) { throw "todo"; }
CHARACTER.prototype.DelKnowableSpell = function(uniqueName) { throw "todo"; }
CHARACTER.prototype.ClrKnowableSpell = function() { throw "todo"; }
CHARACTER.prototype.ComputeAIBaseclass = function() { throw "todo"; }
// 	DEFINE_mCARRAY_ACCESS_FUNCTIONS (SkillAdj, SKILL_ID, skillID, skillAdjustments, SKILL_ADJ, CHARACTER) void DeleteSkillAdj(indx) { throw "todo"; }
CHARACTER.prototype.LocateSkillAdj = function(skillName, adjName) { throw "todo"; }
CHARACTER.prototype.ApplyTempSkillAdjustments = function(SC) { throw "todo"; }
CHARACTER.prototype.InsertSkillAdj = function(skillName, adjName, type, value) { throw "todo"; }
CHARACTER.prototype.GetAdjSkillValue = function(skillName, minimize, includeTempAdj) { throw "todo"; }
CHARACTER.prototype.GetBaseclassLevel = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.GetBaseclassPrevLevel = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.GetBaseclassPreDrainLevel = function(baseclassID) { throw "todo"; }
CHARACTER.prototype.SetBaseclassLevel = function(baseclassID, level) { throw "todo"; }
CHARACTER.prototype.ComputeCharacterViewValues = function() { throw "todo"; }
//virtual CHARACTER.prototype.ComputeCombatViewValues () { throw "todo"; }
//DEFINE_mCARRAY_ACCESS_FUNCTIONS (SpellAdj, SCHOOL_ID, schoolID, spellAdjustments, SPELL_ADJ, CHARACTER) CString SpellAdjustment(schoolID

CHARACTER.prototype.ApplySpellEffectAdjustments = function(flags, key, val) {

    return val;
    /**TODO**
    var pos = this.GetFirstEffectAttributeMod(key);
    while (pos != null) {
        pData = this.m_spellEffects.PeekAt(pos);
        var src = val;
        {
            // Temporary effect (otherwise a permanent effect applied periodically)
            val = pData.ApplyChange(src); // return accumulated delta
        };
        pos = GetNextEffectAttributeMod(key, pos);
    }*/
}

CHARACTER.prototype.GetFirstEffectAttributeMod = function(attr) {
    var pos = this.m_spellEffects.GetHeadPosition();
    while (pos != null) {
        if (this.m_spellEffects.PeekAt(pos).affectedAttr_IFKEY == attr)
            return pos;
        pos = this.m_spellEffects.PeekNext(pos);
        pos = this.m_spellEffects.NextPos(pos);   //PORT NOTE: Added - no out params in Javascript, so have to advance pointer (pos)
    }
    return null;
}
