// Requires TestSetupCombat.js
cWarrior = combatData.GetCombatant(0);
//Globals.debug("cWarrior.x / cWarrior.y: " + cWarrior.x + " / " + cWarrior.y + " / " + cWarrior.GetStatus());

// Inject a proxy to determine if the Guarding-CanGuardAttack script is called
var guardScriptCount = 0;
var origExecuteScript = GPDL.prototype.ExecuteScript;
GPDL.prototype.ExecuteScript = function (code, entryPointOrdinal) {
    if (pScriptContext.scriptName == "Guarding-CanGuardAttack") {
        guardScriptCount++;
    }
    return origExecuteScript(code, entryPointOrdinal);
};

var numAttacks = 0;
// Inject a proxy to determine how many attacks were made - I go by the sound here because it's just easier ¯\_(:)_/¯
var origPlaySound = SoundMgrObj.prototype.PlaySound;
SoundMgrObj.prototype.PlaySound = function (file) {
    if (file == "sound_Hit" || file == "sound_Miss") {
        numAttacks++;
    }
    return origPlaySound(file);
};


//**TODO** I think moving to a diagonal attack should work here - see comment at COMBATANT::canAttack() -- cWarrior.MoveCombatant(11, 10, false);

// Moves next to a monster!
cWarrior.MoveCombatant(cWarrior.x - 1, cWarrior.y - 1, false);
// Should have called the Guarding-CanGuardAttack script at least once
Globals.ASSERT(guardScriptCount > 0, "TestCombatMovementAttack.js - 1");
// Monster gets "free attack".  What this equates to is that the player's status gets set to ICS_NONE and the guarder goes to the top of the queue
Globals.ASSERT(cWarrior.State() == individualCombatantState.ICS_None, "TestCombatMovementAttack.js - 2");
Globals.ASSERT(Globals.GetQueuedCombatants().Top() == 2, "TestCombatMovementAttack.js - 2"); // 2 is hardcoded here but that is the index of the enemy that gets put at the block next to one that the character is moving next to


var origX = cWarrior.x, origY = cWarrior.y;
// Moves into an attack!
cWarrior.MoveCombatant(cWarrior.x, cWarrior.y - 1, false);


// Still at the same location
Globals.ASSERT(cWarrior.x == origX, "TestCombatMovementAttack.js - 3");
Globals.ASSERT(cWarrior.y == origY, "TestCombatMovementAttack.js - 4");

// And Attacking a gobby
Globals.ASSERT(cWarrior.State() == individualCombatantState.ICS_Attacking, "TestCombatMovementAttack.js - cWarrior.State() == individualCombatantState.ICS_Attacking");