function Globals() {
    this.CHARACTER_VERSION = UAFUtil.ByteFromHexString("0x80000001");
    this.logDebuggingInfo = false;
    this.version = 6.0;
    this.debugSeverity = 0;
}

Globals.prototype.die = function (message) {
    throw message;
}

Globals.prototype.debug = function (msg) {
    var log = System.Console.WriteLine; log(msg);
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

Globals.MIN_INTELLIGENCE = 3;
Globals.MAX_INTELLIGENCE = 25;
Globals.MIN_STRENGTH = 3;
Globals.MAX_STRENGTH = 25;
Globals.MIN_STRENGTHMOD = 0;
Globals.MAX_STRENGTHMOD = 100;
Globals.MIN_WISDOM = 3;
Globals.MAX_WISDOM = 25;
Globals.MIN_DEXTERITY = 3;
Globals.MAX_DEXTERITY = 25;
Globals.MIN_CONSTITUTION = 3;
Globals.MAX_CONSTITUTION = 25;
Globals.MIN_CHARISMA = 3;
Globals.MAX_CHARISMA = 25;

Globals.CombatScreenX = 14;
Globals.CombatScreenY = 14;
Globals.TILES_HORZ = 8;
Globals.TILES_VERT = 9;

Globals.SCREEN_WIDTH = 640;  // these change to reflect the current
Globals.SCREEN_HEIGHT = 480; // screen resolution for the editor
Globals.COLOR_BITS = 24;

Globals.MAX_COMBATANTS = 100;
Globals.MAX_CHARS_IN_ROW = 3; // how many chars wide the battle line can be
Globals.MAX_COMBAT_IDLE_ROUNDS = 20;




var Globals = new Globals();