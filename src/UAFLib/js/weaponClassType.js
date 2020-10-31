function weaponClassType() {
    // CanAttack?   AttackAdjacent?   AttackRanged?   NeedsAmmoReadied?  ConsumesAmmo?
    //
    this.NotWeapon = 0,       //   No              No               No                N/A              N/A
    this.HandBlunt = 1,       //   Yes             Yes              Yes               No               No
    this.HandCutting = 2,     //   Yes             Yes              Yes               No               No
    this.HandThrow = 3,       //   Yes             Yes              Yes               No               if range > 1
    this.SlingNoAmmo = 4,     //   Yes             No               Yes               No               No
    this.Bow = 5,             //   Yes             No               Yes               Yes              Yes
    this.Crossbow = 6,        //   Yes             No               Yes               Yes              Yes
    this.Throw = 7,           //   Yes             No               Yes               No               Yes
    this.Ammo = 8,            //   No              N/A              No                N/A              N/A
    this.SpellCaster = 9,     //   Yes             Yes              Yes               No               Yes
    this.SpellLikeAbility = 10//   Yes             YEs              Yes               No               Yes

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
}

function weaponClassTypeObj() {
    weaponClassType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}



var MAX_WEAPON_CLASS = 11;
