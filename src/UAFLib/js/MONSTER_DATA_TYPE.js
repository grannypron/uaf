/** TODO **/
function MONSTER_DATA_TYPE() {
    // **TODO** **EFFICIENCY** - Rewrite so that .MonsterData is an associative array indexed by a monsterID string
    this.MonsterData = [];          // Array of MONSTER_DATA

    this.preSpellNameKey = 0;  //See comment at defintion of VersionSaveIDs 
    this.Name = "";  // In the engine, the printable name.
                 // In the editor, the unique name.
    this.monsterID = "";  // Not serialized.  Used as unique ID in the engine.
    this.Icon = new PIC_DATA();
    this.classID = "";
    this.raceID = ""; // Serialized via ASL

    this.HitSound = null; //SOUND_BYTE 
    this.MissSound = null; //SOUND_BYTE 
    this.MoveSound = null; //SOUND_BYTE 
    this.DeathSound = null; //SOUND_BYTE 

    this.Intelligence = 0;  //long
    this.Armor_Class = 0;  //long
    this.Movement = 0;  //long
    this.Hit_Dice = 0.0;
    this.UseHitDice = false;
    this.Hit_Dice_Bonus = 0;  //long
    this.THAC0 = 0;  //long
    this.Magic_Resistance = 0;  //long
    this.Size = creatureSizeType.getByNumber(0);
    this.Morale = 0;  //long
    this.XP_Value = 0;  //long

    this.specAbs = new SPECIAL_ABILITIES();

    this.Form_Type = 0;  //DWORD
    this.Penalty_Type = 0;  //DWORD
    this.Immunity_Type = 0;  //DWORD
    this.MiscOptions_Type = 0;  //DWORD
    this.myItems = new ITEM_LIST();
    this.money = new MONEY_SACK();

    this.undeadType = "";

    this.attackData = new ATTACK_DATA();
    this.mon_asl = new A_ASLENTRY_L();
    this.temp_asl = new A_ASLENTRY_L();
    this.ClearSounds();
    this.Clear(true);
}

MONSTER_DATA_TYPE.prototype.Clear = function (ctor) {
    this.Name = "";
    this.preSpellNameKey = -1;
    this.Icon.filename = "DEFAULT";
    this.Icon.picType = surfaceType.IconDib;
    this.Icon.SetDefaults();
    this.HitSound = "DEFAULT";
    this.MissSound = "DEFAULT";
    this.MoveSound = "DEFAULT";
    this.DeathSound = "DEFAULT";
    this.ClearSounds();
    this.Intelligence = 0;
    this.Armor_Class = 0;
    this.Movement = 0;
    this.Hit_Dice = 0.0;
    this.UseHitDice = true;
    this.Hit_Dice_Bonus = 0;
    this.THAC0 = 20;
    this.Magic_Resistance = 0;
    this.Size = creatureSizeType.Medium;
    this.Morale = 0;
    this.XP_Value = 0;
    this.specAbs.Clear();
    this.Form_Type = 0;
    this.Penalty_Type = 0;
    this.Immunity_Type = 0;
    this.MiscOptions_Type = 0;
    this.mon_asl.Clear();
    this.myItems.Clear();
    this.money.Clear();
    this.undeadType = "";
    this.attackData.Clear();
    this.raceID = "Human";
}

MONSTER_DATA_TYPE.prototype.FreeArt = function() {
    var i, n;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.GetItem(i).FreeArt();
    }
}

MONSTER_DATA_TYPE.prototype.GetCount = function () {
    return this.MonsterData.length;
}

MONSTER_DATA_TYPE.prototype.ClearSounds = function () {
    var i, n;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.MonsterData[i].ClearSounds();
    };

}

MONSTER_DATA_TYPE.prototype.PeekMonster = function (idx) {
    return this.MonsterData[idx];
}

MONSTER_DATA_TYPE.prototype.LocateMonster = function (monsterID) {
    for (var idx = 0; idx < this.MonsterData.length; idx++) {
        if (this.MonsterData[idx].monsterID = monsterID) {
            return this.MonsterData[idx];
        }
    }    
    return -1;
}
