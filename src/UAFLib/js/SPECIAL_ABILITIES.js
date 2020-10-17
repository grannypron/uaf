/** TODO **/
function SPECIAL_ABILITIES() {
    this.m_specialAbilities = new A_CStringPAIR_L();
}


SPECIAL_ABILITIES.prototype.Clear = function () {
    /**TODO**/
}


SPECIAL_ABILITIES.prototype.SerializeCAR = function(ar, version, objName, objType) {
    if ((version <= 0.920) && !ar.IsStoring()) {
        if (ar.IsStoring()) {
      /* Really */ Globals.NotImplemented(0x8da3f, false);
        }
        else {
      /* Really */ Globals.NotImplemented(0x54fa23, false);
        };
    }
    else {
        this.m_specialAbilities.SerializeCAR(ar);
    };
}



function SPECAB() {

    this.MAX_SPEC_AB = 100;

    this.INVOKE_SPELL_ON_TARGET =       "InvokeSpellOnTarget";
    this.GET_ADJ_MAX_MOVEMENT =         "GetAdjMaxMovement";
    this.CAN_CAST_SPELLS =              "CanCastSpells";
    this.CAN_MEMORIZE_SPELLS =          "CanMemorizeSpells";
    this.IS_COMBAT_READY =              "IsCombatReady";
    this.CHAR_DISPLAY_STATUS =          "CharDisplayStatus";
    this.GET_NUMBER_OF_ATTACKS =        "GetNumberOfAttacks";
    this.GET_ITEM_TARGET_HIT_BONUS =    "GetItemTargetHitBonus";
    this.SPECIAL_GRAPHICS =             "SpecialGraphics";

//    this.ON_READY_WEAPON             "OnReadyWeapon"
//    this.ON_UNREADY_WEAPON           "OnUnReadyWeapon"
//    this.ON_READY_SHIELD             "OnReadyShield"
//    this.ON_UNREADY_SHIELD           "OnUnReadyShield"
//    this.ON_READY_ARMOR              "OnReadyArmor"
//    this.ON_UNREADY_ARMOR            "OnUnReadyArmor"
//    this.ON_READY_GAUNTLETS          "OnReadyGauntlets"
//    this.ON_UNREADY_GAUNTLETS        "OnUnReadyGauntlets"
//    this.ON_READY_HELM               "OnReadyHelm"
//    this.ON_UNREADY_HELM             "OnUnReadyHelm"
//    this.ON_READY_BELT               "OnReadyBelt"
//    this.ON_UNREADY_BELT             "OnUnReadyBelt"
//    this.ON_READY_ROBE               "OnReadyRobe"
//    this.ON_UNREADY_ROBE             "OnUnReadyRobe"
//    this.ON_READY_CLOAK              "OnReadyCloak"
//    this.ON_UNREADY_CLOAK            "OnUnReadyCloak"
//    this.ON_READY_BOOTS              "OnReadyBoots"
//    this.ON_UNREADY_BOOTS            "OnUnReadyBoots"
//    this.ON_READY_RING               "OnReadyRing"
//    this.ON_UNREADY_RING             "OnUnReadyRing"
//    this.ON_READY_AMMO               "OnReadyAmmo"
//    this.ON_UNREADY_AMMO             "OnUnReadyAmmo"
    this.CAN_READY =                    "CanReady";
    this.ON_READY =                     "OnReady";
    this.ON_UNREADY =                   "OnUnReady";
}


var SPECAB = new SPECAB();