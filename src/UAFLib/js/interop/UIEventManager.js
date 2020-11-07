function UIEventManager() {
    this.PlaySound = function (file) { Globals.debug("PlaySound: " + file); }
    this.CombatantMoved = function (x, y, dude, w, h) { Globals.debug("CombatantMoved: " + x + ", " + y + ", " + dude + ", " + w + ", " + h ); };
}
