var weaponClassType = {};
    // CanAttack?   AttackAdjacent?   AttackRanged?   NeedsAmmoReadied?  ConsumesAmmo?
    //
weaponClassType.NotWeapon = 0,       //   No              No               No                N/A              N/A
weaponClassType.HandBlunt = 1,       //   Yes             Yes              Yes               No               No
weaponClassType.HandCutting = 2,     //   Yes             Yes              Yes               No               No
weaponClassType.HandThrow = 3,       //   Yes             Yes              Yes               No               if range > 1
weaponClassType.SlingNoAmmo = 4,     //   Yes             No               Yes               No               No
weaponClassType.Bow = 5,             //   Yes             No               Yes               Yes              Yes
weaponClassType.Crossbow = 6,        //   Yes             No               Yes               Yes              Yes
weaponClassType.Throw = 7,           //   Yes             No               Yes               No               Yes
weaponClassType.Ammo = 8,            //   No              N/A              No                N/A              N/A
weaponClassType.SpellCaster = 9,     //   Yes             Yes              Yes               No               Yes
weaponClassType.SpellLikeAbility = 10//   Yes             YEs              Yes               No               Yes

    //
    //  CanAttack = isWeapon()
    //
    //  AttackRanged = WpnCanAttackAtRange(Range>1)
    //
    //  AttackAdjacent = WpnCanAttackAtRange(Range=1)
    //
    //  NeedsAmmoReadied = WpnUsesAmmo()
    //
    //  ConsumesAmmo = WpnConsumesAmmoAtRange(Range)
    //


function weaponClassTypeObj() {
    entityType.call(this, weaponClassType);
    Object.setPrototypeOf(this, entityType.prototype);
}



var MAX_WEAPON_CLASS = 11;
