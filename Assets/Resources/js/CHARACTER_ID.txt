/** TODO **/
function CHARACTER_ID(uniqueName) {
    this.uniqueName = uniqueName;
}
CHARACTER_ID.prototypeIsValidNPC = function () { /** TODO **/ };
CHARACTER_ID.prototype.Clear = function () { this.uniqueName = "" };
CHARACTER_ID.prototype.UniqueName = function () { return this.uniqueName; };