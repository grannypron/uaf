function UIEventManager() {
    this.PlaySound = function (file) { Globals.debug("PlaySound: " + file); }
    this.CombatantMoved = function (x, y, dude, w, h) { Globals.debug("CombatantMoved: " + x + ", " + y + ", " + dude + ", " + w + ", " + h); }
    this.CenterMap = function () { Globals.debug("Map should be centered now"); }
    this.HitPointsUpdated = function (combatantIdx, value) { Globals.debug("Combatant " + combatantIdx + " hit points has been updated to " + value); }
}
