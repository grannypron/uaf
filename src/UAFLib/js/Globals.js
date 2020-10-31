function Globals() {
    this.CHARACTER_VERSION = UAFUtil.ByteFromHexString("0x80000001");
    this.logDebuggingInfo = false;
    this.version = 6.0;
    this.debugSeverity = 0;

    this.PlaySoundEffects = true;

    this.miscError = 0;
    this.miscErrorText = [];

    this.MIN_INTELLIGENCE = 3;
    this.MAX_INTELLIGENCE = 25;
    this.MIN_STRENGTH = 3;
    this.MAX_STRENGTH = 25;
    this.MIN_STRENGTHMOD = 0;
    this.MAX_STRENGTHMOD = 100;
    this.MIN_WISDOM = 3;
    this.MAX_WISDOM = 25;
    this.MIN_DEXTERITY = 3;
    this.MAX_DEXTERITY = 25;
    this.MIN_CONSTITUTION = 3;
    this.MAX_CONSTITUTION = 25;
    this.MIN_CHARISMA = 3;
    this.MAX_CHARISMA = 25;

    this.CombatScreenX = 14;
    this.CombatScreenY = 14;
    this.TILES_HORZ = 8;
    this.TILES_VERT = 9;

    this.SCREEN_WIDTH = 640;  // these change to reflect the current
    this.SCREEN_HEIGHT = 480; // screen resolution for the editor
    this.COLOR_BITS = 24;

    this.START_EXP_VALUE = 0;
    this.START_LVL_VALUE = 1;

    this.MAX_COMBATANTS = 100;
    this.MAX_CHARS_IN_ROW = 3; // how many chars wide the battle line can be
    this.MAX_COMBAT_IDLE_ROUNDS = 20;

    this.Ability_Dexterity = "Dexterity";
    this.Ability_Constitution = "Constitution";
    this.Ability_Strength = "Strength";
    this.Ability_Intelligence = "Intelligence";
    this.Ability_Charisma = "Charisma";
    this.Ability_Wisdom = "Wisdom";

    this.Baseclass_magicUser = "magicUser";
    this.Baseclass_thief = "thief";
    this.Baseclass_paladin = "paladin";
    this.Baseclass_cleric = "cleric";
    this.Baseclass_fighter = "fighter";
    this.Baseclass_ranger = "ranger";
    this.Baseclass_druid = "druid";

    this.NoSkill = UAFUtil.ByteFromHexString("0x80000000");
}

Globals.prototype.die = function (message) {
    throw message;
}

Globals.prototype.debug = function (msg) {
    var log = System.Console.WriteLine; log(msg);
}

Globals.prototype.TRACE = function (msg) {
    this.debug(msg);
}

Globals.prototype.WriteDebugString = function (msg) {
    this.debug(msg);
}

Globals.prototype.MsgBoxError = function (msg) {
    /** TODO **/
}

Globals.prototype.debugStop = function (msg) {
    var log = Globals.debug(msg); Globals.die("debugStop");
}

Globals.prototype.IsCombatActive = function () {
    return combatData.m_bCombatActive;
}

Globals.prototype.GetMaxPartyMembers = function()
{
    return (Math.min(MAX_PARTY_MEMBERS, globalData.GetMaxPartySize()));
}

Globals.prototype.StripFilenamePath = function (filename) {
    if (filename == "" || filename == null)
        return filename;

    // if filaname ends with '\', strip it
    if (filename.charAt(filename.length - 1) == "\\") {
        filename = filename.substring(0, filename.length - 1);
    }
    if (filename.indexOf('\\') < 0) return filename;
    return filename.substr(filename.lastIndexOf("\\") + 1);
}

Globals.prototype.ASSERT = function (val, msg) {
    if (!val) {
        throw "Assertion failed.\n" + msg;
    }
}

Globals.prototype.GetGameVersion = function(data) {
    return data.version;
}

Globals.prototype.RollDice = function(sides, numTimes, bonus) {
    if (sides <= 0) return bonus;//0.0;
    if (numTimes <= 0) return bonus;//0.0;
    var result = 0.0;
    for (var i = 0; i < numTimes; i++)
    result += ((this.randomMT() % sides) + 1);
    result += bonus;
    return result;
}

Globals.prototype.randomMT = function () {
    // Passing this off to an RNG that can be swapped in
    return RNG.newDWORD();
}


Globals.prototype.GetMonsterQtyMod = function()
{
    var lvl = Globals.GetCurrSkillLevel();
    if ((lvl < 0) || (lvl > 4)) return 0;
    if (globalData.m_diffLvlData.m_data[lvl].m_ModifyQty)
        return globalData.m_diffLvlData.m_data[lvl].m_QtyAmount;
    else
        return 0;
}

Globals.prototype.timeGetTime = function() {
    return (new Date()).getTime();
}

Globals.prototype.GetCurrSkillLevel = function()
{
    return party.skillLevel;
}

Globals.prototype.GetConfigMonsterPlyrControl = function () {
    return combatData.m_bMonsterPlyrControl;
}

Globals.prototype.SetMiscError = function (error) {
    if ((error >= 0) && (error < MAX_MISC_ERROR_MSGS)) {
        miscError = error;
        if (error != miscErrorType.NoError) {
            if (!this.debugStrings.AlreadyNoted("SME01" + miscErrorText[error])) {
                this.WriteDebugString("MISC ERROR: " + miscErrorText[error] + "\n");
            };
        };
    }
}

Globals.prototype.GetConfigMonsterNoMove = function () {
    return combatData.m_bMonsterNoMove;
}

Globals.prototype.PlayCombatMove = function () {
    combatData.PlayCombatMove();
}

Globals.prototype.GetQueuedCombatants = function () {
    return combatData.QComb;
}

Globals.prototype.PlaceCursorOnCurrentDude = function () {
    combatData.PlaceCursorOnCurrentDude();
}

Globals.prototype.GetCombatantPtr = function (index) {
    return (combatData.getCombatantPtr(index));;
}

Globals.prototype.GetCurrentRound = function () {
    return combatData.m_iCurrRound;
}