function COMBAT_EVENT_DATA() {
    GameEvent.call(this);
    Object.setPrototypeOf(this, GameEvent.prototype);

    this.m_UseOutdoorMap = false; // set at run-time as event activates
    this.distance = eventDistType.getByNumber(0);
    this.direction = eventDirType.getByNumber(0);
    this.m_eSurprise = eventSurpriseType.getByNumber(0);
    this.m_eTEMPsurprise = eventSurpriseType.getByNumber(0);
    this.autoApproach = false;
    this.outdoors = false;
    this.noMonsterTreasure = false;
    this.partyNeverDies = false;
    this.partyNoExperience = false;
    this.noMagic = false;
    this.randomMonster = false;
    this.DeathSound = "";
    this.MoveSound = "";
    this.TurnUndeadSound = "";
    this.monsterMorale = 0;
    this.turningMod = eventTurnUndeadModType.getByNumber(0);
    this.monsters = null;  //MONSTER_EVENT_DATA
    this.bgSounds = null;  //BACKGROUND_SOUND_DATA
    this.needDeathSprite = false;
    this.needDyingSprite = false;

    this.Clear();
    this.event = Combat;
}
