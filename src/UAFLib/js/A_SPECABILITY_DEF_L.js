/** PORT NOTE: I just couldn't port over another one of these list types - it is too easy to just move it directly into the weakly typed JS array */
function A_SPECABILITY_DEF_L() {
    this.mItems = [];
}

A_SPECABILITY_DEF_L.prototype.Clear = function () {
    this.mItems = [];
}

A_SPECABILITY_DEF_L.prototype.FindAbility = function (key) {
    for (idx = 0; idx < this.mItems.length; idx++) {
        if (this.mItems[idx].m_specAbName == key) {
            return this.mItems[idx];
        }
    }
    return null;
}
