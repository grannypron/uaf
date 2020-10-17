function Globals() {
    this.CHARACTER_VERSION = UAFUtil.ByteFromHexString("0x80000001");
    this.combatData = new COMBAT_DATA();
    this.version = 6.0;
    this.debugSeverity = 0;
}

Globals.prototype.die = function (message) {
    throw message;
}

Globals.prototype.debug = function (msg) {
    var log = System.Console.WriteLine; log(msg);
}

Globals.prototype.debugStop = function (msg) {
    var log = Globals.debug(msg); Globals.die("debugStop");
}

Globals.prototype.IsCombatActive = function () {
    return this.combatData.m_bCombatActive;
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

Globals.prototype.ASSERT = function (val) {
    if (!val) {
        throw "Assertion failed.";
    }
}

Globals.prototype.GetGameVersion = function(data) {
    return data.version;
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

var Globals = new Globals();