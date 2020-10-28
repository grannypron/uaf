function MONSTER_EVENT() {

    this.UseQty = 0; // use qty or roll dice
    this.qty = 0; // absolute number of monsters

    this.qtyDiceSides = 0;    // roll dice for number of monsters
    this.qtyDiceQty = 0;
    this.qtyBonus = 0;

    //int monster; // monster db key
    this.monsterID = ""; // Used to be monster db key
    this.characterID = "";
    this.m_type = 0;    // monster or npc
    this.friendly = false; // whose side is monster on?
    this.moraleAdj = 0;    // This is no longer used.
                    // Originally, this was designed such that the editor set
                    // moraleAdj for each monster in the combat.
                    // But now, the runtime engine determines the moraleAdj
                    // when the combat begins.  Otherwise, a designer who changed
                    // the morale of a monster would have to re-edit each combat
                    // event that included the modified monster.
                    // Now we do this adjustment at runtime to always use the latest
                    // values for the monsters.
    this.money = new MONEY_SACK(); // goodies in monster's inventory
    this.items = new ITEM_LIST();
}


MONSTER_EVENT.prototype.Clear = function () {
    this.moraleAdj = 0;    // This is no longer used.
    // Originally, this was designed such that the editor set
    // moraleAdj for each monster in the combat.
    // But now, the runtime engine determines the moraleAdj
    // when the combat begins.  Otherwise, a designer who changed
    // the morale of a monster would have to re-edit each combat
    // event that included the modified monster.
    // Now we do this adjustment at runtime to always use the latest
    // values for the monsters.
    this.qty = 0;
    this.m_type = MONSTER_TYPE;
    //monster=NO_MONSTER;
    this.monsterID = "";
    this.characterID = "";
    this.friendly = false;
    this.items.Clear();
    this.money.Clear();
    this.qtyDiceSides = 0;
    this.qtyDiceQty = 0;
    this.qtyBonus = 0;
    this.UseQty = MONSTER_EVENT.meUseQty;
}

MONSTER_EVENT.meUseQty = 0;
MONSTER_EVENT.meUsePercent = 1;
