function OBSTICAL_TYPE() {
    this.OBSTICAL_none = 0;
    this.OBSTICAL_wall = 1;
    this.OBSTICAL_occupied = 2;
    this.OBSTICAL_offMap = 3;
    this.OBSTICAL_lingeringSpell = 4;
}

function OBSTICAL_TYPEObj() {
    entityType.call(this, OBSTICAL_TYPE);
    Object.setPrototypeOf(this, entityType.prototype);

    this.ObsticalType = function (x, y, w, h, CheckOccupants, ignoreCurrentCombatant, pCombatantLinger) {
        var i = 0, j = 0, dude = 0;

        for (i = 0; i < h; i++) {
            for (j = 0; j < w; j++) {
                if (!Drawtile.coordsOnMap(x + j, y + i, 1, 1)) return OBSTICAL_TYPE.OBSTICAL_offMap;
                if (!Drawtile.HaveMovability(y + i, x + j)) return OBSTICAL_TYPE.OBSTICAL_wall;

                if (CheckOccupants) {
                    dude = this.getCombatantInCell(x + j, y + i, 1, 1);
                    if ((dude != NO_DUDE) && (!ignoreCurrentCombatant || (dude != combatData.GetCurrCombatant())))
                        return OBSTICAL_TYPE.OBSTICAL_occupied;
                    if (pCombatantLinger != NULL) {
                        if (activeSpellList.LingerSpellBlocksCombatant(x + j, y + i, pCombatantLinger)) return OBSTICAL_TYPE.OBSTICAL_lingeringSpell;
                    };
                }
            }
        }

        return OBSTICAL_TYPE.OBSTICAL_none;
    }
}
