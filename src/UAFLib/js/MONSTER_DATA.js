function MONSTER_DATA() {
    this.preSpellNameKey = "";  //See comment at defintion of VersionSaveIDs 
    this.Name = "";  // In the engine, the printable name.
                 // In the editor, the unique name.
    this.monsterID;  // Not serialized.  Used as unique ID in the engine.
    this.Icon = new PIC_DATA();
    this.classID = "";
    this.raceID = ""; // Serialized via ASL
    this.HitSound = new SOUND_BYTE();
    this.MissSound = new SOUND_BYTE();
    this.MoveSound = new SOUND_BYTE();
    this.DeathSound = new SOUND_BYTE();

    this.Intelligence = 0;  // long
    this.Armor_Class = 0;  // long
    this.Movement = 0;  // long
    this.Hit_Dice = 0.0;
    this.UseHitDice = false;
    this.Hit_Dice_Bonus = 0;  // long
    this.THAC0 = 0;  // long
    this.Magic_Resistance = 0;  // long
    this.Size = 0;
    this.Morale = 0;  // long
    this.XP_Value = 0;  // long
    this.specAbs = new SPECIAL_ABILITIES();
    this.Form_Type = 0;     //DWORD
    this.Penalty_Type = 0;     //DWORD
    this.Immunity_Type = 0;     //DWORD
    this.MiscOptions_Type = 0;     //DWORD
    this.myItems = new ITEM_LIST();
    this.money = new MONEY_SACK();

    this.undeadType = "";

    this.attackData = new ATTACK_DATA();
    this.mon_asl = new A_ASLENTRY_L();
    this.temp_asl = new A_ASLENTRY_L();
}

MONSTER_DATA.prototype.ClearSounds = function() {
    this.HitSound.Clear();
    this.MissSound.Clear();
    this.MoveSound.Clear();
    this.DeathSound.Clear();
}

MONSTER_DATA.prototype.MonsterID = function() {
    var mID; 
    //#ifdef UAFEngine
        mID = this.Name;   // PORT NOTE:  Changed from mID = this.monsterID because I am using a string as the monsterID to simplify now
    //#else
    //    mID = Name; 
    //#endif
    return mID;
}

MONSTER_DATA.prototype.ToString = function () {
    return "MONSTER_DATA";
}

MONSTER_DATA.prototype.RunMonsterScripts = function (scriptName, fnc, pkt, comment) {
    return this.specAbs.RunScripts(scriptName,
        fnc,
        pkt,
        comment,
        SCRIPT_SOURCE_TYPE.ScriptSourceType_Monster,
        this.Name);
}


MONSTER_DATA.prototype.PlayMove = function () {
    if (!Globals.PlaySoundEffects) return;
    if (!this.MoveSound.Play()) {
        this.MoveSound = new SOUND_BYTE(SoundMgr.DEF_CHAR_MOVE_SOUND);  // PORT NOTE: Not sure how this worked with DEF_CHAR_MOVE_SOUND is a CString, so I wrapped in a SOUND_BYTE
        if (!this.MoveSound.Play()) {                       
            this.MoveSound = new SOUND_BYTE(globalData.sounds.PartyStep);
            if (!this.MoveSound.Play()) {
                this.MoveSound.TotalDisable();
            }
        }
    }
}


MONSTER_DATA.prototype.PlayMiss = function()
{
    if (!Globals.PlaySoundEffects) return;
    this.MissSound.Play();
}

MONSTER_DATA.prototype.PlayHit = function () {
    if (!Globals.PlaySoundEffects) return;
    this.HitSound.Play();
}