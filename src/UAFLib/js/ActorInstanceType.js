function ActorInstanceType() {
    this.InstanceType_Unknown = 0;
    this.InstanceType_UniquePartyID = 1;
    this.InstanceType_OrigIndex = 2;
    this.InstanceType_Invalid = 3;
    this.InstanceType_CombatantIndex = 4;
    this.InstanceType_GlobalDataIndex = 5;
    this.InstanceType_NewCombatant = 6;
//#ifdef SpellInitiationScript        // PORT NOTE: ???
    this.InstanceType_NullActor = 7;  // And any other flags are meaningless
//#endif
}

function ActorInstanceTypeObj() {
    entityType.call(this, ActorInstanceType);
    Object.setPrototypeOf(this, entityType.prototype);
}

