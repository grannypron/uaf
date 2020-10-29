/** TODO **/
function spellLimitsType() {
    this.UseLimits = false;
}

spellLimitsType.prototype.Clear = function () {
        /** TODO **/
}

spellLimitsType.prototype.SerializeCAR = function (ar, version) {
    if (ar.IsStoring()) {
        ar.writeInt(this.UseLimits);
    }
    else {
        if (version < _VERSION_0780_) {
      /* Really */ Globals.NotImplemented(0xbcbffa, false);
        }
        else {
            if (version >= _VERSION_0780_) {
                {
                    this.UseLimits = ar.readInt();
                };
            }
            else {
                this.UseLimits = true;
            };
        };
    }
}



