function SCRIPT_SOURCE_TYPE() {
    this.ScriptSourceType_Unknown = 0;
    this.ScriptSourceType_Class = 1;
    this.ScriptSourceType_Item = 2;
    this.ScriptSourceType_Race = 3;
    this.ScriptSourceType_Baseclass = 4;
    this.ScriptSourceType_Spell = 5;
    this.ScriptSourceType_Character = 6;
    this.ScriptSourceType_Monster = 7;
    this.ScriptSourceType_Combatant = 8;
    this.ScriptSourceType_Aura = 9;
    this.ScriptSourceType_Event = 10;
    this.ScriptSourceType_EventTrigger = 11;
    this.ScriptSourceType_ForEachPrtyMember = 12;
}

function SCRIPT_SOURCE_TYPEObj() {
    entityType.call(this, SCRIPT_SOURCE_TYPE);
    Object.setPrototypeOf(this, entityType.prototype);
}

