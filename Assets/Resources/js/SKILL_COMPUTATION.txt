function SKILL_COMPUTATION(pCH, skid, min, includeTemp) {
    this.pChar = pCH;
    this.skillID = skid;
    this.minimize = min;
    this.includeTempAdj = includeTemp;
    this.baseclassID = "";
    this.pRace = this.pChar.PeekRaceData();
    this.pClass = classData.PeekClass(this.pChar.GetClass());
    this.ppBestBaseclass = null;
    this.baseclassLevel = 0;
    this.baseclassValue = Globals.NoSkill;
    this.raceValue = Globals.NoSkill;
    this.baseVal = Globals.NoSkill;    // Best of baseclass and race values
    this.finalAdjustedValue = Globals.NoSkillAdj;
    this.bestRaceAdj = Globals.NoSkillAdj;
    this.bestBaseclassAdj = Globals.NoSkillAdj;
    this.abilityAdj = Globals.NoSkillAdj;
    this.tempAdj = Globals.NoSkillAdj;
}