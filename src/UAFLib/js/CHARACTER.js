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
    this.preSpellNamesKey = 0;
    this.name = "";
    this.undeadType = "";
    this.spellAbility = new SPELL_ABILITY();
    this.money = new MONEY_SACK();
    this.icon = new PIC_DATA();
    this.smallPic = new PIC_DATA();
    this.monsterID = "";
    this.characterID = new CHARACTER_ID();
    this.specAbs = new SPECIAL_ABILITIES();
    this.blockageData = new BLOCKAGE_STATUS();
    this.m_spellbook = new spellBookType();
    this.myItems = new ITEM_LIST();
    this.char_asl = new A_ASLENTRY_L();
    this.targets = new SPELL_TARGETING_DATA();
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
    this.monsterID = "";
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
    this.spellAdjustments = [];
    this.skillAdjustments = [];
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
        if ((temp & UAFUtil.ByteFromHexString("0x80000000")) != 0) {
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
        n = this.m_spellbook.GetCount();
        for (i = 0; i < n; i++) {
            var pCharSp;
            pCharSp = this.m_spellbook.PeekCharacterSpell(i);
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

    var wpnHand = this.myItems.GetReadiedItem(Items.WeaponHand, 0);

    if (wpnHand != NO_READY_ITEM) {
        var itemID = new ITEM_ID();
        itemID = this.myItems.GetItem(wpnHand);

        data = itemData.GetItemFromID(itemID);

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
                if (ammoHand != NO_READY_ITEM) {
                    var itemName;
                    itemName = this.myItems.GetItem(ammoHand);
                    data = itemData.GetItemFromID(itemID);
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

    if (index != NO_READY_ITEM) {
        var itemData = itemData.GetItemFromID(itemID);
        if ((Items.isMagical(myItems.GetItem(index))) && (this.myItems.GetId(index)))
            armor = itemData.GetItemFromIDIdName(myItems.GetItem(index));
        else
            armor = itemData.GetItemFromIDCommonName(myItems.GetItem(index));
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
                "race=" + raceData.GetRaceName(this.race) + ";" +
                "gender=" + (this.GetAdjGender() == genderType.Male ? "male" : "female") + ";" +
                "class=" + classData.PeekClass(this.GetClass()).m_name + ";" +
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
        var itemData = itemData.GetItemFromID(itemID);
        var itemData = itemData.GetItemFromID(itemID);
        temp.Format("{" + itemData.GetItemFromIDUniqueName(itemID) + "[" + id + "]}");
        result += temp;
        this.myItems.GetNext(pos);
    }
    return result;
};

CHARACTER.prototype.addCharacterItem = function (itemID, qty, numCharges, id, cost) {
    if ((itemData.getItemEncumbrance(itemID, qty) + this.GetEncumbrance()) > this.GetAdjMaxEncumbrance()) {
        Globals.SetMiscError(miscErrorType.TooMuchWeight);
        return false;
    }

    if (!(this.myItems.addItem5(itemID, qty, numCharges, id, cost) >=0)) {  // PORT NOTE:  Had to add >=0 since 0 is an ok index, but is also == false
        Globals.WriteDebugString("Failed to addItem to character\n");
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

    var data = itemData.GetItemFromID(itemID);
    if (data == null)
        return;

    var cost = this.ApplyCostFactor(cf, data.Cost);

    if (!this.enoughMoney(cost, 0, 0, globalData.moneyData.GetDefaultType())) {
        Globals.SetMiscError(Globals.miscErrorType.NotEnoughMoney);
        return;
    }

    if ((itemData.getItemEncumbrance(itemID, 1) + this.GetEncumbrance()) > this.GetAdjMaxEncumbrance())
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
    pItem = itemData.GetItemFromID(itemID);
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
    if (index != NO_READY_ITEM) {
        {
            var actor;
            var pItem;
            var hookParameters = new HOOK_PARAMETERS();
            var scriptContext = new SCRIPT_CONTEXT();
            var answer;
            actor = this.GetContextActor();
            actor = RunTimeIF.SetCharContext(actor);
            pItem = itemData.GetItemFromID(this.myItems.GetItem(index));
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
    var data = itemData.GetItemFromID(this.myItems.GetItem(item).itemID);   // PORT NOTE:   Changed due to item indexing scheme change
    if (data == null) return false;
    rdyLoc = data.Location_Readied;

    if (this.myItems.IsReady(item)) {
        this.myItems.UnReady(item);
        success = !this.myItems.IsReady(item);
        if (success) {
            if (itemReadiedLocation.WeaponHand.EqualsDWORD(rdyLoc)) this.UnReadyWeaponScript(item);
            else if (itemReadiedLocation.ShieldHand.EqualsDWORD(rdyLoc)) this.UnReadyShieldScript(item);
            else if (itemReadiedLocation.BodyArmor.EqualsDWORD(rdyLoc)) this.UnReadyArmorScript(item);
            else if (itemReadiedLocation.Hands.EqualsDWORD(rdyLoc)) this.UnReadyGauntletsScript(item);
            else if (itemReadiedLocation.Head.EqualsDWORD(rdyLoc)) this.UnReadyHelmScript(item);
            else if (itemReadiedLocation.Waist.EqualsDWORD(rdyLoc)) this.UnReadyBeltScript(item);
            else if (itemReadiedLocation.BodyRobe.EqualsDWORD(rdyLoc)) this.UnReadyRobeScript(item);
            else if (itemReadiedLocation.Back.EqualsDWORD(rdyLoc)) this.UnReadyCloakScript(item);
            else if (itemReadiedLocation.Feet.EqualsDWORD(rdyLoc)) this.UnReadyBootsScript(item);
            else if (itemReadiedLocation.Fingers.EqualsDWORD(rdyLoc)) this.UnReadyRingScript(item);
            // should separate this into ring1 and ring2?
            else if (itemReadiedLocation.AmmoQuiver.EqualsDWORD(rdyLoc)) this.UnReadyAmmoScript(item);
            else this.UnReadyXXXScript(SPECAB.ON_UNREADY, item);
        };
    }
    else {
        if (itemReadiedLocation.WeaponHand.EqualsDWORD(rdyLoc)) this.ReadyWeaponScript(item);
        else if (itemReadiedLocation.ShieldHand.EqualsDWORD(rdyLoc)) this.ReadyShieldScript(item);
        else if (itemReadiedLocation.BodyArmor.EqualsDWORD(rdyLoc)) this.ReadyArmorScript(item);
        else if (itemReadiedLocation.Hands.EqualsDWORD(rdyLoc)) this.ReadyGauntletsScript(item);
        else if (itemReadiedLocation.Head.EqualsDWORD(rdyLoc)) this.ReadyHelmScript(item);
        else if (itemReadiedLocation.Waist.EqualsDWORD(rdyLoc)) this.ReadyBeltScript(item);
        else if (itemReadiedLocation.BodyRobe.EqualsDWORD(rdyLoc)) this.ReadyRobeScript(item);
        else if (itemReadiedLocation.Back.EqualsDWORD(rdyLoc)) this.ReadyCloakScript(item);
        else if (itemReadiedLocation.Feet.EqualsDWORD(rdyLoc)) this.ReadyBootsScript(item);
        else if (itemReadiedLocation.Fingers.EqualsDWORD(rdyLoc)) this.ReadyRingScript(item);
        // should separate this into ring1 and ring2?
        else if (itemReadiedLocation.AmmoQuiver.EqualsDWORD(rdyLoc)) this.ReadyAmmoScript(item);
        else this.ReadyXXXScript(data.Location_Readied, SPECAB.ON_READY, item);
        success = this.myItems.IsReady(item);
    }

    if (success)
        this.SetCharAC();

    return success;
};

CHARACTER.prototype.ReadyBestWpn = function (dist, isLargeTarget) {
    this.ReadyWeaponScript(NO_READY_ITEM);

    // need to check for usable items
    // need to check for targets requiring magical weapons

    var dmgIdx = NO_READY_ITEM;
    var dmg = 0;
    var dmgSum = 0;
    var defIdx = NO_READY_ITEM;
    var def = 0;
    var attIdx = NO_READY_ITEM;
    var att = 0;
    var data;

    // determine which avail item has best damage, 
    // attack, and protection values for given distance
    // to target
    //
    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {
        if ((data = itemData.GetItemFromID(this.myItems.PeekAtPos(pos).itemID)) != null) {
            if (data.Location_Readied == Items.WeaponHand) {
                var err;
                err = this.myItems.CanReadyItem(this.myItems.PeekAtPos(pos), this);
                if (err == miscErrorType.NoError) {
                    if (itemData.WpnCanAttackAtRange(this.myItems.PeekAtPos(pos).itemID, dist)) {
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
        pos = this.myItems.NextPos(pos);                // PORT NOTE:  Changed to advance the pointer since no pass-by-reference in JS
    };

    // clear miscError that may have been set by CanReady()
    Globals.SetMiscError(miscErrorType.NoError);

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
            nonwpn_sum = (this.unarmedDieS * this.unarmedNbrDieS) + this.unarmedBonus;
        else
            nonwpn_sum = (this.unarmedDieL * this.unarmedNbrDieL) + this.unarmedBonus;

        nonwpn_sum *= this.GetNbrAttacks();

        // better off using natural attacks
        if (wpn_sum < nonwpn_sum) return;
    }

    // for now, choose item with most damage first
    var IdxToUse = dmgIdx;
    if (IdxToUse == NO_READY_ITEM)
        IdxToUse = attIdx;
    if (IdxToUse == NO_READY_ITEM)
        IdxToUse = defIdx;
    if (IdxToUse == NO_READY_ITEM)
        return;

    data = itemData.GetItemFromID(this.myItems.GetItem(IdxToUse));
    if (data != null) {
        if (data.Hands_to_Use > 1)
            myItems.UnReady(this.myItems.GetReadiedItem(Items.ShieldHand, 0));
    }

    this.ReadyWeaponScript(IdxToUse);
    if (itemData.WpnUsesAmmo(this.myItems.GetItem(IdxToUse)))
        this.ReadyBestAmmo(isLargeTarget);
};

CHARACTER.prototype.ReadyBestShield = function () {
    // if 2-handed weapon being used, can't ready a shield
    if ((this.myItems.GetReadiedItem(Items.ShieldHand, 0) != NO_READY_ITEM)
        && (this.myItems.GetReadiedItem(Items.ShieldHand, 0) == this.myItems.GetReadiedItem(Items.WeaponHand, 0)))
        return;

    this.ReadyShieldScript(NO_READY_ITEM);

    var defIdx = NO_READY_ITEM;
    var def = 0;
    var data;

    // determine which avail item has best
    // protection value
    //
    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {
        if ((data = itemData.GetItemFromID(this.myItems.PeekAtPos(pos).itemID)) != null) {
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
    Globals.SetMiscError(miscErrorType.NoError);

    // else try for best defensive bonus item
    if (defIdx != NO_READY_ITEM) {
        this.ReadyShieldScript(defIdx);
        return;
    }
};

CHARACTER.prototype.ReadyBestArmor = function () {
    this.ReadyArmorScript(NO_READY_ITEM);  //Discard current Armor

    var defIdx = NO_READY_ITEM;
    var def = 0;
    var data;

    // determine which avail item has best
    // protection value
    //
    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {
        if ((data = itemData.GetItemFromID(this.myItems.PeekAtPos(pos).itemID)) != null) {

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
    Globals.SetMiscError(miscErrorType.NoError);

    // else try for best defensive bonus item
    if (defIdx != NO_READY_ITEM) {
        this.ReadyArmorScript(defIdx);
        return;
    }
};

CHARACTER.prototype.ReadyBestAmmo = function (isLargeTarget) {
    this.ReadyAmmoScript(NO_READY_ITEM);

    // need to check for targets requiring magical weapons

    var dmgIdx = NO_READY_ITEM;
    var dmg = 0;
    var dmgSum = 0;
    var defIdx = NO_READY_ITEM;
    var def = 0;
    var attIdx = NO_READY_ITEM;
    var att = 0;
    var data = null;

    var pos = this.myItems.GetHeadPosition();
    while (pos != null) {


        var err;
        err = this.myItems.CanReadyItem(this.myItems.PeekAtPos(pos), this);
        if ((err == Globals.miscErrorType.NoError) && ((data = itemData.GetItemFromID(this.myItems.PeekAtPos(pos).itemID)) != null)) {
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
    if (dmgIdx != NO_READY_ITEM) {
        this.ReadyAmmoScript(dmgIdx);
        return;
    }

    // else try for best attack bonus item
    if (attIdx != NO_READY_ITEM) {
        this.ReadyAmmoScript(attIdx);
        return;
    }

    // else try for best defensive bonus item
    if (defIdx != NO_READY_ITEM) {
        this.ReadyAmmoScript(defIdx);
        return;
    }
};

CHARACTER.prototype.ReadyItemByLocation = function (rdyLoc, index, specAbsOK) {
    if (index == NO_READY_ITEM) {
        // unready all items of this type
        this.UnreadyItemByLocation(rdyLoc, specAbsOK);
        return true;
    }

    if (this.myItems.CanReadyKey(index, this) != miscErrorType.NoError) return false;

    // We see if the character has any of the item's Allowed Baseclasses.
    this.myItems.Ready(index, this, rdyLoc);
    if (this.myItems.IsReady(index)) {
        var pData = itemData.GetItemFromID(this.myItems.GetItem(index));
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

    var pData = itemData.GetItemFromID(this.myItems.GetItem(myitemidx));
    if (pData != null) {
        if (!specAbsOK) {
            pData.specAbs.DisableAllFor(this);
        };
    };

    this.myItems.UnReady(myitemidx); 
};

CHARACTER.prototype.ReadyXXXScript = function (rdyLoc, scriptName, index) {
    if (this.ReadyItemByLocation(rdyLoc, index, true)) {
        if (index != NO_READY_ITEM) {
            var actor;
            var pItem;
            var hookParameters = new HOOK_PARAMETERS();
            var scriptContext = new SCRIPT_CONTEXT();
            actor = this.GetContextActor();
            actor = RunTimeIF.SetCharContext(actor);
            pItem = itemData.GetItemFromID(this.myItems.GetItem(index));
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
    actor = this.GetContextActor();
    actor = RunTimeIF.SetCharContext(actor);
    pItem = itemData.GetItemFromID(this.myItems.GetItem(index));
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
        Globals.WriteDebugString("Wrong item type passed to removeCharMoney()\n");
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
    if (GetType() == MONSTER_TYPE) return false;

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
    return UAFUtil.IsEmpty(this.undeadType) ? "none" : undeadType;
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

    if (wpn != NO_READY_ITEM) {
        var pData = itemData.GetItemFromID(itemID);
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
            var minmax = [];
            var pCombatant;
            minmax[0] = -1;
            minmax[1] = UAFUtil.ByteFromHexString("0x7fffffff");
            actor = this.GetContextActor();
            actor = RunTimeIF.SetCharContext(actor);
            hookParameters[5] = "" + this.GetMaxMovement();
            scriptContext.SetCharacterContext(this);
            // Removed 20101228 PRS....sometimes called outside of combat!!! Like taking treasure.
            //pCombatant = this->GetCombatant();
            //scriptContext.SetCombatantContext(pCombatant);
            this.RunCharacterScripts(SPECAB.GET_ADJ_MAX_MOVEMENT,
                SPECAB.ScriptCallback_MinMax,
                minmax,
                comment);

            pCombatant = this.m_pCombatant;
            if (pCombatant != null) {
                pCombatant.RunCombatantScripts(SPECAB.GET_ADJ_MAX_MOVEMENT,
                    SPECAB.ScriptCallback_MinMax,
                    minmax,
                    comment);
            };
            scriptContext.Clear();
            RunTimeIF.ClearCharContext();
            if ((minmax[0] == -1) && (minmax[1] == 0x7fffffff)) return this.GetMaxMovement();
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
            total += itemData.getItemEncumbrance(this.myItems.PeekAtPos(pos).itemID, this.myItems.PeekAtPos(pos).qty);
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

CHARACTER.prototype.UpdateSpellAbility = function () {
    if (this.GetType() == MONSTER_TYPE) return;

/**
#ifdef UpdateSpellOld   // PORT NOTE: Did not copy over all the commented out code for this - assumed no UpdateSpellOld
#else
**/
    if (this.spellAbility.valid) return;
    this.spellAbility.Clear();
    this.spellAbility.valid = true;
/**
#ifdef OldDualClass20180126   // PORT NOTE: Did not copy over all the commented out code for this - assumed no OldDualClass20180126
#else
**/
    {
        var i, n;
        n = this.baseclassStats.length;
        for (i = 0; i < n; i++) {
            var pBaseclassStats;
            pBaseclassStats = this.baseclassStats[i];       // PORT NOTE:  Used regular array so no need for peeks
            this.UpdateSpellAbilityForBaseclass(pBaseclassStats);
        };
    };
//#endif
//#endif
};

CHARACTER.prototype.UpdateSpellAbilityForBaseclass = function (pBaseclassStats) {
    var m, j, effectiveBaseclassLevel;
    var pBaseclass;
    if (!this.CanUseBaseclassBaseclassStats(pBaseclassStats)) return;
    if (pBaseclassStats.currentLevel > 0)
    {
        effectiveBaseclassLevel = pBaseclassStats.currentLevel;
    }
    else
    {
        effectiveBaseclassLevel = pBaseclassStats.previousLevel;
    };
    if ((pBaseclassStats.pBaseclassData == null)
        ||  (pBaseclassStats.pBaseclassData.BaseclassID() != pBaseclassStats.baseclassID))
    {
        pBaseclassStats.pBaseclassData = baseclassData.GetByBaseclassID(pBaseclassStats.baseclassID);
    };
    pBaseclass = pBaseclassStats.pBaseclassData;
    if (pBaseclass == null) return;
    m = pBaseclass.GetCastingInfoCount();
    for (j=0; j<m; j++)
    {
        var k, spellLevel;
        var primeScore, bonusScore, maxSpells, maxSpellLevel, baseclassLevel;
        var pMaxSpellsByPrime = [];
        var pMaxSpellLevelsByPrime = [];
        var pSpellLimits = [];
        var pCastingInfo = [];
        var pSchoolID = "";
        var pPrimeAbility = "";
        var pBonusAbility = "";
        var pSchoolAbility;
        pCastingInfo = pBaseclass.PeekCastingInfo(j);
        pSchoolID = pCastingInfo.schoolID;
        // Do we already have a spell ability for this school?
        k = spellAbility.LocateSchoolAbility(pSchoolID);
        if (k < 0) {
            var schoolAbility = new SCHOOL_ABILITY();
            schoolAbility.schoolID = pSchoolID;
            k = spellAbility.GetSchoolAbilityCount();
            // We need to add a school to the spellAbility
            spellAbility.Add(schoolAbility);
        };
        pSchoolAbility             = spellAbility.GetSchoolAbility(k);
        pPrimeAbility              = pCastingInfo.primeAbility;
        pBonusAbility              = pBaseclass.m_spellBonusAbility;
        baseclassLevel             = effectiveBaseclassLevel;
        pMaxSpellsByPrime          = pCastingInfo.m_maxSpellsByPrime;
        pMaxSpellLevelsByPrime     = pCastingInfo.m_maxSpellLevelsByPrime;
        primeScore                 = GetAbilityScore(pPrimeAbility);
        bonusScore                 = GetAbilityScore(pBonusAbility);
        pSpellLimits               = pCastingInfo.m_spellLimits[baseclassLevel-1];
        {
          var idx;
          maxSpellLevel            = 0;
          for (idx=0; idx < MAX_SPELL_LEVEL; idx++)
          {
            if (pSpellLimits[idx] != 0) maxSpellLevel = idx+1;
          };
        };
        maxSpells                  = pMaxSpellsByPrime[primeScore-1];
        if (maxSpells > pSchoolAbility.maxSpells)
        {
          pSchoolAbility.maxSpells = maxSpells;
        };
        pSchoolAbility.maxAbilitySpellLevel = pMaxSpellLevelsByPrime[primeScore-1];
        for (spellLevel = 1; spellLevel <= maxSpellLevel; spellLevel++)
        {
            if (pSpellLimits[spellLevel-1] >= pSchoolAbility.base[spellLevel-1])
            {
            pSchoolAbility.base[spellLevel-1] = pSpellLimits[spellLevel-1];
            if (baseclassLevel > pSchoolAbility.baseclassLevel[spellLevel-1])
            {
                pSchoolAbility.baseclassLevel[spellLevel-1] = baseclassLevel;
                // The highest baseclass level that contributed the largest 
                // number of base spells at this spell level.
            };
        };
    };
    {  // Add the bonus spells for high baseclass levels.
      var idx, num;
      num = pBaseclass.m_bonusSpells.GetSize()/3;
      for (idx =0; idx<num; idx++)
      {
        if (bonusScore >= pBaseclass.m_bonusSpells[3* idx])
        {
          var level, bonus;
          level = pBaseclass.m_bonusSpells[3* idx +2];
          if (level > maxSpellLevel) continue;
          if (level > pSchoolAbility.maxAbilitySpellLevel) continue;
          bonus = pBaseclass.m_bonusSpells[3* idx +1];
          pSchoolAbility.bonus[level-1] += bonus;
        };
      };
    };
  };
}

CHARACTER.prototype.CanUseBaseclassBaseclassStats = function (pBaseclassStats) {
    // He can use this baseclass if it is a current baseclass.
    //
    // He can use this baseclass if it is a previous baseclass
    // and a current baseclass level is greater than this previous baseclass level.
    //
    if (pBaseclassStats.currentLevel > 0) return true;
    if (pBaseclassStats.previousLevel <= 0) return false;
    {
        var i, n;
        n = this.baseclassStats.GetCount();
        for (i = 0; i < n; i++) {
            if (this.baseclassStats[i].previousLevel > 0) continue;
            if (this.baseclassStats[i].currentLevel > pBaseclassStats.previousLevel) return true;
        };
    };
    return false;
}

CHARACTER.prototype.GetAllowInCombat = function () {
    return this.allowInCombat;
}

CHARACTER.prototype.GetMorale = function () {
    return this.morale;
}

CHARACTER.prototype.SetAllowPlayerControl = function (flag) {
    this.allowPlayerControl = flag;
    this.SetAutomatic(!this.allowPlayerControl);
}

CHARACTER.prototype.SetAutomatic = function (flag) {
    this.automatic = flag;
}

CHARACTER.prototype.determineNbrAttacks = function () {
    var spellName = "";
    var actor;
    var hookParameters = new HOOK_PARAMETERS();
    var scriptContext = new SCRIPT_CONTEXT();

    var minmax = [- 1, 9999999];
    actor = this.GetContextActor();
    actor = RunTimeIF.SetCharContext(actor);

    var pCombatant;
    pCombatant = this.m_pCombatant;
    if (pCombatant != null) {
        var pWeapon;
        var wpn;
        var weaponID;
        wpn = this.myItems.GetReadiedItem(Items.WeaponHand, 0);
        if (wpn != NO_READY_ITEM) {
            weaponID = this.myItems.GetItem(wpn);
            pWeapon = itemData.GetItemFromID(weaponID);
            scriptContext.SetItemContext(pWeapon);
            scriptContext.SetItemContextKey(wpn);
        };
        scriptContext.SetCombatantContext(pCombatant);
    };

    scriptContext.SetCharacterContext(this);
    if (pCombatant != null) {
        pCombatant.RunCombatantScripts(SPECAB.GET_NUMBER_OF_ATTACKS,
            SPECAB.ScriptCallback_MinMax,
            minmax,
            "Determine number of attacks");
    };
    this.RunCharacterScripts(SPECAB.GET_NUMBER_OF_ATTACKS,
        SPECAB.ScriptCallback_MinMax,
        minmax,
        "Determine number of attacks");
    RunTimeIF.ClearCharContext();

    if (minmax[1] != 9999999) {
        this.SetNbrAttacks(minmax[1]);
        return this.GetNbrAttacks();
    };

    if (this.GetType() != MONSTER_TYPE)
        this.SetNbrAttacks(1.0);
    else
        this.SetNbrAttacks(monsterData.GetMonsterNbrAttacks(this.monsterID));

    if (this.myItems.GetReadiedItem(Items.WeaponHand, 0) != NO_READY_ITEM) {
        var wpnAttacks = itemData.GetItemFromIDROF(this.myItems.GetItem(this.myItems.GetReadiedItem(Items.WeaponHand, 0)));
        if (wpnAttacks < 1.0) wpnAttacks = 1.0;
        this.SetNbrAttacks(wpnAttacks);
        // check for sweeps
    }
    return this.GetNbrAttacks();
}

CHARACTER.prototype.GetContextActor = function (pActor) {               // PORT NOTE: Changed to have a return type since no out params in JS.  Also, changed name because no overrides by parameter types in JS
  //  Avoid all these extra Clears!!!  ActorType data;

    var pActor = new ActorType();
    if (this.m_pCombatant != null)
    {
        return this.m_pCombatant.GetContextActor();
    };
    pActor.Clear();

    if (this.IsPartyMember()) {
        pActor.SetPartySrc();
    }
    else {
        switch (this.GetType()) {
        case MONSTER_TYPE:
            pActor.SetMonsterSrc();
            break;
        case NPC_TYPE:
            pActor.SetNPCSrc();
            if (this.gender == genderType.Bishop) {
                pActor.SetBishopSrc();
            };
            break;
        case CHAR_TYPE:
            // This will be used during character creation. The temp char
            // is displayed before it is added to the party.
            //return NULL_ACTOR;
            pActor.SetCreatedCharacterSrc();
            break;
        case FAKE_CHARACTER_TYPE:
            // Used when processing script $CASTSPELLONTARGET.
            pActor.SetFakeCharacterSrc();
            if (Globals.IsCombatActive()) {
                pActor.Flags &= ~FLAG_NONCOMBAT;
                pActor.Flags |= FLAG_COMBAT;
            };
            break;
        }
    }

    // if ClassFlag=0, GetCurrentLevel will
    // return the highest level attained from
    // all classes this character has
    pActor.Level = 0;  // Unknown level

    pActor.m_raceID = this.race;
    pActor.m_classID = this.GetClass();
    if (this.gender == genderType.Bishop) {
        pActor.Instance = this.origIndex;
        pActor.instanceType = ActorInstanceType.InstanceType_GlobalDataIndex;
    }
    else if (this.IsPartyMember()) {
        pActor.Instance = this.uniquePartyID; // assigned when added to party, never changes
        pActor.instanceType = ActorInstanceType.InstanceType_UniquePartyID;
    }
    else {
        pActor.Instance = this.origIndex;
        pActor.instanceType = ActorInstanceType.InstanceType_OrigIndex;
    };
    return pActor;
}

CHARACTER.prototype.IsPartyMember = function () {
    return ((this.type & IN_PARTY_TYPE) > 0);
}


CHARACTER.prototype.RunCharacterScripts = function (scriptName, fnc, pkt, comment) {
    return this.specAbs.RunScripts(scriptName, fnc, pkt, comment, SCRIPT_SOURCE_TYPE.ScriptSourceType_Character, this.name);

}

CHARACTER.prototype.SetNbrAttacks = function (val) {
    val = Math.max(0, val);
    this.NbrAttacks = val;
}

CHARACTER.prototype.GetNbrAttacks = function () {
    return this.NbrAttacks;
}

CHARACTER.prototype.GetAdjAutomatic = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    // if player control not allowed, then automatic is TRUE
    if (!this.GetAdjAllowPlayerControl(flags)) return true;
    return this.GetAutomatic();
};

CHARACTER.prototype.GetAutomatic = function () {
    return this.automatic;
}

CHARACTER.prototype.GetAllowPlayerControl = function () {
    return this.allowPlayerControl;
}

CHARACTER.prototype.GetAdjAllowPlayerControl = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_ALLOWPLAYERCONTROL;
    var val = this.GetAllowPlayerControl();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.max(0, val);
    val = Math.min(1, val);
    return val;
};

CHARACTER.prototype.GetIsFriendly = function () {
    return (this.m_pCombatant == null) ? true : this.m_pCombatant.GetIsFriendly();
}

CHARACTER.prototype.SetType = function (flag) {
    var inparty = this.IsPartyMember();
    flag &= (~IN_PARTY_TYPE);
    this.type = flag;
    if (inparty) this.SetPartyMember(true);
}

CHARACTER.prototype.SetPartyMember = function (flag) {
    if (flag == undefined) { flag = true; }
    if (flag) this.type |= IN_PARTY_TYPE; else this.type &= (~IN_PARTY_TYPE);
};

CHARACTER.prototype.generateNewCharacter = function (StartExperience, StartExpType) {
    var actor = new ActorType();

    this.GetContextActor();
    RunTimeIF.SetCharContext(actor);

    this.rollStats();
    this.baseclassStats = [];

    // Here we need to add baseclassStats for each
    // baseclass of his current class.
    {
        var pClass;
        var bcs = new BASECLASS_STATS();
        var i = 0, n = 0;
        pClass = classData.PeekClass(this.classID);
        if (pClass == null) {
            n = classData.GetCount();
            if (n == 0) {
                Globals.WriteDebugString("Unable to assign any Class to character\n");
                return;
            };
            pClass = classData.PeekClass(0);
            this.classID = pClass.ClassID();
        }
        else {
            bcs.currentLevel = 1;
            bcs.x_experience = 0;
            bcs.preDrainLevel = 0;
            bcs.previousLevel = 0;
            n = pClass.GetCount();
            for (i = 0; i < n; i++) {
                var baseclassID = "";
                baseclassID = pClass.PeekBaseclassID(i); // PORT NOTE:  CLASS_DATA has .m_baseclasses, a BASECLASS_LIST.  BASECLASS_LIST has .baseclasses, and array of BASECLASS_ID (which probably can just be strings).  So, CLASS_DATA.PeekBaseclassID(idx) is just .m_baseclasses[idx] //                 //classData is a global of CLASS_DATA_TYPE and it has .m_ClassData which is an array of CLASS_DATA //                 // So pClass here is a CLASS_DATA - why does it have multiple BaseclassIDs?  Maybe multiclass?
                bcs.baseclassID = baseclassID;
                baseclassStats.Add(bcs);
            };
        }
    };
    {
        var i;
        i = raceData.LocateRace(this.RaceID());
        if (i < 0) {
            var n;
            n = raceData.GetCount();
            if (n == 0) {
                Globals.WriteDebugString("Cannot assign any Race to character");
                return;
            };
            race = raceData.m_RaceData[0].RaceID();//PORT NOTE: was raceData.PeekRace(0).RaceID(); but I simplified the raceID to a string type and that changed a few things
        };
    };

    if ((this.GetType() == MONSTER_TYPE) || (StartExpType == Globals.START_EXP_VALUE)) {
        Globals.ASSERT(StartExpType == Globals.START_EXP_VALUE, "StartExpType == Globals.START_EXP_VALUE");
        this.giveCharacterExperience(StartExperience, false);
    }
    else // startexp is minimum level
    {
        Globals.ASSERT(GetType() != MONSTER_TYPE, "GetType() != MONSTER_TYPE");
        Globals.die("Not Needed?"); //Not Implemented(0x47abc, false);
    }

    Globals.TRACE(Globals.timeGetTime() + " Before getNewCharLevel\n");

    this.getNewCharLevel(null, 999);

    Globals.TRACE(Globals.timeGetTime() + " After getNewCharLevel\n");

    this.DetermineCharMaxAge();
    this.DetermineCharStartAge(); //(START_AGE + (rand() % (AGE_RANGE+1)));
    this.age = Math.min(this.maxAge, this.age);
    this.SetBirthday(Globals.RollDice(365, 1));

    this.status = charStatusType.Okay;
    this.myItems.Clear();

    this.getNewCharStartingMoney();
    this.getNewCharStartingEquip();
    this.determineNbrAttacks();
    this.DetermineUnarmedHitDice();

    this.maxHitPoints = 0;
    this.hitpointSeed = Globals.randomMT();
    this.DetermineNewCharMaxHitPoints(this.hitpointSeed);
    if (this.maxHitPoints > 0) {
        this.hitPoints = this.maxHitPoints;
        this.UpdateStats(true);
    }  // else we have a race/class definition error

    RunTimeIF.ClearCharContext();
}

CHARACTER.prototype.rollStats = function () {
    var actor;
    actor = this.GetContextActor();
    RunTimeIF.SetCharContext(actor);

    this.strength = GameRules.rollSkillDie(Globals.Ability_Strength);
    this.intelligence = GameRules.rollSkillDie(Globals.Ability_Intelligence);
    this.wisdom = GameRules.rollSkillDie(Globals.Ability_Wisdom);
    this.dexterity = GameRules.rollSkillDie(Globals.Ability_Dexterity);
    this.constitution = GameRules.rollSkillDie(Globals.Ability_Constitution);
    this.charisma = GameRules.rollSkillDie(Globals.Ability_Charisma);


    this.strengthMod = 0;
    if (this.GetAdjStr() == 18) {
        var pClass;
        pClass = classData.PeekClass(classID);
        if (pClass != null) {
            var roll;
            pClass.strengthBonusDice.Roll(roll);
            this.strengthMod = roll;
        }
    }

    this.spellAbility.valid = false;
    RunTimeIF.ClearCharContext();
};

CHARACTER.prototype.RaceID = function (raceID) {
    /**TODO - this is a stub for now - unless this might be all we need here*/
    return raceID;
}

CHARACTER.prototype.RaceID = function () {
    return this.race;
}

CHARACTER.prototype.getNewCharLevel = function (pTrainableBaseclasses, maxLevelGain) {
    if (this.GetType() == MONSTER_TYPE) {
        var pMonster;
        pMonster = monsterData.PeekMonster(this.monsterID);
        if (pMonster == null) return;
        var hd = pMonster.Hit_Dice;
        var bonus = pMonster.Hit_Dice_Bonus;
        if (!pMonster.UseHitDice) // represents hit points instead of hit dice
            hd = (hd / GameRules.FIGHTER_HIT_DIE) + 1;

        this.SetLevelBaseClassLevel(Math.max(hd, 1));
        var hdBonus = bonus;
        if (hdBonus > 0) {
            var adjBonus = (hdBonus / 4) + 1;
            if ((hdBonus % 4) == 0)
                adjBonus--;
            this.SetLevel(max(hd, 1) + adjBonus);
        }
    }
    {
        var i, n, maxLevel, limitLevel;
        maxLevel = 0;
        var baseclassID;
        n = this.GetBaseclassStatsCount();
        for (i = 0; i < n; i++) {
            var j, m;
            var exp;
            var pstats;
            var pTrainableBaseclass;
            var pBaseclass;
            pstats = this.GetBaseclassStats(i);
            if (pstats.currentLevel == 0) {
                continue;  // To next baseclass
            };
            // 20170926 exp = pstats->experience;
            exp = pstats.CurExperience();
            baseclassID = pstats.baseclassID;
            pBaseclass = baseclassData.PeekBaseclass(baseclassID);
            if (pBaseclass == null) {
                Globals.WriteDebugString("Character " + this.GetName() + " has unknown baseclass '" + baseclassID + "'\n");
                continue;
            };
            m = pBaseclass.GetExpLevelsCount();
            for (j = m - 1; j >= 0; j--) {
                if (exp >= pBaseclass.PeekExpLevels(j)) break;
            };
            j++; // j is the new level for this baseclass
            pTrainableBaseclass = null;
            if (pTrainableBaseclasses != null) {
                var k, num;
                num = pTrainableBaseclasses.GetSize();
                for (k = 0; k < num; k++) {
                    var pTB;
                    pTB = pTrainableBaseclasses[k];
                    if (pTB.baseclassID == baseclassID) {
                        pTrainableBaseclass = pTB;
                        break;
                    };
                };
                if (pTrainableBaseclass == null) {
                    continue;  // To next baseclass
                };
            };
            limitLevel = pstats.currentLevel + maxLevelGain;
            if (j > pstats.currentLevel + maxLevelGain) {
                j = pstats.currentLevel + maxLevelGain;
            };
            while (pstats.currentLevel < j) {
                if (pTrainableBaseclass != null) {
                    if (pstats.currentLevel < pTrainableBaseclass.minLevel) break;
                    if (pstats.currentLevel > pTrainableBaseclass.maxLevel) break;
                };
                pstats.currentLevel++;
//#ifdef OldDualClass20180126                   // PORT NOTE:  I don't think this is on
//                temp__canUsePrevClass = -1;
//#endif
            };
            {
                // At the last possible moment we will check for
                // level capping caused by skill "MaxLevel$SYS$" defined
                // in baseclass database and race database.
                var levelCap;
                levelCap = this.GetLevelCap(pBaseclass);
                if (levelCap != Globals.NoSkill) {
                    if (pstats.currentLevel > levelCap) {
                        pstats.currentLevel = levelCap;
                    };
                };
            };
            {
                // Manikus says we should steal experience points
                // so that this fellow does not qualify for any
                // level higher than limitLevel;
                var maxExp;
                if (limitLevel < HIGHEST_CHARACTER_LEVEL) {
                    maxExp = pBaseclass.PeekExpLevels(limitLevel) - 1;
                    if (pstats.previousLevel <= 0) {
                        if (pstats.x_experience > maxExp) {
                            pstats.x_experience = maxExp;
                        };
                    };
                };
            };
        };
    };
}

CHARACTER.prototype.DetermineCharMaxAge = function () {
    if (this.GetType() == MONSTER_TYPE) {
        this.maxAge = 1500;
        return;
    }
    this.maxAge = this.determineCharMaxAgeRace(race);
}

CHARACTER.prototype.DetermineCharStartAge = function () {
    if (this.GetType() == MONSTER_TYPE)
        this.age = GameRules.START_AGE;
    else
        this.age = this.determineCharStartAgeRace(race);
}

CHARACTER.prototype.getNewCharStartingMoney = function () {
    if (this.GetType() == MONSTER_TYPE) return;

    this.money.Clear();
    this.money.Add(this.money.GetDefaultType(), this.GetDesignStartPlatinum());
    for (var g = 0; g < this.GetDesignStartGems(); g++) this.money.AddGem();
    for (var j = 0; j < this.GetDesignStartJewelry(); j++) this.money.AddJewelry();
}

CHARACTER.prototype.getNewCharStartingEquip = function () {
    if (this.GetType() == MONSTER_TYPE) return;
    this.myItems.Clear();
    {
        var pClass;
        pClass = classData.PeekClass(classID);
        if (pClass != null) {
            this.myItems = pClass.m_startingEquipment;
        };
    }
}

CHARACTER.prototype.DetermineUnarmedHitDice = function () {
    if (this.GetType() == MONSTER_TYPE) {
        // Mostly a safety net. In combat the actual
        // attackData array should be used to determine
        // values for natural attacks (no weapons)
        //monsterData.GetMonsterDamageDice(origIndex,
        monsterData.GetMonsterDamageDice(this.monsterID,
            0, // first attack
            this.unarmedNbrDieS,
            this.unarmedDieS,
            this.unarmedBonus);
        this.unarmedDieL = this.unarmedDieS;
        this.unarmedNbrDieL = this.unarmedNbrDieS;
    }
    else {
        this.unarmedDieS = 2;
        this.unarmedNbrDieS = 1;
        this.unarmedBonus = 0;
        this.unarmedDieL = 2;
        this.unarmedNbrDieL = 1;
    }
}

CHARACTER.prototype.DetermineNewCharMaxHitPoints = function (randomSeed) {
    /*
     * A little problem....We want to 're-roll' the HP computation, changing
     * ONLY the bonus for ability scores for Strength, etc.  The computation can be
     * very complex and non-linear because of multiple baseclasses, averages,
     * and such.  So we have our own little random number generator
     * and we seed it before doing anything random.  That way, we can
     * repeat the computation for new values of strength, etc and only
     * changes in ability scores will affect the outcome.
     */
    var ran = new LITTLE_RAN();
    ran.Init(randomSeed);
    if (this.GetType() == MONSTER_TYPE) {
        var pMonster;
        pMonster = monsterData.PeekMonster(this.monsterID);
        if (pMonster == null) return;
        var hd = pMonster.Hit_Dice;
        var bonus = pMonster.Hit_Dice_Bonus;
        if (!pMonster.UseHitDice) // represents hit points instead of hit dice
        {
            maxHitPoints = hd;
        }
        else {
            if (hd < 1.0)
                maxHitPoints = Globals.RollDice(Math.floor(8.0 * hd), 1, bonus);
            else
                maxHitPoints = Globals.RollDice(8, hd, bonus);
        }

        this.maxHitPoints = Math.max(1.0, maxHitPoints);
        return;
    }

    var currClass = this.GetAdjClass();
    var totalBaseClasses;
    var pClass;
    pClass = classData.PeekClass(currClass);
    if (pClass != null) {
        totalBaseClasses = pClass.GetCount();

        if (totalBaseClasses == 0) {
            // we have a race/class configuration error
            Globals.WriteDebugString("Invalid race/class definition - totalClasses = 0 in DetermineNewCharMaxHitPoints()\n");
            return;
        }
    }
    else {
        Globals.WriteDebugString("invlaid class definition = \"" + currClass + "\"\n");
        return;
    };

    // I think what we wnat to do is:
    //
    // For each Baseclass
    //    Get Hit Dice for that baseclass
    //    HP = Roll those dice
    //
    // Add up all the HP for all Baseclasses and take the average.

    var numBaseclass = 0;
    var totalHP = 0;
    var maxHP = 0;
    var specificHP = 0;
    numBaseclass = 0;
    totalHP = 0;
    maxHP = 0;
    specificHP = 0;
    pClass = classData.PeekClass(currClass);
    if (pClass != null) {
        var i = 0, n = 0;
        n = pClass.GetCount();
        for (i = 0; i < n; i++) {
            var pBaseclass;
            var baseclassID = "";
            baseclassID = pClass.PeekBaseclassID(i);
            pBaseclass = baseclassData.PeekBaseclass(baseclassID);
            if (pBaseclass != null) {
                var pBaseclassStats;
                pBaseclassStats = this.PeekBaseclassStats(baseclassID);
                if (pBaseclassStats != null) {
                    var j = 0, level = 0, numDice = 0, sides = 0, constant = 0, bonus = 0, HP = 0;
                    numBaseclass++;
                    level = pBaseclassStats.currentLevel;
                    bonus = DetermineHitDiceBonus(pBaseclassStats.baseclassID);
                    HP = 0;
                    for (j = 1; j <= level; j++) {
                        pBaseclass.GetHitDice(j, sides, numDice, constant);
                        HP += (numDice > 0) ? ran.Roll(sides, numDice, bonus) : 0 + constant;
                    };
                    totalHP += HP;
                }
            }
        }
    }
    this.maxHitPoints = max(1, totalHP);
}

CHARACTER.prototype.SetStatus = function (val) {
    this.status = val;
    if (this.status == charStatusType.Dead)
        this.m_spellEffects.RemoveAll(); 
}

CHARACTER.prototype.SetName = function (val) {
    this.name = val;
    this.name = this.name.trim();
    if (this.name.length > MAX_CHAR_NAME)
        this.name = this.name.substr(0, MAX_CHAR_NAME);
}

CHARACTER.prototype.SetPermInt = function(val)
{
    if (val != this.intelligence) {
        spellAbility.valid = false;
        this.intelligence = val;
        this.UpdateSkillBasedStats();
    };
}

CHARACTER.prototype.UpdateSkillBasedStats = function () {
    var actor;
    actor = this.GetContextActor();
    RunTimeIF.SetCharContext(actor);

    this.maxEncumbrance = this.determineMaxEncumbrance();
    this.encumbrance = this.determineEffectiveEncumbrance();
    this.determineMaxMovement();

    var hb = 0, db = 0;
    var od = 0, omd = 0, bl = 0;
    GameRules.determineStrengthProperties(this.GetLimitedStr(), this.GetLimitedStrMod(),
        hb, db,
        od, omd, bl);
    this.SetHitBonus(hb);
    this.SetDmgBonus(db);
    this.SetThiefSkills();
    this.UpdateSpellAbility();
    this.SetCharBaseAC();
    RunTimeIF.ClearCharContext();
}

CHARACTER.prototype.SetMagicResistance = function (val) {
    if (this.GetType() != MONSTER_TYPE) val = 0;
    val = Math.max(0, val);
    val = Math.min(100, val);
    this.magicResistance = val;
}

CHARACTER.prototype.SetSize = function (val) {
    this.creatureSize = val;
}

CHARACTER.prototype.SetMorale = function (val) {
    val = Math.max(0, val);
    val = Math.min(100, val);
    this.morale = val;
}

CHARACTER.prototype.SetUndead = function (uType) {
    this.undeadType = uType;
}

CHARACTER.prototype.determineMaxMovement = function () {
    if (this.GetType() == MONSTER_TYPE) {
        var val = monsterData.GetMonsterMovement(this.monsterID);
        this.SetMaxMovement(val);
        return this.GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS,
            "Determine monster max movement");
    }

    var ewt = this.determineEffectiveEncumbrance();
    var normalEncumbrance = this.determineNormalEncumbrance();
    var val = 12;

    if (ewt <= normalEncumbrance) val = 12;
    else if (ewt <= (normalEncumbrance * 2)) val = 9;
    else if (ewt <= (normalEncumbrance * 3)) val = 6;
    else if (ewt <= (normalEncumbrance * 4)) val = 3;
    else val = 1;

    this.SetMaxMovement(val);
    return this.GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS,
        "Determine non-monster max movement");
}

CHARACTER.prototype.SetMaxMovement = function (val) {
    if (this.GetType() != MONSTER_TYPE) {
        if (val > Globals.MAX_MOVE) val = Globals.MAX_MOVE;
    }
    this.maxMovement = val;
}

CHARACTER.prototype.SetLevelBaseClassLevel = function (baseclassID, lvl) { 
    var i;
    i = this.LocateBaseclassStats(baseclassID);
    if (i >= 0) {
        var pBaseclassStats;
        pBaseclassStats = this.GetBaseclassStats(i);
        pBaseclassStats.currentLevel = lvl;
    }
}

CHARACTER.prototype.LocateBaseclassStats = function(baseclassID) {
    var i = 0, n = 0;
    n = this.baseclassStats.length;
    for (i = 0; i < n; i++) {
        if (this.baseclassStats[i] == baseclassID) return i;
    }
    return -1;
}

CHARACTER.prototype.GetBaseclassStatsCount = function () {
    return this.baseclassStats.length;
}

CHARACTER.prototype.UpdateStats = function (IsNewChar) {
    var actor;
    actor = this.GetContextActor();
    RunTimeIF.SetCharContext(actor);

    this.checkNewCharRaceScores(IsNewChar);  // Check Ability limits for race
    this.checkNewCharClassScores();          // Check Ability limits for class

    this.UpdateBaseclassLevels();  // They may be based on ability scores.

    this.UpdateSkillBasedStats();            // HitBonus, etc
    this.UpdateLevelBasedStats();            // THAC0, hit dice, etc

    RunTimeIF.ClearCharContext();
}

CHARACTER.prototype.checkNewCharClassScores = function () {

    if (this.GetType() == MONSTER_TYPE) return;

    var data = new CHARSKILLDATA();
    data.m_str = GetPermStr();
    data.m_int = GetPermInt();
    data.m_wis = GetPermWis();
    data.m_dex = GetPermDex();
    data.m_con = GetPermCon();
    data.m_cha = GetPermCha();
    this.CheckNewCharRaceScores(race, data);

    this.SetPermStr(data.m_str);
    this.SetPermInt(data.m_int);
    this.SetPermWis(data.m_wis);
    this.SetPermDex(data.m_dex);
    this.SetPermCon(data.m_con);
    this.SetPermCha(data.m_cha);
}

CHARACTER.prototype.checkNewCharRaceScores = function (ApplyModifiers) {
    if (this.GetType() == MONSTER_TYPE) return;

    var data = new CHARSKILLDATA();
    data.m_str = GetPermStr();
    data.m_int = GetPermInt();
    data.m_wis = GetPermWis();
    data.m_dex = GetPermDex();
    data.m_con = GetPermCon();
    data.m_cha = GetPermCha();

    this.CheckNewCharRaceScores(race, data);

    this.SetPermStr(data.m_str);
    this.SetPermInt(data.m_int);
    this.SetPermWis(data.m_wis);
    this.SetPermDex(data.m_dex);
    this.SetPermCon(data.m_con);
    this.SetPermCha(data.m_cha);
}

CHARACTER.prototype.UpdateLevelBasedStats = function () {
    var actor;
    actor = this.GetContextActor();
    RunTimeIF.SetCharContext(actor);

    this.getCharTHAC0();
    this.SetThiefSkills();
    this.DetermineCharHitDice();
    this.DetermineMaxCureDisease();

    RunTimeIF.ClearCharContext();
};

CHARACTER.prototype.UpdateBaseclassLevels = function () {
    this.getNewCharLevel(null, 999);
};

CHARACTER.prototype.determineMaxEncumbrance = function () {
    if (this.GetType() == MONSTER_TYPE)
        return 100000; // monsters are not encumbered in combat?

    return (this.determineNormalEncumbrance() * 5);
}

CHARACTER.prototype.LimitAb = function (v, min, max) {
    return Math.min(Math.max(v, min), max);
}

CHARACTER.prototype.SetHitBonus = function (val) {
    this.hitBonus = val;
}

CHARACTER.prototype.SetDmgBonus = function (val) {
    this.dmgBonus = val;
}

CHARACTER.prototype.SetThiefSkills = function () {
    // PORT NOTE:  Body of this method was commented out in original code
}

CHARACTER.prototype.SetCharBaseAC = function () {
    if (this.GetType() == MONSTER_TYPE) {
        this.m_AC = monsterData.GetMonsterAC(this.monsterID);
    }
    else {
        var dex = this.GetAdjDex();
        if (dex > 14)
            this.m_AC = 10 - (dex - 14);
        else
            this.m_AC = 10;
        if (this.m_AC > MAX_AC) this.m_AC = MAX_AC;
        if (this.m_AC < MIN_AC) this.m_AC = MIN_AC;
    }
}

CHARACTER.prototype.getCharTHAC0 = function () {
    var i = 0, n = 0, thac0 = 0;
    var pBaseclassStats;
    if (this.GetType() == MONSTER_TYPE) {
        this.THAC0 = monsterData.GetMonsterTHAC0(this.monsterID);
        return;
    }
    n = baseclassStats.GetCount();
    this.THAC0 = 20;
    for (i = 0; i < n; i++) {
        var level;
        pBaseclassStats = this.PeekBaseclassStats(i);
        if (pBaseclassStats == null) continue;
        if (!this.CanUseBaseclassBaseclassStats(pBaseclassStats)) continue;
        if (pBaseclassStats.currentLevel > 0) {
            level = pBaseclassStats.currentLevel;
        }
        else {
            level = pBaseclassStats.previousLevel;
        };
        if (level < 1) continue;
        if (level > HIGHEST_CHARACTER_LEVEL) level = HIGHEST_CHARACTER_LEVEL;
        if ((pBaseclassStats.pBaseclassData == null)
            || (pBaseclassStats.baseclassID != pBaseclassStats.pBaseclassData.BaseclassID())) {
            pBaseclassStats.pBaseclassData = baseclassData.PeekBaseclass(pBaseclassStats.baseclassID);
        };
        if (pBaseclassStats.pBaseclassData == null) continue;
        thac0 = pBaseclassStats.pBaseclassData.THAC0[level - 1];;
        if (thac0 < this.THAC0) this.THAC0 = thac0;
    }
}

CHARACTER.prototype.DetermineCharHitDice = function () {
    if (this.GetType() == MONSTER_TYPE) {
        var pMonster;
        pMonster = monsterData.PeekMonster(this.monsterID);
        if (pMonster == null) return;
        var hd = pMonster.Hit_Dice;
        if (!pMonster.UseHitDice) // represents hit points instead of hit dice
            hd = (hd / FIGHTER_HIT_DIE) + 1;
        if (hd < 1.0) hd = 1.0;
        this.SetNbrHD(hd);
        return;
    }
}

CHARACTER.prototype.SetNbrHD = function (val) {
    val = Math.max(0, val);
    this.nbrHitDice = val;
}

CHARACTER.prototype.DetermineMaxCureDisease = function () {
    this.m_iMaxCureDisease = 0;
    return this.m_iMaxCureDisease;
}


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


CHARACTER.prototype.SetPartyMember = function (flag) {
    if (flag == null || flag == undefined) {
        flag = true;
    }

    if (flag) this.type |= IN_PARTY_TYPE;
    else this.type &= (~IN_PARTY_TYPE);

}


// give this character a chance to modify the attack dice roll used against
// a target during combat
//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackRollDice
//
// PURPOSE: 
//
//*****************************************************************************
CHARACTER.prototype.ModifyAttackRollDice = function (pTarget, num, sides, pBonus) {
    var modify = false;
    var src = 0;   //DWORD
    var spellID = "";
    var result = this.GetAdjSpecAb(SPECAB.SA_Bless, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
    if (result.returnVal) {
        this.QueueUsedSpecAb(SPECAB.SA_Bless, src, spellID);
        pBonus++;
        modify = true;
    }

    result = this.GetAdjSpecAb(SPECAB.SA_Curse, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
    if (result.returnVal) {
        this.QueueUsedSpecAb(SPECAB.SA_Curse, src, spellID);
        pBonus--;
        modify = true;
    }

    // I'm invisible, so give +2 to dice roll
    if (!pTarget.GetAdjDetectingInvisible()) {
        result = this.GetAdjSpecAb(SPECAB.SA_Invisible, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
        if (result.returnVal) {
            pBonus += 2;
            this.QueueUsedSpecAb(SPECAB.SA_Invisible, src, spellID);
            modify = true;
        }
        else {
            result = this.GetAdjSpecAb(SPECAB.SA_InvisibleToAnimals, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
            if (result.returnVal) {
                if (pTarget.IsAnimal()) {
                    pBonus += 2;
                    this.QueueUsedSpecAb(SPECAB.SA_InvisibleToAnimals, src, spellID);
                    modify = true;
                }
            }
            else {
                result = this.GetAdjSpecAb(SPECAB.SA_InvisibleToUndead, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
                if (result.returnVal) {
                    if (!UAFUtil.IsEmpty(pTarget.GetUndeadType())) {
                        pBonus += 2;
                        this.QueueUsedSpecAb(SPECAB.SA_InvisibleToUndead, src, spellID);
                        modify = true;
                    }
                }
            }
        }

    }

    // Target is invisible, so give -4 to dice roll
    if (!this.GetAdjDetectingInvisible()) {
        result = pTarget.GetAdjSpecAb(SPECAB.SA_Invisible, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
        if (result.returnVal) {
            pBonus -= 4;
            pTarget.QueueUsedSpecAb(SPECAB.SA_Invisible, src, spellID);
            modify = true;
        }
        else {
            result = pTarget.GetAdjSpecAb(SPECAB.SA_InvisibleToAnimals, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
            if (result.returnVal) {
                if (this.IsAnimal()) {
                    pBonus -= 4;
                    pTarget.QueueUsedSpecAb(SPECAB.SA_InvisibleToAnimals, src, spellID);
                    modify = true;
                }
            }
            else {
                result = pTarget.GetAdjSpecAb(SPECAB.SA_InvisibleToUndead, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
                if (result.returnVal) {
                    if (!UAFUtil.IsEmpty(this.GetUndeadType())) {
                        pBonus -= 4;
                        pTarget.QueueUsedSpecAb(SPECAB.SA_InvisibleToUndead, src, spellID);
                        modify = true;
                    }
                }
            }
        }
    }
    return { modify: modify, pBonus: pBonus };
}

CHARACTER.prototype.GetAdjSpecAb = function (sa, src, spellID) {
    if (src != null) src = 0;
    if (spellID != null) spellID = "";

    if (this.specAbs.FindAbility(sa) == null) {
        return { returnVal: 0, pSource: src, pSpellName: spellID };
    }

    var akey;
    akey = SPECAB.ConvertSpecAbToRuntimeIfText(sa);
    if (akey.length == 0) return { returnVal: 0, pSource: src, pSpellName: spellID };

    var val = 0.0;
    Globals.die("Not Needed?"); //Not Implemented(0x4f6da8, FALSE);
    return { returnVal: val, src: src, spellID: spellID };
}

CHARACTER.prototype.QueueUsedSpecAb = function (sa, src, SpellID) {
    Globals.die("Not Needed?"); //Not Implemented(0xdaf754,FALSE);
}

CHARACTER.prototype.ModifyAttackDamageDiceForItem = function (pTarget, itemID, num, sides, pBonus, pNonLethal, distance) {
    var weaponType;
    var modify = false;
    weaponType = itemData.GetWpnType(itemID);
    switch (weaponType) {
        case weaponClassType.NotWeapon:
        case weaponClassType.HandBlunt:
        case weaponClassType.HandCutting:
            break;
        case weaponClassType.HandThrow:
            if (distance > 0) pBonus = 0;
            break;
        case weaponClassType.SlingNoAmmo:
        case weaponClassType.Bow:
        case weaponClassType.Crossbow:
        case weaponClassType.Throw:
            pBonus = 0;
            break;
        case weaponClassType.Ammo:
            pBonus = 0;
            break;
        case weaponClassType.SpellCaster:
        case weaponClassType.SpellLikeAbility:
            break;
    }
    return { returnVal: modify, pBonus: pBonus };
}

CHARACTER.prototype.ModifyAttackRollDiceAsTarget = function(pAttacker, num, sides, pBonus) {
    var src = 0;  // DWORD
    var spellID = "";
    var modify = false;
    if (pAttacker.IsAlignmentEvil()) {
        var result = this.GetAdjSpecAb(SPECAB.SA_ProtFromEvil, src, spellID);
        if (result.returnVal) {
            pBonus -= 2;
            this.QueueUsedSpecAb(SPECAB.SA_ProtFromEvil, src, spellID);
            modify = true;
        }
    }
    else if (pAttacker.IsAlignmentGood()) {
        var result = this.GetAdjSpecAb(SPECAB.SA_ProtFromGood, src, spellID);
        if (result.returnVal) {
            pBonus -= 2;
            this.QueueUsedSpecAb(SPECAB.SA_ProtFromGood, src, spellID);
            modify = TRUE;
        }
    }

    var result = this.GetAdjSpecAb(SPECAB.SA_Enfeebled, src, spellID);
    if (result.returnVal) {
        pBonus -= 2;
        this.QueueUsedSpecAb(SPECAB.SA_Enfeebled, src, spellID);
        modify = true;
    }
    return { returnVal: modify, pBonus: pBonus };
}

CHARACTER.prototype.ModifyAttackRollDiceForItem = function(pTarget, itemID, num, sides, pBonus, distance) {
    // PORT NOTE:  This method was pretty much commented out
    var modify = false;            
    return { returnVal: modify, pBonus: pBonus };
}

CHARACTER.prototype.IsAlignmentEvil = function() {
    switch (this.GetAdjAlignment())
    {
        case alignmentType.LawfulEvil:
        case alignmentType.NeutralEvil:
        case alignmentType.ChaoticEvil:
            return true;
    }
    return false;
}

CHARACTER.prototype.IsAlignmentGood = function() {
    switch (this.GetAdjAlignment())
    {
        case alignmentType.LawfulGood:
        case alignmentType.NeutralGood:
        case alignmentType.ChaoticGood:
            return true;
    }
    return false;
}

CHARACTER.prototype.ModifyAttackRollDiceForItemAsTarget = function(pAttacker, itemID, num, sides, pBonus) {
    return { returnVal: false, pBonus: pBonus };
}

CHARACTER.prototype.GetAdjSkillValue = function (skillName, minimize, includeTempAdj) {
    var SC = new SKILL_COMPUTATION(this, skillName, minimize, includeTempAdj);
    SC = Globals.GetAdjSkillValue(SC);
    return SC.finalAdjustedValue == Globals.NoSkillAdj ? Globals.NoSkill : SC.finalAdjustedValue + 0.5;
}

CHARACTER.prototype.PeekRaceData = function () {
    var pRace;
    pRace = raceData.PeekRace(race);
    if (pRace != null) return pRace;
    if (this == Globals.FakeCharacter) return raceData.PeekRace(0);
    if (!debugStrings.AlreadyNoted("CHARPRD")) {
        var msg = "";
        var i = 0, n = 0;
        n = raceData.GetCount();
        msg = "Character '" + name + "' has an illegal race.\nRace : " + race + "\nAvailable races are:\n";
        for (i = 0; i < n; i++) {
            if (i >= 12) {
                msg += ". . . .etc.";
                break;
            };
            msg += raceData.PeekRace(i).Name();
            if (i < n - 1) msg += ",\n";
        };
        Globals.MsgBoxInfo(msg, "ALERT");
    };
    return null;
}

CHARACTER.prototype.ApplyTempSkillAdjustments = function (SC) {
    var i = 0, n = 0;
    var adjType = 0;  //Char
    var adj = 0.0;
    n = this.GetSkillAdjCount();
    for (i = 0; i < n; i++) {
        var ps;
        ps = this.PeekSkillAdj(i);
        if (ps.skillID == SC.skillID) {
            adjType = ps.type;
            switch (adjType) {
                case '%': adj = (SC.baseVal * ps.value + 50) / 100.0 - SC.baseVal; break;
                case '+': adj = ps.value; break;
                case '*': adj = SC.baseVal * (ps.value - 1.0); break;
                case '-': adj = -(ps.value); break;
                case '=': adj = ps.value - SC.baseVal; break;
                default:
                    {
                        if (!debugStrings.AlreadyNoted("CATSAIA")) {
                            var msg = "";
                            msg.Format("CHARACTER::ApplyTempSkillAdjustments - Illegal adjType= " + adjType);
                            Globals.MsgBoxInfo(msg);
                            Globals.WriteDebugString(msg);
                        };
                        return;
                    };
            };
            if (SC.tempAdj == NoSkillAdj) SC.tempAdj = 0.0;
            SC.tempAdj += adj;
        }
    }
    return SC;
}

CHARACTER.prototype.GetSkillAdjCount = function () {
    return this.skillAdjustments.length;
}

CHARACTER.prototype.ModifyACAsTarget = function(pAttacker, pAC, itemID) {
    var src = 0;   // DWORD
    var spellID = "";
    var modify = false;
    if (pAC > 2) {
        var result = this.GetAdjSpecAb(SPECAB.SA_Shield, src, spellID); src = result.pSource, spellID = result.pSpellName;
        if (result.returnVal) {
            if (spellData.IsValidSpell(spellID)) {
                var pItem = itemData.GetItemFromID(itemID);
                if (pItem != null) {
                    switch (pItem.Wpn_Type) {
                        case weaponClassType.NotWeapon:
                        case weaponClassType.Bow:
                        case weaponClassType.Crossbow:
                            break;
                        case weaponClassType.HandBlunt:       pAC=Math.min(pAC, 4); this.QueueUsedSpecAb(SA_Shield, src, spellID); modify = true; break;
                        case weaponClassType.HandCutting:     pAC=Math.min(pAC, 4); this.QueueUsedSpecAb(SA_Shield, src, spellID); modify = true; break;
                        case weaponClassType.HandThrow:       pAC=Math.min(pAC, 2); this.QueueUsedSpecAb(SA_Shield, src, spellID); modify = true; break;
                        case weaponClassType.SlingNoAmmo:     pAC=Math.min(pAC, 3); this.QueueUsedSpecAb(SA_Shield, src, spellID); modify = true; break;
                        case weaponClassType.SpellCaster:     pAC=Math.min(pAC, 3); this.QueueUsedSpecAb(SA_Shield, src, spellID); modify = true; break;
                        case weaponClassType.SpellLikeAbility:pAC=Math.min(pAC, 3); this.QueueUsedSpecAb(SA_Shield, src, spellID); modify = true; break;
                        case weaponClassType.Throw:           pAC=Math.min(pAC, 2); this.QueueUsedSpecAb(SA_Shield, src, spellID); modify = true; break;
                        case weaponClassType.Ammo:            pAC=Math.min(pAC, 3); this.QueueUsedSpecAb(SA_Shield, src, spellID); modify = true; break;
                    }
                }
                else {
                    pAC = Math.min(pAC, 4);
                    this.QueueUsedSpecAb(SPECAB.SA_Shield, src, spellID);
                    modify = true;
                }
            }
            else {
                pAC = Math.min(pAC, 4);
                this.QueueUsedSpecAb(SPECAB.SA_Shield, src, spellID);
                modify = true;
            }
        }
    }
    return { returnVal: modify, pAC: pAC };
}

CHARACTER.prototype.HasDwarfACPenalty = function () {
    if (this.GetType() == MONSTER_TYPE) {
        return ((monsterData.GetMonsterPenaltyFlags(this.monsterID) & MonsterPenaltyType.PenaltyDwarfAC) == MonsterPenaltyType.PenaltyDwarfAC);
    }

    return false;
}

CHARACTER.prototype.HasGnomeACPenalty = function () {
    if (this.GetType() == MONSTER_TYPE)
    {
        return ((monsterData.GetMonsterPenaltyFlags(this.monsterID) & MonsterPenaltyType.PenaltyGnomeAC) == MonsterPenaltyType.PenaltyGnomeAC);
    }

    return false;
}

CHARACTER.prototype.IsAlwaysLarge = function () {
    if (this.GetType() == MONSTER_TYPE) {
        return ((monsterData.GetMonsterFormFlags(this.monsterID) & MonsterFormType.FormLarge) == MonsterFormType.FormLarge);
    }

    return true;  
}

CHARACTER.prototype.GetAdjStatus = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_STATUS;
    var val = this.GetStatus();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    if ((val < 0) || (val >= GameRules.NUM_CHAR_STATUS_TYPES))
        val = this.GetStatus();
    return val;
}


CHARACTER.prototype.GetAdjDetectingInvisible = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_DETECTINVISIBLE;
    var val = this.GetDetectingInvisible();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.max(0, val);
    val = Math.min(1, val);
    return val;
}

CHARACTER.prototype.GetDetectingInvisible = function () {
    return this.detectingInvisible;
}

CHARACTER.prototype.GetAdjAlignment = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_ALIGNMENT;
    var val = this.GetAlignment();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    if ((val < 0) || (val >= GameRules.NUM_ALIGNMENT_TYPES))
        val = this.GetAlignment();
    return val;
}


CHARACTER.prototype.GetAdjSize = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_SIZE;
    var val = this.GetSize();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    if ((val < 0) || (val >= GameRules.NUM_SIZE_TYPES))
        val = this.GetSize();
    // lookup sa's Enlarge and Reduce
    if (this.GetAdjSpecAb(SPECAB.SA_Enlarge))
        val++;
    if (this.GetAdjSpecAb(SPECAB.SA_Reduce))
        val--;

    if (val < creatureSizeType.Small) val = creatureSizeType.Small;
    if (val > creatureSizeType.Large) val = creatureSizeType.Large;

    return val;
}

CHARACTER.prototype.GetAdjDmgBonus = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_DAMAGEBONUS;
    var val = this.GetDmgBonus();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    return val;
};

CHARACTER.prototype.GetDmgBonus = function () {
    return this.dmgBonus;
}

CHARACTER.prototype.ModifyAttackDamageDice = function(pTarget, num, sides, pBonus, pNonLethal)  {
    var src = "";
    var spellID = "";
    var modify = false;
    var result = this.GetAdjSpecAb(SPECAB.SA_Enlarge, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
    if (result.returnVal) {
        var maxdmg = num * sides;
        var bonusdmg = Math.ceil(maxdmg * 0.33);
        if (bonusdmg < 1.0) bonusdmg = 1.0;
        pBonus += bonusdmg;
        this.QueueUsedSpecAb(SPECAB.SA_Enlarge, src, spellID);
        modify = true;
    }
    var result = this.GetAdjSpecAb(SPECAB.SA_Reduce, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
    if (result.returnVal) {
        var maxdmg = num * sides;
        var bonusdmg = ceil(maxdmg * 0.33);
        if (bonusdmg < 1.0) bonusdmg = 1.0;
        pBonus -= bonusdmg;
        QueueUsedSpecAb(SPECAB.SA_Reduce, src, spellID);
        modify = true;
    }

    if (pTarget.HasRangerDmgPenalty()) {
        var pBaseclass;
        var adjClassID;
        adjClassID = this.GetAdjClass();
        var pClass;

        var rangerBonus = 0;
        pClass = classData.PeekClass(adjClassID);
        rangerBonus = this.GetAdjSkillValue(Skill_RangerBonusLevel, false, true);
        if ((rangerBonus != Globals.NoSkill) && (pClass != null)) {
            var numBaseclass = 0, i = 0;
            var scriptContext = new SCRIPT_CONTEXT();
            var hookParameters = new HOOK_PARAMETERS();
            scriptContext.SetAttackerContext(this);
            scriptContext.SetTargetContext(pTarget);
            hookParameters[5] = "" + rangerBonus;
            numBaseclass = pClass.GetCount();
            for (i = 0; i < numBaseclass; i++) {
                var skillID = "";
                skillID = Globals.Skill_RangerBonusLevel;
                pBaseclass = baseclassData.PeekBaseclass(pClass.PeekBaseclassID(i));
                if ((pBaseclass != null) && (pBaseclass.PeekSkill(skillID) != null)) {
                    scriptContext.SetBaseClassContext(pBaseclass);
                    pBaseclass.RunBaseClassScripts(SPECAB.RANGER_DAMAGE_BONUS,
                        SPECAB.ScriptCallback_RunAllScripts,
                        null,
                        "ModifyAttackDamageDice");
                }
            }
            rangerBonus = parseInt(hookParameters[5]);
        };
        if (rangerBonus != Globals.NoSkill) {
          pBonus += rangerBonus;
        };

    };


    var result = this.GetAdjSpecAb(SPECAB.SA_Enfeebled, src, spellID); src = result.src; spellID = result.spellID;   // PORT NOTE:  Dealing with output parameters
    if (result.returnVal) {
        pBonus -= 1;
        this.QueueUsedSpecAb(SA_Enfeebled, src, spellID);
        modify = true;
    }

    
    return { returnVal: modify, pBonus: pBonus, pNonLethal: pNonLethal };
}

CHARACTER.prototype.HasRangerDmgPenalty = function () {
    if (this.GetType() == MONSTER_TYPE) {
        return ((monsterData.GetMonsterPenaltyFlags(this.monsterID) & MonsterPenaltyType.PenaltyRangerDmg) == MonsterPenaltyType.PenaltyRangerDmg);
    }

    return false;
}


// give this character a chance to modify the damage dice roll used against
// it during combat
CHARACTER.prototype.ModifyAttackDamageDiceAsTarget = function(pAttacker, num, sides, pBonus, pNonLethal) {
    // PORT NOTE:  This method contained only "return FALSE";
    return false;
}

CHARACTER.prototype.ModifyAttackDamageDiceForItemAsTarget = function(pAttacker, itemID, num, sides, pBonus, pNonLethal) {
    var sName = "";

    var relAbs = [];
    relAbs[0] = 0;
    relAbs[1] = pBonus;
    var pItem;
    pItem = itemData.GetItemFromID(itemID);
    if (pItem == null) return { pBonus: pBonus };
    {
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        scriptContext.SetAttackerContext(pAttacker);
        scriptContext.SetTargetContext(this);
        scriptContext.SetItemContext(pItem);
        this.RunCharacterScripts(SPECAB.GET_ITEM_TARGET_HIT_BONUS, 
                            SPECAB.ScriptCallback_RelOrAbs, 
                            relAbs, 
                            "Modify attack damage dice for item as target");
        pItem.RunItemScripts(SPECAB.GET_ITEM_TARGET_HIT_BONUS,
                                SPECAB.ScriptCallback_RelOrAbs,
                                relAbs,
                                "Modify attack damage dice for item as target");
    };
    pBonus = relAbs[1] + relAbs[0];         //***TODO:  This is not going to be working right now because it is a bass-by-reference to the scripts **/ 
    return { pBonus: pBonus };
   
}


CHARACTER.prototype.UpdateSpellForAttacks = function(AttacksTaken) {
    // this func is only called when a successful hit is 
    // made on this dude.

    if (AttacksTaken <= 0) return;

    var found = false;
    var pos = this.m_spellEffects.GetHeadPosition();
    while (pos != null) {
        var spellID;
        spellID = this.m_spellEffects.PeekAt(pos).SourceSpell_ID();

        var pSpell = spellData.GetSpell(spellID);
        if (pSpell != null) {
            if (pSpell.Duration_Rate == byNbrAttacks) {
                var activespellkey = m_spellEffects.PeekAt(pos).parent;
                var pActive = activeSpellList.Get(activespellkey);
                if (pActive != null) {
                    found = true;
                    pActive.CountTime += AttacksTaken;
                    activeSpellList.Set(activespellkey, pActive);
                }
            }
        }

        this.m_spellEffects.GetNext(pos);
    }

    if (this.GetAdjSpecAb(SPECAB.SA_MirrorImage)) {
        // decrement counter used to keep track of number of mirror images
        this.AdjustEffectData(IF_KEYWORD_INDEX.CHAR_MIRRORIMAGE, -1);
    }

    if (found)
        activeSpellList.ProcessTimeSensitiveData(0);
}

CHARACTER.prototype.AdjustEffectData = function (key, val) {
    throw "todo";
}

CHARACTER.prototype.AdjustEffectData = function(key, val) {
    // adjust only the first effect that matches
    var pos = this.GetFirstEffectAttributeMod(key);
    if (pos == null)
        return;

    var data = this.m_spellEffects.GetAt(pos);
    Globals.WriteDebugString("Adjusting data for effect " + data.AffectedAttr() + " by " + val + " for " + name + "\n");

    data.data += val;

    var RemoveSpell = false;
    Globals.die("Not Needed?"); //Not Implemented(0xcccd54,FALSE);//specialAbilitiesType sa = ConvertRuntimeIfTextToSpecAb(akey);

    Globals.die("Not Needed?"); //Not Implemented(0x5c9bf6, FALSE);
    if (RemoveSpell) {
        var pSpell = activeSpellList.Get(data.parent);
        if (pSpell != null) {
            // if this is the only effect for this spell, might as well expire it
            // otherwise, wait for normal spell duration
            var pSpellData = spellData.GetSpell(pSpell.spellID);
            if (pSpellData != null) {
                Globals.WriteDebugString("Marking spell " + pSpellData.Name + " from " + name + " in AdjustEffectData() for removal\n");
                if (pSpellData.m_EffectedAttributes.GetCount() == 1)
                    pSpell.StopTime = 0; // force spell to expire
            }
            else {
                pSpell.StopTime = 0; // force spell to expire
                WriteDebugString("Marking spell from " + name + " in AdjustEffectData() for removal\n");
            }
        }
    }
}

CHARACTER.prototype.GetFirstEffectAttributeMod = function(attr) {
    var pos = this.m_spellEffects.GetHeadPosition();
    while (pos != null) {
        if (this.m_spellEffects.PeekAt(pos).affectedAttr_IFKEY == attr)
            return pos;
        this.m_spellEffects.PeekNext(pos);
    }
    return null;
}

CHARACTER.prototype.SetHitPoints = function (val) {
    // are we healing this character?
    if ((val > 0) && (val > this.hitPoints)) {
        // disallow if diseased
        if (this.GetAdjSpecAb(SA_Diseased)) {
            if (val > 1)
                val = 1;
        }
    }


    this.hitPoints = val;

    if (this.hitPoints > this.maxHitPoints)
        this.hitPoints = this.maxHitPoints;
    else if (this.hitPoints < -10)
        this.hitPoints = -10;

    if (this.hitPoints < 0) {
        if (!Globals.IsCombatActive()) // combat takes care of setting correct status
        {
            if (this.hitPoints == -10) {
                this.SetStatus(charStatusType.Dead);
            }
            else {
                // if okay, set to Dead, else leave status alone
                if (this.GetStatus() == charStatusType.Okay)
                    this.SetStatus(charStatusType.Dead);
            }
        }
    }
}

CHARACTER.prototype.UpdateSpellForDamage = function(DamageTaken) {
    if (DamageTaken <= 0) return;

    var found = false;
    var pos = this.m_spellEffects.GetHeadPosition();
    while (pos != null) {
        var spellID;
        spellID = this.m_spellEffects.PeekAt(pos).SourceSpell_ID();

        var pSpell = spellData.GetSpell(spellID);
        if (pSpell != null) {
            if (pSpell.Duration_Rate == spellDurationType.byDamageTaken) {
                var activespellkey = m_spellEffects.PeekAt(pos).parent;
                var pActive = activeSpellList.Get(activespellkey);
                if (pActive != null) {
                    found = true;
                    pActive.CountTime += DamageTaken;
                    activeSpellList.Set(activespellkey, pActive);
                }
            }
        }

        this.m_spellEffects.GetNext(pos);
    }

    if (found)
        activeSpellList.ProcessTimeSensitiveData(0);
}

CHARACTER.prototype.HasDeathImmunity = function () {
    if (this.GetType() == MONSTER_TYPE) {
        return ((monsterData.GetMonsterImmunityFlags(this.monsterID) & MonsterImmunityType.ImmuneDeath) == MonsterImmunityType.ImmuneDeath);
    }

    return FALSE;
}

CHARACTER.prototype.GetAdjMaxEncumbrance = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    var key = IF_KEYWORD_INDEX.CHAR_MAXENC;
    var val = this.GetMaxEncumbrance();
    val = this.ApplySpellEffectAdjustments(flags, key, val);
    val = Math.max(1, val);
    return val;
}

CHARACTER.prototype.determineNormalEncumbrance = function () {
    return GameRules.DetermineNormalEncumbranceStrength(this.GetAdjStr(), this.GetAdjStrMod());
}


CHARACTER.prototype.ReadyXXXScript = function(rdyLoc, scriptName, index) {
    if (this.ReadyItemByLocation(rdyLoc, index, true)) {
        if (index != NO_READY_ITEM) {
            var actor;
            var pItem;
            var hookParameters = new HOOK_PARAMETERS();
            var scriptContext = new SCRIPT_CONTEXT();
            actor = this.GetContextActor();
            RunTimeIF.SetCharContext(actor);
            pItem = itemData.GetItemFromID(this.myItems.GetItemIDByPos(index));
            scriptContext.SetCharacterContext(this);
            scriptContext.SetItemContext(pItem);
            scriptContext.SetItemContextKey(index);
            pItem.RunItemScripts(scriptName,
                SPECAB.ScriptCallback_RunAllScripts,
                null,
                "Character just readied an item");
            RunTimeIF.ClearCharContext();
        }
    }
}

CHARACTER.prototype.UnReadyXXXScript = function(scriptName, index) {
    var actor;
    var pItem;
    var hookParameters = new HOOK_PARAMETERS();
    var scriptContext = new SCRIPT_CONTEXT();
    actor = this.GetContextActor();
    RunTimeIF.SetCharContext(actor);
    pItem = itemData.GetItemFromID(this.myItems.GetItemIDByPos(index));
    scriptContext.SetCharacterContext(this);
    scriptContext.SetItemContext(pItem);
    scriptContext.SetItemContextKey(index);
    pItem.RunItemScripts(scriptName,
        SPECAB.ScriptCallback_RunAllScripts,
        null,
        "Character just un-readied an item");
    RunTimeIF.ClearCharContext();
}

CHARACTER.prototype.PeekBaseclassStats = function (i) {
    return this.baseclassStats[i];
}

CHARACTER.prototype.SetCharAC = function () {
    this.SetCharBaseAC();
    return this.GetEffectiveAC();
}

CHARACTER.prototype.GetEffectiveAC = function () {
    var val = this.GetBaseAC();
    val += this.myItems.GetProtectModForRdyItems();
    val = Math.min(MAX_AC, val);
    val = Math.max(MIN_AC, val);
    return val;
}



CHARACTER.prototype.SetLevel = function (lvl) { throw "todo"; }
CHARACTER.prototype.CanMemorizeSpells = function (circumstance) { throw "todo"; };
CHARACTER.prototype.GetBestMemorizedHealingSpell = function (pSpellID) { throw "todo"; };
CHARACTER.prototype.GetHealingPointsNeeded = function () { throw "todo"; };
CHARACTER.prototype.giveCharacterDamage = function (eventSave, attackTHAC0, dmgDice, dmgDiceQty, dmgBonus, spellSave, saveBonus, pAttacker) { throw "todo"; };
CHARACTER.prototype.giveCharacterDamage = function (damage) { throw "todo"; };
CHARACTER.prototype.TakeDamage = function (dmg, IsNonLethal, invokeOptions, canFinishCasting, pDeathIndex) { throw "todo"; };
CHARACTER.prototype.GetLevelCap = function (pBaseclass) { throw "todo"; };
CHARACTER.prototype.CanBeModified = function () { throw "todo"; }
CHARACTER.prototype.getAllowedAlignments = function () { throw "todo"; }
CHARACTER.prototype.GetThiefBackstabDamageMultiplier = function () { throw "todo"; }
CHARACTER.prototype.GetThiefBackstabAttackAdjustment = function () { throw "todo"; }
CHARACTER.prototype.GetThiefSkillArmorAdustments = function () { throw "todo"; }
CHARACTER.prototype.UpdateSkill = function (abilityModifier, abilityName) { throw "todo"; }
CHARACTER.prototype.TrainCharacter = function (pTrainableBaseclasses) { throw "todo"; }
CHARACTER.prototype.CanChangeToClass = function (pFromClass, pToClass) { throw "todo"; }
CHARACTER.prototype.CreateChangeClassList = function (pClassArray) { throw "todo"; }
CHARACTER.prototype.HumanChangeClass = function (classID) { throw "todo"; }
CHARACTER.prototype.CanUseBaseclassBaseclassID = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.IsDualClass = function () { throw "todo"; }
CHARACTER.prototype.IsUsingBaseclass = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.IsAlive = function () { throw "todo"; }
CHARACTER.prototype.GetContext = function (pActor, baseclassID) { throw "todo"; }
CHARACTER.prototype.GetContext = function (pActor, schoolID) { throw "todo"; }
CHARACTER.prototype.GetContext = function (pActor, pSpell) { throw "todo"; }
CHARACTER.prototype.EvalDiceAsClass = function (dice, baseclassID, pRollerLevel) { throw "todo"; }
CHARACTER.prototype.EvalDiceAsClass = function (dice, schoolID, spellLevel, pRollerLevel) { throw "todo"; }
CHARACTER.prototype.EvalDiceAsClass = function (dice, pSpell, pRollerLevel) { throw "todo"; }
CHARACTER.prototype.WhichBaseclassCastThisSpell = function (pSpell) { throw "todo"; }
CHARACTER.prototype.GetMaxCureDisease = function () { throw "todo"; }
CHARACTER.prototype.SetMaxCureDisease = function (val) { throw "todo"; }
CHARACTER.prototype.GetCureDisease = function () { throw "todo"; }
CHARACTER.prototype.SetCureDisease = function (val) { throw "todo"; }
CHARACTER.prototype.IncCureDisease = function () { throw "todo"; }
CHARACTER.prototype.HaveCureDisease = function () { throw "todo"; }
CHARACTER.prototype.GetAdjMorale = function (flags) { if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; } throw "todo"; };
CHARACTER.prototype.DetermineHitDiceBonus = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.GetNbrHD = function () { throw "todo"; }
CHARACTER.prototype.GetCurrentLevel = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.CurrentBaseclassLevel = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.SetCurrentLevel = function (baseclassID, lvl) { throw "todo"; }
CHARACTER.prototype.ComputeCharSavingThrows = function () { throw "todo"; }
CHARACTER.prototype.DidSaveVersus = function (type, bonus, pSpell, pAttackerr) { throw "todo"; }
CHARACTER.prototype.GetMagicResistance = function () { throw "todo"; }
CHARACTER.prototype.GetAdjMagicResistance = function (flags) { if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; } throw "todo"; };
CHARACTER.prototype.GetHighestLevelBaseclass = function () { throw "todo"; }
CHARACTER.prototype.GetAdjClass = function (flags) { if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; } throw "todo"; };
CHARACTER.prototype.SetGender = function (val) { throw "todo"; }
CHARACTER.prototype.SetGender = function (gender) { throw "todo"; }
CHARACTER.prototype.SetClass = function (classID) { throw "todo"; }
CHARACTER.prototype.SetAlignment = function (val) { throw "todo"; }
CHARACTER.prototype.SetAllowInCombat = function (allow) { throw "todo"; }
CHARACTER.prototype.determineEncumbrance = function () { throw "todo"; }
CHARACTER.prototype.GetPerm = function () { };
CHARACTER.prototype.GetAdj = function (flags) { if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; } throw "todo"; };
CHARACTER.prototype.GetLimited = function () { throw "todo"; }
CHARACTER.prototype.SetPerm = function (val) { throw "todo"; }
CHARACTER.prototype.GetAbilityScore = function (abilityName) { throw "todo"; }
CHARACTER.prototype.CreateAllowedSchoolList = function (pSchoolList) { throw "todo"; }
CHARACTER.prototype.MatchSpellBaseclass = function (pSpell) { throw "todo"; }
CHARACTER.prototype.GetPrimeSpellCastingScore = function (pBaseclass, schoolID) { throw "todo"; }
CHARACTER.prototype.PrimeSpellCastingScore = function (spellSchool) { throw "todo"; }
CHARACTER.prototype.HaveSpell = function (spellID, checkmemorized) { throw "todo"; }
CHARACTER.prototype.GetSpellBookIndex = function (spellID, checkMemorized) { throw "todo"; }
CHARACTER.prototype.GetAbilityLimits = function (abilityID) { throw "todo"; }
CHARACTER.prototype.SetDetectingInvisible = function (flag) { throw "todo"; }
CHARACTER.prototype.GetDetectingTraps = function () { throw "todo"; }
CHARACTER.prototype.SetDetectingTraps = function (flag) { throw "todo"; }
CHARACTER.prototype.GetAdjDetectingTraps = function (flags) { if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; } throw "todo"; };
CHARACTER.prototype.SetTHAC0 = function (val) { throw "todo"; }
CHARACTER.prototype.SetAC = function (val) { throw "todo"; }
CHARACTER.prototype.DetermineCharMaxHitPoints = function () { throw "todo"; }
CHARACTER.prototype.SetHitPointsIntIntBool = function (val, int, canFinishCasting) { throw "todo"; }
CHARACTER.prototype.RestoreMaxHitPoints = function () { throw "todo"; }
CHARACTER.prototype.SetMaxHitPoints = function (val) { throw "todo"; }
CHARACTER.prototype.RestoreToBestCondition = function () { throw "todo"; }
CHARACTER.prototype.SetAge = function (val) { throw "todo"; }
CHARACTER.prototype.GetIconIndex = function () { throw "todo"; }
CHARACTER.prototype.SetIconIndex = function (val) { throw "todo"; }
CHARACTER.prototype.GetBirthday = function () { throw "todo"; }
CHARACTER.prototype.SetMaxAge = function (val) { throw "todo"; }
CHARACTER.prototype.GetAdjMaxMovement_GPDL = function (flags) { if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; } throw "todo"; };
CHARACTER.prototype.IsReadyToTrain = function (pBaseclassStats) { throw "todo"; }
CHARACTER.prototype.IsReadyToTrain = function () { throw "todo"; }
CHARACTER.prototype.GetReadyToTrain = function () { throw "todo"; }
CHARACTER.prototype.GetAdjReadyToTrain = function (flags) { if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; } throw "todo"; };
CHARACTER.prototype.SetReadyToTrain = function (enable) { throw "todo"; }
CHARACTER.prototype.IsAbleToTrade = function () { throw "todo"; }
CHARACTER.prototype.SetAbleToTrade = function (enable) { throw "todo"; }
CHARACTER.prototype.getCharExpWorth = function () { throw "todo"; }
CHARACTER.prototype.giveCharacterExperience = function (exp, UseLimits) { if (UseLimits == undefined) { UseLimits = true; } };
CHARACTER.prototype.GetCurrExp = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.SetCurrExp = function (baseclassID, exp) { throw "todo"; }
CHARACTER.prototype.ClearLevels = function () { throw "todo"; }
CHARACTER.prototype.GetCurrLevel = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.GetAllowedLevel = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.IncCurrExp = function (baseclassID, exp) { throw "todo"; }
CHARACTER.prototype.GetAdjBaseclassExp = function (baseclassID, flags) { if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; } throw "todo"; };
CHARACTER.prototype.GetBaseclassExp = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.SetSpecAb = function (sa, enable, flags) { throw "todo"; }
CHARACTER.prototype.ClearQueuedSpecAb = function () { throw "todo"; }
CHARACTER.prototype.IsMammal = function () { throw "todo"; }
CHARACTER.prototype.IsAnimal = function () { throw "todo"; }
CHARACTER.prototype.IsSnake = function () { throw "todo"; }
CHARACTER.prototype.IsGiant = function () { throw "todo"; }
CHARACTER.prototype.HasDwarfTHAC0Penalty = function () { throw "todo"; }
CHARACTER.prototype.HasGnomeTHAC0Penalty = function () { throw "todo"; }
CHARACTER.prototype.HasPoisonImmunity = function () { throw "todo"; }
CHARACTER.prototype.HasConfusionImmunity = function () { throw "todo"; }
CHARACTER.prototype.HasVorpalImmunity = function () { throw "todo"; }
CHARACTER.prototype.CanBeHeldOrCharmed = function () { throw "todo"; }
CHARACTER.prototype.AffectedByDispelEvil = function () { throw "todo"; }
CHARACTER.prototype.CharacterID = function (id) { throw "todo"; }
CHARACTER.prototype.ProcessLingeringSpellEffects = function () { throw "todo"; }
CHARACTER.prototype.GetSpellCount = function () { throw "todo"; }
CHARACTER.prototype.ClearSpellbook = function () { throw "todo"; }
CHARACTER.prototype.FetchCharacterSpell = function (spellID, pCharSp) { throw "todo"; }
CHARACTER.prototype.LocateCharacterSpell = function (spellID) { throw "todo"; }
CHARACTER.prototype.PeekCharacterSpell = function (index) { throw "todo"; }
CHARACTER.prototype.GetCharacterSpell = function (index) { throw "todo"; }
CHARACTER.prototype.DecMemorized = function (spellID, count) { throw "todo"; }
CHARACTER.prototype.SpellsKnown = function () { throw "todo"; }
CHARACTER.prototype.IsMemorized = function (spellID) { throw "todo"; }
CHARACTER.prototype.IncAllMemorizedTime = function (minuteInc, all) { throw "todo"; }
CHARACTER.prototype.KnowSpell = function (baseclassID, spellSchoolID, spellID, level, known) { throw "todo"; }
CHARACTER.prototype.KnowSpellyyy = function (pSpell, know) { throw "todo"; }
CHARACTER.prototype.CopySpellbook = function (b) { throw "todo"; }
CHARACTER.prototype.UnselectIfNotMemorized = function () { throw "todo"; }
CHARACTER.prototype.CanKnowSpell = function (schoolID, spellLevel) { throw "todo"; }
CHARACTER.prototype.HasKnowableSpell = function (uniqueName) { throw "todo"; }
CHARACTER.prototype.AddKnowableSpell = function (uniqueName) { throw "todo"; }
CHARACTER.prototype.DelKnowableSpell = function (uniqueName) { throw "todo"; }
CHARACTER.prototype.ClrKnowableSpell = function () { throw "todo"; }
CHARACTER.prototype.ComputeAIBaseclass = function () { throw "todo"; }
// 	DEFINE_mCARRAY_ACCESS_FUNCTIONS (SkillAdj, SKILL_ID, skillID, skillAdjustments, SKILL_ADJ, CHARACTER) void DeleteSkillAdj(indx) { throw "todo"; }
CHARACTER.prototype.LocateSkillAdj = function (skillName, adjName) { throw "todo"; }
CHARACTER.prototype.InsertSkillAdj = function (skillName, adjName, type, value) { throw "todo"; }
CHARACTER.prototype.GetBaseclassLevel = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.GetBaseclassPrevLevel = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.GetBaseclassPreDrainLevel = function (baseclassID) { throw "todo"; }
CHARACTER.prototype.SetBaseclassLevel = function (baseclassID, level) { throw "todo"; }
CHARACTER.prototype.ComputeCharacterViewValues = function () { throw "todo"; }
//virtual CHARACTER.prototype.ComputeCombatViewValues () { throw "todo"; }
//DEFINE_mCARRAY_ACCESS_FUNCTIONS (SpellAdj, SCHOOL_ID, schoolID, spellAdjustments, SPELL_ADJ, CHARACTER) CString SpellAdjustment(schoolID
