/** TODO **/
function BASECLASS_STATS() {
    this.baseclassID = null;  //BASECLASS_ID
    this.pBaseclassData = null;  // BASE_CLASS_DATA
    this.currentLevel = 0;
    this.temp__origLevel = 0; // Temporary....not serialized
    this.previousLevel = 0;
    this.preDrainLevel = 0;
    this.x_experience = 0;

}

BASECLASS_STATS.prototype.Equals = function (bcs) {
    if (this.baseclassID != bcs.baseclassID) return false;
    if (this.currentLevel != bcs.currentLevel) return false;
    if (this.previousLevel != bcs.previousLevel) return false;
    if (this.preDrainLevel != bcs.preDrainLevel) return false;
    if (this.x_experience != bcs.x_experience) return false;
    return true;
}

BASECLASS_STATS.prototype.CurExperience = function() {
    if (this.previousLevel > 0) return 0;
    return this.x_experience;
}

BASECLASS_STATS.prototype.IncCurExperience = function(exp) {
    if (this.previousLevel > 0) return 0;
    return this.x_experience += exp;
}

BASECLASS_STATS.prototype.Clear = function () {
    /**TODO**/
}


BASECLASS_STATS.prototype.Serialize = function(ar) {
    Globals.die("BASECLASS_STATS should not serialize to CArchive"); // Not Implemented(0x2e4da65, false);
    return ar;
}

BASECLASS_STATS.prototype.SerializeCAR = function(car, version) {
    if (car.IsStoring()) {
        car.writeString(baseclassID);
        car.writeInt(currentLevel);
        car.writeInt(previousLevel);
        car.writeInt(preDrainLevel);
        car.writeInt(x_experience);
    }
    else {
        this.baseclassID = car.readString();
        this.currentLevel = car.readInt();
        this.previousLevel = car.readInt();
        this.preDrainLevel = car.readInt();
        this.x_experience = car.readInt();
    };
    return car;
}